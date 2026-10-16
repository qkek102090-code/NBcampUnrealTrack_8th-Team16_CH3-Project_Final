#include "StockDetailWidget.h"
#include "StockChartWidget.h"
#include "PriceAxisWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"

void UStockDetailWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GI = GetGameInstance())
	{
		CachedSubsystem = GI->GetSubsystem<UStockMarketSubsystem>();
	}

	if (!CachedSubsystem)
	{
		return;
	}

	CachedSubsystem->OnPriceUpdated.AddDynamic(this, &UStockDetailWidget::HandlePriceUpdated);
	CachedSubsystem->OnAssetChanged.AddDynamic(this, &UStockDetailWidget::HandleAssetChanged);
	CachedSubsystem->OnAllPricesUpdated.AddDynamic(this, &UStockDetailWidget::HandleAllPricesUpdated);
	CachedSubsystem->OnTradeResult.AddDynamic(this, &UStockDetailWidget::HandleTradeResult);

	if (BuyTabButton)
	{
		BuyTabButton->OnClicked.AddDynamic(this, &UStockDetailWidget::SetTradeModeBuy);
	}
	if (SellTabButton)
	{
		SellTabButton->OnClicked.AddDynamic(this, &UStockDetailWidget::SetTradeModeSell);
	}
	if (ExecuteOrderButton)
	{
		ExecuteOrderButton->OnClicked.AddDynamic(this, &UStockDetailWidget::HandleExecuteOrderClicked);
	}
	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UStockDetailWidget::HandleBackClicked);
	}
	if (QtyPlus1Button)
	{
		QtyPlus1Button->OnClicked.AddDynamic(this, &UStockDetailWidget::HandleQtyPlus1);
	}
	if (QtyPlus10Button)
	{
		QtyPlus10Button->OnClicked.AddDynamic(this, &UStockDetailWidget::HandleQtyPlus10);
	}
	if (QtyMinus1Button)
	{
		QtyMinus1Button->OnClicked.AddDynamic(this, &UStockDetailWidget::HandleQtyMinus1);
	}
	if (QtyMinus10Button)
	{
		QtyMinus10Button->OnClicked.AddDynamic(this, &UStockDetailWidget::HandleQtyMinus10);
	}
	if (QtySet1Button)
	{
		QtySet1Button->OnClicked.AddDynamic(this, &UStockDetailWidget::HandleQtySet1);
	}
	if (QtySet10Button)
	{
		QtySet10Button->OnClicked.AddDynamic(this, &UStockDetailWidget::HandleQtySet10);
	}
	if (QtySet50Button)
	{
		QtySet50Button->OnClicked.AddDynamic(this, &UStockDetailWidget::HandleQtySet50);
	}
	if (QtySetMaxButton)
	{
		QtySetMaxButton->OnClicked.AddDynamic(this, &UStockDetailWidget::HandleQtySetMax);
	}

	UpdateTradeModeHighlight();
}

void UStockDetailWidget::NativeDestruct()
{
	if (CachedSubsystem)
	{
		CachedSubsystem->OnPriceUpdated.RemoveDynamic(this, &UStockDetailWidget::HandlePriceUpdated);
		CachedSubsystem->OnAssetChanged.RemoveDynamic(this, &UStockDetailWidget::HandleAssetChanged);
		CachedSubsystem->OnAllPricesUpdated.RemoveDynamic(this, &UStockDetailWidget::HandleAllPricesUpdated);
		CachedSubsystem->OnTradeResult.RemoveDynamic(this, &UStockDetailWidget::HandleTradeResult);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MessageTimerHandle);
	}

	Super::NativeDestruct();
}

void UStockDetailWidget::ShowStock(FName Symbol)
{
	SelectedSymbol = Symbol;
	CurrentTradeMode = EStockTradeMode::Buy;
	UpdateTradeModeHighlight();
	SetQuantity(1);
	RefreshSelectedStockInfo();
	RefreshPriceChart();
}

void UStockDetailWidget::RefreshSelectedStockInfo()
{
	if (!CachedSubsystem || SelectedSymbol.IsNone())
	{
		return;
	}

	const float Price = CachedSubsystem->GetCurrentPrice(SelectedSymbol);
	const float StartPrice = CachedSubsystem->GetStartPrice(SelectedSymbol);
	const int32 HoldingQty = CachedSubsystem->GetHoldingQuantity(SelectedSymbol);

	if (SelectedSymbolText)
	{
		SelectedSymbolText->SetText(FText::FromName(SelectedSymbol));
	}
	if (SelectedPriceText)
	{
		SelectedPriceText->SetText(FText::AsNumber(FMath::RoundToInt(Price)));
	}
	if (StartPriceText)
	{
		StartPriceText->SetText(FText::AsNumber(FMath::RoundToInt(StartPrice)));
	}
	if (HoldingQuantityText)
	{
		HoldingQuantityText->SetText(FText::AsNumber(HoldingQty));
	}

	UpdateOrderSummary();
}

void UStockDetailWidget::RefreshPriceChart()
{
	if (!CachedSubsystem || SelectedSymbol.IsNone())
	{
		return;
	}

	const TArray<FCandleData> Candles = CachedSubsystem->GetCandleHistory(SelectedSymbol);

	if (PriceChart)
	{
		PriceChart->SetCandles(Candles);
	}

	if (PriceAxis && Candles.Num() > 0)
	{
		const int32 TargetVisible = PriceChart ? FMath::Max(PriceChart->MaxVisibleCandles, 1) : Candles.Num();
		const int32 VisibleCount = FMath::Min(Candles.Num(), TargetVisible);
		const int32 StartIndex = Candles.Num() - VisibleCount;

		float MinPrice = Candles[StartIndex].Low;
		float MaxPrice = Candles[StartIndex].High;
		for (int32 Index = StartIndex; Index < Candles.Num(); ++Index)
		{
			MinPrice = FMath::Min(MinPrice, Candles[Index].Low);
			MaxPrice = FMath::Max(MaxPrice, Candles[Index].High);
		}

		const float CurrentPrice = CachedSubsystem->GetCurrentPrice(SelectedSymbol);
		PriceAxis->SetPriceRange(MinPrice, MaxPrice, CurrentPrice);
	}
}

void UStockDetailWidget::UpdateOrderSummary()
{
	if (!CachedSubsystem || SelectedSymbol.IsNone())
	{
		return;
	}

	const int32 Quantity = GetCurrentQuantity();
	const float Price = CachedSubsystem->GetCurrentPrice(SelectedSymbol);
	const float FeeRate = CachedSubsystem->GetTransactionFeeRate();

	const float FillPrice = Price * Quantity;
	const float Fee = FillPrice * FeeRate;
	const float OrderTotal = FillPrice + Fee;

	if (EstimatedFillPriceText)
	{
		EstimatedFillPriceText->SetText(FText::Format(
			NSLOCTEXT("Stock", "EstimatedFillPriceFormat", "예상 체결가 ({0}개) 시장가  {1}원"),
			FText::AsNumber(Quantity),
			FText::AsNumber(FMath::RoundToInt(FillPrice))
		));
	}

	if (EstimatedFeeText)
	{
		EstimatedFeeText->SetText(FText::Format(
			NSLOCTEXT("Stock", "EstimatedFeeFormat", "예상 수수료  +{0}%"),
			FText::AsNumber(FeeRate * 100.f)
		));
	}

	if (OrderTotalText)
	{
		OrderTotalText->SetText(FText::Format(
			NSLOCTEXT("Stock", "OrderTotalFormat", "주문 합계  {0}원"),
			FText::AsNumber(FMath::RoundToInt(OrderTotal))
		));
	}

	if (ExecuteOrderButtonText)
	{
		const FText ActionText = (CurrentTradeMode == EStockTradeMode::Buy)
			? NSLOCTEXT("Stock", "BuyActionFormat", "{0}개 매수")
			: NSLOCTEXT("Stock", "SellActionFormat", "{0}개 매도");

		ExecuteOrderButtonText->SetText(FText::Format(ActionText, FText::AsNumber(Quantity)));
	}
}

void UStockDetailWidget::HandlePriceUpdated(FName Symbol, float NewPrice)
{
	if (Symbol == SelectedSymbol && SelectedPriceText)
	{
		SelectedPriceText->SetText(FText::AsNumber(FMath::RoundToInt(NewPrice)));
		UpdateOrderSummary();
	}
}

void UStockDetailWidget::HandleAssetChanged()
{
	RefreshSelectedStockInfo();
}

void UStockDetailWidget::HandleAllPricesUpdated()
{
	RefreshPriceChart();
	UpdateOrderSummary();
}

void UStockDetailWidget::HandleTradeResult(bool bSuccess, FName Symbol, FText Message)
{
	ShowTemporaryMessage(Message);
}

void UStockDetailWidget::ShowTemporaryMessage(const FText& Message)
{
	if (!MessageText)
	{
		return;
	}

	MessageText->SetText(Message);
	MessageText->SetVisibility(ESlateVisibility::Visible);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(MessageTimerHandle, this, &UStockDetailWidget::ClearMessageText, 3.0f, false);
	}
}

void UStockDetailWidget::ClearMessageText()
{
	if (MessageText)
	{
		MessageText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UStockDetailWidget::SetTradeModeBuy()
{
	CurrentTradeMode = EStockTradeMode::Buy;
	UpdateTradeModeHighlight();
	UpdateOrderSummary();
}

void UStockDetailWidget::SetTradeModeSell()
{
	CurrentTradeMode = EStockTradeMode::Sell;
	UpdateTradeModeHighlight();
	UpdateOrderSummary();
}

void UStockDetailWidget::UpdateTradeModeHighlight()
{
	// 색상값은 클래스 프로퍼티(BuyColor/SellColor/InactiveTradeTabColor)로 빠져있으니,
	// 색을 바꾸고 싶으면 여기가 아니라 WBP의 Class Defaults에서 직접 조정하세요.
	const bool bIsBuyMode = (CurrentTradeMode == EStockTradeMode::Buy);

	if (BuyTabButton)
	{
		BuyTabButton->SetBackgroundColor(bIsBuyMode ? BuyColor : InactiveTradeTabColor);
	}
	if (SellTabButton)
	{
		SellTabButton->SetBackgroundColor(bIsBuyMode ? InactiveTradeTabColor : SellColor);
	}
	if (ExecuteOrderButton)
	{
		ExecuteOrderButton->SetBackgroundColor(bIsBuyMode ? BuyColor : SellColor);
	}
}

void UStockDetailWidget::HandleExecuteOrderClicked()
{
	if (!CachedSubsystem || SelectedSymbol.IsNone())
	{
		return;
	}

	const int32 Quantity = GetCurrentQuantity();

	if (CurrentTradeMode == EStockTradeMode::Buy)
	{
		CachedSubsystem->BuyStock(SelectedSymbol, Quantity);
	}
	else
	{
		CachedSubsystem->SellStock(SelectedSymbol, Quantity);
	}
}

void UStockDetailWidget::HandleBackClicked()
{
	OnBackRequested.Broadcast();
}

void UStockDetailWidget::HandleQtyPlus1()
{
	AdjustQuantity(1);
}

void UStockDetailWidget::HandleQtyPlus10()
{
	AdjustQuantity(10);
}

void UStockDetailWidget::HandleQtyMinus1()
{
	AdjustQuantity(-1);
}

void UStockDetailWidget::HandleQtyMinus10()
{
	AdjustQuantity(-10);
}

void UStockDetailWidget::HandleQtySet1()
{
	SetQuantity(1);
}

void UStockDetailWidget::HandleQtySet10()
{
	SetQuantity(10);
}

void UStockDetailWidget::HandleQtySet50()
{
	SetQuantity(50);
}

void UStockDetailWidget::HandleQtySetMax()
{
	if (!QuantityInputBox || !CachedSubsystem || SelectedSymbol.IsNone())
	{
		return;
	}

	if (CurrentTradeMode == EStockTradeMode::Sell)
	{
		// 매도 모드에서는 "최대" = 지금 보유한 수량 전부
		SetQuantity(CachedSubsystem->GetHoldingQuantity(SelectedSymbol));
		return;
	}

	const float Price = CachedSubsystem->GetCurrentPrice(SelectedSymbol);
	if (Price <= 0.f)
	{
		return;
	}

	const int32 MaxAffordable = FMath::FloorToInt(CachedSubsystem->GetCash() / Price);
	SetQuantity(FMath::Max(MaxAffordable, 0));
}

void UStockDetailWidget::AdjustQuantity(int32 Delta)
{
	SetQuantity(GetCurrentQuantity() + Delta);
}

void UStockDetailWidget::SetQuantity(int32 Value)
{
	if (!QuantityInputBox)
	{
		return;
	}

	QuantityInputBox->SetText(FText::AsNumber(FMath::Max(Value, 0)));
	UpdateOrderSummary();
}

int32 UStockDetailWidget::GetCurrentQuantity() const
{
	if (!QuantityInputBox)
	{
		return 0;
	}

	const int32 Parsed = FCString::Atoi(*QuantityInputBox->GetText().ToString());
	return FMath::Max(Parsed, 0);
}
