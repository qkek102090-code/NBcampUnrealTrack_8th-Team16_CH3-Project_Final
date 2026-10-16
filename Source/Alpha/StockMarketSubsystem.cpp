#include "StockMarketSubsystem.h"

void UStockMarketSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);


	if (UGameInstance* GI = GetGameInstance())
	{
		if (UWorld* World = GI->GetWorld())
		{
			World->GetTimerManager().SetTimer(
				PriceUpdateTimerHandle,
				this,
				&UStockMarketSubsystem::UpdatePrices,
				PriceUpdateInterval,
				true
			);
		}
	}
}

void UStockMarketSubsystem::Deinitialize()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UWorld* World = GI->GetWorld())
		{
			World->GetTimerManager().ClearTimer(PriceUpdateTimerHandle);
		}
	}
	Super::Deinitialize();
}

float UStockMarketSubsystem::CalculateNextPrice(FStockData& Stock) const
{
	const float MomentumDrift = FMath::FRandRange(-0.05f, 0.05f);
	Stock.Momentum = FMath::Clamp(
		Stock.Momentum * MomentumDecayRate + MomentumDrift,
		-MaxMomentumMagnitude,
		MaxMomentumMagnitude
	);

	const float PriceRatio = FMath::Max(Stock.CurrentPrice / ReferencePriceForVolatility, 0.01f);
	const float PriceDampening = 1.0f / (1.0f + FMath::Max(0.f, FMath::LogX(10.f, PriceRatio)));

	const float RandomFactor = FMath::FRandRange(-0.015f, 0.015f) * Stock.VolatilityMultiplier * PriceDampening;

	const float MomentumFactor = Stock.Momentum * MomentumPriceImpact * Stock.VolatilityMultiplier * PriceDampening;

	const float DeviationFromStart = (Stock.CurrentPrice - Stock.StartPrice) / Stock.StartPrice;
	const float RangeReversionFactor = -DeviationFromStart * Stock.RangeReversionStrength;

	const float ChangeRate = RandomFactor + MomentumFactor + RangeReversionFactor + Stock.GrowthBias;
	float NewPrice = Stock.CurrentPrice * (1.0f + ChangeRate);

	NewPrice = FMath::Max(NewPrice, 1.0f);

	return NewPrice;
}

void UStockMarketSubsystem::UpdatePrices()
{
	for (auto& Pair : Stocks)
	{
		FStockData& Stock = Pair.Value;

		if (FMath::FRand() > Stock.TradingActivity)
		{
			continue;
		}

		const float NewPrice = CalculateNextPrice(Stock);

		Stock.CurrentPrice = NewPrice;
		Stock.PriceHistory.Add(NewPrice);

		constexpr int32 MaxHistoryLength = 200;
		if (Stock.PriceHistory.Num() > MaxHistoryLength)
		{
			Stock.PriceHistory.RemoveAt(0);
		}

		// --- 캔들 갱신 ---
		if (Stock.TicksInCurrentCandle == 0)
		{
			Stock.CurrentCandle.Open = NewPrice;
			Stock.CurrentCandle.High = NewPrice;
			Stock.CurrentCandle.Low = NewPrice;
		}

		Stock.CurrentCandle.High = FMath::Max(Stock.CurrentCandle.High, NewPrice);
		Stock.CurrentCandle.Low = FMath::Min(Stock.CurrentCandle.Low, NewPrice);
		Stock.CurrentCandle.Close = NewPrice;
		Stock.TicksInCurrentCandle++;

		if (Stock.TicksInCurrentCandle >= TicksPerCandle)
		{
			Stock.CandleHistory.Add(Stock.CurrentCandle);

			constexpr int32 MaxCandleHistoryLength = 100;
			if (Stock.CandleHistory.Num() > MaxCandleHistoryLength)
			{
				Stock.CandleHistory.RemoveAt(0);
			}

			Stock.TicksInCurrentCandle = 0;
		}

		OnPriceUpdated.Broadcast(Pair.Key, NewPrice);
	}

	OnAllPricesUpdated.Broadcast();
}

void UStockMarketSubsystem::PreSimulatePriceHistory(int32 TickCount)
{
	for (int32 Index = 0; Index < TickCount; ++Index)
	{
		UpdatePrices();
	}
}

void UStockMarketSubsystem::LoadStocksFromDataTable(UDataTable* DataTable)
{
	if (!DataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("StockMarketSubsystem: LoadStocksFromDataTable에 nullptr이 전달되었습니다."));
		return;
	}

	Stocks.Empty();

	TArray<FStockTableRow*> Rows;
	DataTable->GetAllRows<FStockTableRow>(TEXT("LoadStocksFromDataTable"), Rows);

	for (const FStockTableRow* Row : Rows)
	{
		if (!Row)
		{
			continue;
		}

		FStockData Data;
		Data.Symbol = Row->Symbol;
		Data.DisplayName = Row->DisplayName;
		Data.CurrentPrice = Row->StartPrice;
		Data.StartPrice = Row->StartPrice;
		Data.Momentum = 0.f;
		Data.VolatilityMultiplier = Row->VolatilityMultiplier;
		Data.TradingActivity = Row->TradingActivity;
		Data.GrowthBias = Row->GrowthBias;
		Data.RangeReversionStrength = Row->RangeReversionStrength;
		Data.PriceHistory.Add(Row->StartPrice);

		Stocks.Add(Row->Symbol, Data);
	}

	UE_LOG(LogTemp, Log, TEXT("StockMarketSubsystem: 종목 %d개를 DataTable에서 불러왔습니다."), Stocks.Num());

	OnAssetChanged.Broadcast();
}

float UStockMarketSubsystem::GetCurrentPrice(FName Symbol) const
{
	if (const FStockData* Found = Stocks.Find(Symbol))
	{
		return Found->CurrentPrice;
	}
	return 0.f;
}

float UStockMarketSubsystem::GetStartPrice(FName Symbol) const
{
	if (const FStockData* Found = Stocks.Find(Symbol))
	{
		return Found->StartPrice;
	}
	return 0.f;
}

FText UStockMarketSubsystem::GetStockDisplayName(FName Symbol) const
{
	if (const FStockData* Found = Stocks.Find(Symbol))
	{
		return Found->DisplayName;
	}
	return FText::GetEmpty();
}

TArray<float> UStockMarketSubsystem::GetPriceHistory(FName Symbol) const
{
	if (const FStockData* Found = Stocks.Find(Symbol))
	{
		return Found->PriceHistory;
	}
	return TArray<float>();
}

TArray<FCandleData> UStockMarketSubsystem::GetCandleHistory(FName Symbol) const
{
	if (const FStockData* Found = Stocks.Find(Symbol))
	{
		TArray<FCandleData> Result = Found->CandleHistory;

		if (Found->TicksInCurrentCandle > 0)
		{
			Result.Add(Found->CurrentCandle);
		}

		return Result;
	}
	return TArray<FCandleData>();
}

TArray<FStockData> UStockMarketSubsystem::GetAllStocks() const
{
	TArray<FStockData> Result;
	Stocks.GenerateValueArray(Result);
	return Result;
}

int32 UStockMarketSubsystem::GetHoldingQuantity(FName Symbol) const
{
	if (const int32* Found = Holdings.Find(Symbol))
	{
		return *Found;
	}
	return 0;
}

bool UStockMarketSubsystem::BuyStock(FName Symbol, int32 Quantity)
{
	if (Quantity <= 0)
	{
		OnTradeResult.Broadcast(false, Symbol, FText::FromString(TEXT("수량은 1 이상이어야 합니다.")));
		return false;
	}

	const FStockData* Stock = Stocks.Find(Symbol);
	if (!Stock)
	{
		OnTradeResult.Broadcast(false, Symbol, FText::FromString(TEXT("존재하지 않는 종목입니다.")));
		return false;
	}

	const float TotalCost = Stock->CurrentPrice * Quantity;
	const float Fee = TotalCost * TransactionFeeRate;
	const float FinalCost = TotalCost + Fee;

	if (Cash < FinalCost)
	{
		OnTradeResult.Broadcast(false, Symbol, FText::FromString(TEXT("예수금이 부족합니다.")));
		return false;
	}

	Cash -= FinalCost;

	// 평균 구매가를 가중평균으로 갱신합니다 (예: 10만원짜리 5주 + 12만원짜리 5주 -> 평균 11만원).
	const int32 OldQuantity = Holdings.FindRef(Symbol);
	const float OldAverageCost = HoldingAverageCost.FindRef(Symbol);
	const int32 NewQuantity = OldQuantity + Quantity;
	const float NewAverageCost = ((OldAverageCost * OldQuantity) + (Stock->CurrentPrice * Quantity)) / NewQuantity;

	Holdings.Add(Symbol, NewQuantity);
	HoldingAverageCost.Add(Symbol, NewAverageCost);

	OnAssetChanged.Broadcast();
	OnTradeResult.Broadcast(true, Symbol, FText::FromString(TEXT("매수 체결")));
	return true;
}

bool UStockMarketSubsystem::SellStock(FName Symbol, int32 Quantity)
{
	if (Quantity <= 0)
	{
		OnTradeResult.Broadcast(false, Symbol, FText::FromString(TEXT("수량은 1 이상이어야 합니다.")));
		return false;
	}

	const int32 CurrentHolding = GetHoldingQuantity(Symbol);
	if (CurrentHolding < Quantity)
	{
		OnTradeResult.Broadcast(false, Symbol, FText::FromString(TEXT("보유 수량이 부족합니다.")));
		return false;
	}

	const FStockData* Stock = Stocks.Find(Symbol);
	if (!Stock)
	{
		OnTradeResult.Broadcast(false, Symbol, FText::FromString(TEXT("존재하지 않는 종목입니다.")));
		return false;
	}

	const float TotalRevenue = Stock->CurrentPrice * Quantity;
	const float Fee = TotalRevenue * TransactionFeeRate;
	const float FinalRevenue = TotalRevenue - Fee;

	Cash += FinalRevenue;
	Holdings[Symbol] -= Quantity;

	OnAssetChanged.Broadcast();
	OnTradeResult.Broadcast(true, Symbol, FText::FromString(TEXT("매도 체결")));
	return true;
}

void UStockMarketSubsystem::LoadShopItemsFromDataTable(UDataTable* DataTable)
{
	if (!DataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("StockMarketSubsystem: LoadShopItemsFromDataTable에 nullptr이 전달되었습니다."));
		return;
	}

	ShopItems.Empty();

	TArray<FShopItemRow*> Rows;
	DataTable->GetAllRows<FShopItemRow>(TEXT("LoadShopItemsFromDataTable"), Rows);

	for (const FShopItemRow* Row : Rows)
	{
		if (Row)
		{
			ShopItems.Add(Row->ItemID, *Row);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("StockMarketSubsystem: 상점 아이템 %d개를 DataTable에서 불러왔습니다."), ShopItems.Num());
	OnAssetChanged.Broadcast();
}

TArray<FShopItemRow> UStockMarketSubsystem::GetAllShopItems() const
{
	TArray<FShopItemRow> Result;
	ShopItems.GenerateValueArray(Result);
	return Result;
}

bool UStockMarketSubsystem::IsItemUnlocked(FName ItemID) const
{
	const FShopItemRow* Item = ShopItems.Find(ItemID);
	if (!Item)
	{
		return false;
	}
	return Reputation >= Item->RequiredReputation;
}

bool UStockMarketSubsystem::IsItemOwned(FName ItemID) const
{
	return OwnedItemIDs.Contains(ItemID);
}

bool UStockMarketSubsystem::PurchaseShopItem(FName ItemID)
{
	const FShopItemRow* Item = ShopItems.Find(ItemID);
	if (!Item)
	{
		OnTradeResult.Broadcast(false, ItemID, FText::FromString(TEXT("존재하지 않는 아이템입니다.")));
		return false;
	}

	if (OwnedItemIDs.Contains(ItemID))
	{
		OnTradeResult.Broadcast(false, ItemID, FText::FromString(TEXT("이미 보유한 아이템입니다.")));
		return false;
	}

	if (Reputation < Item->RequiredReputation)
	{
		OnTradeResult.Broadcast(false, ItemID, FText::FromString(TEXT("인지도가 부족해 아직 구매할 수 없습니다.")));
		return false;
	}

	if (Cash < Item->Price)
	{
		OnTradeResult.Broadcast(false, ItemID, FText::FromString(TEXT("예수금이 부족합니다.")));
		return false;
	}

	Cash -= Item->Price;
	OwnedItemIDs.Add(ItemID);

	if (Item->ReputationReward > 0.f)
	{
		Reputation += Item->ReputationReward;
		OnReputationChanged.Broadcast(Reputation);
	}

	OnAssetChanged.Broadcast();
	OnTradeResult.Broadcast(true, ItemID, FText::FromString(TEXT("구매 완료")));
	return true;
}

bool UStockMarketSubsystem::Donate(float Amount)
{
	if (Amount <= 0.f)
	{
		OnTradeResult.Broadcast(false, NAME_None, FText::FromString(TEXT("기부 금액은 0보다 커야 합니다.")));
		return false;
	}

	if (Cash < Amount)
	{
		OnTradeResult.Broadcast(false, NAME_None, FText::FromString(TEXT("예수금이 부족합니다.")));
		return false;
	}

	Cash -= Amount;

	const float GainedReputation = Amount * ReputationPerDonationCurrency;
	Reputation += GainedReputation;

	OnAssetChanged.Broadcast();
	OnReputationChanged.Broadcast(Reputation);
	OnTradeResult.Broadcast(true, NAME_None, FText::FromString(TEXT("기부해주셔서 감사합니다.")));
	return true;
}

void UStockMarketSubsystem::LoadDonationOrgsFromDataTable(UDataTable* DataTable)
{
	if (!DataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("StockMarketSubsystem: LoadDonationOrgsFromDataTable에 nullptr이 전달되었습니다."));
		return;
	}

	DonationOrgs.Empty();

	TArray<FDonationOrgRow*> Rows;
	DataTable->GetAllRows<FDonationOrgRow>(TEXT("LoadDonationOrgsFromDataTable"), Rows);

	for (const FDonationOrgRow* Row : Rows)
	{
		if (Row)
		{
			DonationOrgs.Add(Row->OrgID, *Row);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("StockMarketSubsystem: 기부 단체 %d개를 DataTable에서 불러왔습니다."), DonationOrgs.Num());
}

TArray<FDonationOrgRow> UStockMarketSubsystem::GetAllDonationOrgs() const
{
	TArray<FDonationOrgRow> Result;
	DonationOrgs.GenerateValueArray(Result);
	return Result;
}

bool UStockMarketSubsystem::DonateToOrg(FName OrgID)
{
	const FDonationOrgRow* Org = DonationOrgs.Find(OrgID);
	if (!Org)
	{
		OnTradeResult.Broadcast(false, OrgID, FText::FromString(TEXT("존재하지 않는 기부 단체입니다.")));
		return false;
	}

	const float Amount = Org->DonationAmount;

	if (Cash < Amount)
	{
		OnTradeResult.Broadcast(false, OrgID, FText::FromString(TEXT("예수금이 부족합니다.")));
		return false;
	}

	Cash -= Amount;

	const float GainedReputation = Amount * ReputationPerDonationCurrency;
	Reputation += GainedReputation;

	OnAssetChanged.Broadcast();
	OnReputationChanged.Broadcast(Reputation);

	const FText Message = FText::Format(
		NSLOCTEXT("Donation", "DonateSuccessFormat", "{0}에 {1}원을 기부해주셔서 감사합니다."),
		Org->OrgName,
		FText::AsNumber(FMath::RoundToInt(Amount))
	);
	OnTradeResult.Broadcast(true, OrgID, Message);
	return true;
}

float UStockMarketSubsystem::GetMaxLoanAmount() const
{
	return (Reputation / 10.f) * LoanAmountPerReputation;
}

float UStockMarketSubsystem::GetAvailableLoanAmount() const
{
	return FMath::Max(GetMaxLoanAmount() - CurrentDebt, 0.f);
}

bool UStockMarketSubsystem::BorrowLoan(float Amount)
{
	if (Amount <= 0.f)
	{
		OnTradeResult.Broadcast(false, NAME_None, FText::FromString(TEXT("대출 금액은 0보다 커야 합니다.")));
		return false;
	}

	if (Amount > GetAvailableLoanAmount())
	{
		OnTradeResult.Broadcast(false, NAME_None, FText::FromString(TEXT("인지도 기준 대출 한도를 초과했습니다.")));
		return false;
	}

	Cash += Amount;
	CurrentDebt += Amount;

	OnAssetChanged.Broadcast();
	OnTradeResult.Broadcast(true, NAME_None, FText::FromString(TEXT("대출이 실행되었습니다.")));
	return true;
}

bool UStockMarketSubsystem::RepayLoan(float Amount)
{
	if (Amount <= 0.f)
	{
		OnTradeResult.Broadcast(false, NAME_None, FText::FromString(TEXT("상환 금액은 0보다 커야 합니다.")));
		return false;
	}

	if (Amount > CurrentDebt)
	{
		OnTradeResult.Broadcast(false, NAME_None, FText::FromString(TEXT("남은 채무보다 많은 금액을 상환할 수 없습니다.")));
		return false;
	}

	if (Cash < Amount)
	{
		OnTradeResult.Broadcast(false, NAME_None, FText::FromString(TEXT("예수금이 부족합니다.")));
		return false;
	}

	Cash -= Amount;
	CurrentDebt -= Amount;

	OnAssetChanged.Broadcast();
	OnTradeResult.Broadcast(true, NAME_None, FText::FromString(TEXT("대출을 상환했습니다.")));
	return true;
}

void UStockMarketSubsystem::ToggleWatchlist(FName Symbol)
{
	if (Watchlist.Contains(Symbol))
	{
		Watchlist.Remove(Symbol);
	}
	else
	{
		Watchlist.Add(Symbol);
	}

	OnWatchlistChanged.Broadcast();
}

TArray<FName> UStockMarketSubsystem::GetWatchlistSymbols() const
{
	return Watchlist.Array();
}

void UStockMarketSubsystem::AddCash(float Amount)
{
	if (Amount <= 0.f)
	{
		return;
	}

	Cash += Amount;
	OnAssetChanged.Broadcast();
}