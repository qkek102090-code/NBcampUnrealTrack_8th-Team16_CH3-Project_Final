#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/DataTable.h"
#include "StockDefinition.h"
#include "ShopItemDefinition.h"
#include "DonationOrgDefinition.h"
#include "StockMarketSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FCandleData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float Open = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float High = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float Low = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float Close = 0.f;
};

USTRUCT(BlueprintType)
struct FStockData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FName Symbol;

	UPROPERTY(BlueprintReadOnly)
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly)
	float CurrentPrice = 100.f;

	UPROPERTY(BlueprintReadOnly)
	float Momentum = 0.f;

	UPROPERTY(BlueprintReadOnly)
	TArray<float> PriceHistory;

	UPROPERTY(BlueprintReadOnly)
	float VolatilityMultiplier = 1.0f;

	UPROPERTY(BlueprintReadOnly)
	float TradingActivity = 1.0f;

	UPROPERTY(BlueprintReadOnly)
	float GrowthBias = 0.f;

	UPROPERTY(BlueprintReadOnly)
	float RangeReversionStrength = 0.02f;

	UPROPERTY(BlueprintReadOnly)
	float StartPrice = 100.f;

	// --- 캔들차트용 데이터 ---

	UPROPERTY(BlueprintReadOnly)
	TArray<FCandleData> CandleHistory;

	UPROPERTY(BlueprintReadOnly)
	FCandleData CurrentCandle;

	UPROPERTY(BlueprintReadOnly)
	int32 TicksInCurrentCandle = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPriceUpdated, FName, Symbol, float, NewPrice);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAssetChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTradeResult, bool, bSuccess, FName, Symbol, FText, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReputationChanged, float, NewReputation);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllPricesUpdated);

UCLASS()
class ALPHA_API UStockMarketSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// --- UI가 구독할 델리게이트 ---
	UPROPERTY(BlueprintAssignable)
	FOnPriceUpdated OnPriceUpdated;

	UPROPERTY(BlueprintAssignable)
	FOnAssetChanged OnAssetChanged;

	UPROPERTY(BlueprintAssignable)
	FOnTradeResult OnTradeResult;

	UPROPERTY(BlueprintAssignable)
	FOnReputationChanged OnReputationChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAllPricesUpdated OnAllPricesUpdated;

	// --- 조회 ---
	UFUNCTION(BlueprintCallable, Category = "Stock")
	float GetCurrentPrice(FName Symbol) const;

	UFUNCTION(BlueprintCallable, Category = "Stock")
	float GetStartPrice(FName Symbol) const;

	UFUNCTION(BlueprintCallable, Category = "Stock")
	FText GetStockDisplayName(FName Symbol) const;

	UFUNCTION(BlueprintCallable, Category = "Stock")
	float GetTransactionFeeRate() const { return TransactionFeeRate; }

	UFUNCTION(BlueprintCallable, Category = "Stock")
	TArray<float> GetPriceHistory(FName Symbol) const;

	UFUNCTION(BlueprintCallable, Category = "Stock")
	TArray<FCandleData> GetCandleHistory(FName Symbol) const;

	UFUNCTION(BlueprintCallable, Category = "Stock")
	TArray<FStockData> GetAllStocks() const;

	UFUNCTION(BlueprintCallable, Category = "Stock")
	float GetCash() const { return Cash; }

	UFUNCTION(BlueprintCallable, Category = "Stock")
	int32 GetHoldingQuantity(FName Symbol) const;

	// 이 종목을 지금까지 매수한 평균 가격 (가중평균). 보유 안 하고 있으면 0을 반환합니다.
	UFUNCTION(BlueprintCallable, Category = "Stock")
	float GetAverageBuyPrice(FName Symbol) const { return HoldingAverageCost.FindRef(Symbol); }

	UFUNCTION(BlueprintCallable, Category = "Shop")
	float GetReputation() const { return Reputation; }

	// --- 거래 (이 두 함수 안에서 모든 검증이 일어납니다) ---
	UFUNCTION(BlueprintCallable, Category = "Stock")
	bool BuyStock(FName Symbol, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Stock")
	bool SellStock(FName Symbol, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Stock")
	void LoadStocksFromDataTable(UDataTable* DataTable);

	// --- 상점 / 인지도 시스템 ---

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void LoadShopItemsFromDataTable(UDataTable* DataTable);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	TArray<FShopItemRow> GetAllShopItems() const;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool IsItemUnlocked(FName ItemID) const;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool IsItemOwned(FName ItemID) const;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool PurchaseShopItem(FName ItemID);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool Donate(float Amount);

	// --- 기부 단체 ---

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void LoadDonationOrgsFromDataTable(UDataTable* DataTable);

	UFUNCTION(BlueprintCallable, Category = "Shop")
	TArray<FDonationOrgRow> GetAllDonationOrgs() const;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	bool DonateToOrg(FName OrgID);

	// --- 은행 대출 ---

	UFUNCTION(BlueprintCallable, Category = "Bank")
	float GetMaxLoanAmount() const;

	UFUNCTION(BlueprintCallable, Category = "Bank")
	float GetCurrentDebt() const { return CurrentDebt; }

	UFUNCTION(BlueprintCallable, Category = "Bank")
	float GetAvailableLoanAmount() const;

	UFUNCTION(BlueprintCallable, Category = "Bank")
	bool BorrowLoan(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Bank")
	bool RepayLoan(float Amount);

	// 미니게임 보상 등, 거래를 거치지 않고 예수금을 직접 늘려야 할 때 씁니다.
	UFUNCTION(BlueprintCallable, Category = "Stock")
	void AddCash(float Amount);

	// --- 관심 주식 ---

	UFUNCTION(BlueprintCallable, Category = "Stock")
	bool IsInWatchlist(FName Symbol) const { return Watchlist.Contains(Symbol); }

	UFUNCTION(BlueprintCallable, Category = "Stock")
	void ToggleWatchlist(FName Symbol);

	UFUNCTION(BlueprintCallable, Category = "Stock")
	TArray<FName> GetWatchlistSymbols() const;

	UPROPERTY(BlueprintAssignable)
	FOnAssetChanged OnWatchlistChanged;

	// 게임 시작 직후, 화면에 보이기 전에 가격을 N틱만큼 미리 돌려서 캔들 히스토리가
	// 어느 정도 쌓인 상태로 시작하게 만듭니다. DataTable 로드 직후에 한 번만 호출하세요.
	UFUNCTION(BlueprintCallable, Category = "Stock")
	void PreSimulatePriceHistory(int32 TickCount);

protected:
	void UpdatePrices();

	float CalculateNextPrice(FStockData& Stock) const;

	UPROPERTY()
	TMap<FName, FStockData> Stocks;

	UPROPERTY()
	TMap<FName, int32> Holdings;

	// 종목별 가중평균 매수가. 매수할 때만 갱신되고, 매도할 때는 안 바뀝니다.
	UPROPERTY()
	TMap<FName, float> HoldingAverageCost;

	UPROPERTY()
	float Cash = 1000000.f;

	// --- 상점 / 인지도 상태 ---

	UPROPERTY()
	float Reputation = 0.f;

	UPROPERTY()
	TMap<FName, FShopItemRow> ShopItems;

	UPROPERTY()
	TSet<FName> OwnedItemIDs;

	UPROPERTY()
	TMap<FName, FDonationOrgRow> DonationOrgs;

	UPROPERTY()
	TSet<FName> Watchlist;

	// --- 은행 대출 상태 ---

	UPROPERTY()
	float CurrentDebt = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Bank|Config")
	float LoanAmountPerReputation = 10000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Shop|Config")
	float ReputationPerDonationCurrency = 0.0001f;

	FTimerHandle PriceUpdateTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Stock|Config")
	float PriceUpdateInterval = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stock|Config")
	int32 TicksPerCandle = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Stock|Config")
	float MomentumDecayRate = 0.9f;

	UPROPERTY(EditDefaultsOnly, Category = "Stock|Config")
	float MaxMomentumMagnitude = 0.6f;

	UPROPERTY(EditDefaultsOnly, Category = "Stock|Config")
	float MomentumPriceImpact = 0.006f;

	UPROPERTY(EditDefaultsOnly, Category = "Stock|Config")
	float ReferencePriceForVolatility = 50000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Stock|Config")
	float TransactionFeeRate = 0.003f;
};