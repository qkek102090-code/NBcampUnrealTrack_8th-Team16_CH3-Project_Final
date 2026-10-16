#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StockMarketSubsystem.h"
#include "StockDetailWidget.generated.h"

class UTextBlock;
class UButton;
class UEditableTextBox;
class UStockChartWidget;
class UPriceAxisWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStockDetailBackRequested);

UENUM(BlueprintType)
enum class EStockTradeMode : uint8
{
	Buy,
	Sell
};

// 종목 리스트에서 종목을 클릭했을 때 들어가는 상세 화면.
// 차트 + 가격 눈금 + 매수/매도 탭 + 수량 조절 + 주문 요약 + 실행 버튼을 전부 여기서 관리합니다.
UCLASS()
class ALPHA_API UStockDetailWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 뒤로가기 버튼을 눌렀을 때 발생. 부모(홈 화면)가 이걸 구독해서 리스트로 돌아갑니다.
	UPROPERTY(BlueprintAssignable)
	FOnStockDetailBackRequested OnBackRequested;

	// 이 종목의 상세 화면을 열어서 모든 정보를 채웁니다. 리스트에서 종목 클릭 시 호출하세요.
	UFUNCTION(BlueprintCallable, Category = "Stock")
	void ShowStock(FName Symbol);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SelectedSymbolText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SelectedPriceText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> StartPriceText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HoldingQuantityText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MessageText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UStockChartWidget> PriceChart;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UPriceAxisWidget> PriceAxis;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UEditableTextBox> QuantityInputBox;

	// 매수/매도 탭. 실제 주문은 안 하고 모드만 바뀝니다 (사진처럼).
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> BuyTabButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> SellTabButton;

	// "예상 체결가 (n개) 시장가  ###원" 형태
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EstimatedFillPriceText;

	// "예상 수수료  +0.30%" 형태
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EstimatedFeeText;

	// "주문 합계  ###원" 형태
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> OrderTotalText;

	// 실제 주문을 실행하는 하나의 큰 버튼. 텍스트가 "{수량}개 매수"/"{수량}개 매도"로 자동 바뀝니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> ExecuteOrderButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ExecuteOrderButtonText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> BackButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> QtyPlus1Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> QtyPlus10Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> QtyMinus1Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> QtyMinus10Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> QtySet1Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> QtySet10Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> QtySet50Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> QtySetMaxButton;

	// --- 매수/매도 탭 색상 (여기서 직접 조정하세요. 디자이너의 Style > Normal 색은
	// 이 값들이 코드에서 매번 덮어쓰기 때문에 의미가 없습니다) ---
	UPROPERTY(EditAnywhere, Category = "Trade")
	FLinearColor BuyColor = FLinearColor(0.85f, 0.2f, 0.2f);

	UPROPERTY(EditAnywhere, Category = "Trade")
	FLinearColor SellColor = FLinearColor(0.25f, 0.45f, 0.95f);

	UPROPERTY(EditAnywhere, Category = "Trade")
	FLinearColor InactiveTradeTabColor = FLinearColor(0.6f, 0.6f, 0.6f);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "Stock")
	void RefreshSelectedStockInfo();

	UFUNCTION(BlueprintCallable, Category = "Stock")
	void RefreshPriceChart();

	// 예상 체결가 / 수수료 / 합계 / 버튼 텍스트를 전부 다시 계산해서 채웁니다.
	UFUNCTION(BlueprintCallable, Category = "Stock")
	void UpdateOrderSummary();

	UFUNCTION()
	void HandlePriceUpdated(FName Symbol, float NewPrice);

	UFUNCTION()
	void HandleAssetChanged();

	UFUNCTION()
	void HandleAllPricesUpdated();

	UFUNCTION()
	void HandleTradeResult(bool bSuccess, FName Symbol, FText Message);

	void ShowTemporaryMessage(const FText& Message);
	void ClearMessageText();

	UFUNCTION()
	void SetTradeModeBuy();

	UFUNCTION()
	void SetTradeModeSell();

	UFUNCTION()
	void HandleExecuteOrderClicked();

	void UpdateTradeModeHighlight();

	UFUNCTION()
	void HandleBackClicked();

	UFUNCTION()
	void HandleQtyPlus1();

	UFUNCTION()
	void HandleQtyPlus10();

	UFUNCTION()
	void HandleQtyMinus1();

	UFUNCTION()
	void HandleQtyMinus10();

	UFUNCTION()
	void HandleQtySet1();

	UFUNCTION()
	void HandleQtySet10();

	UFUNCTION()
	void HandleQtySet50();

	UFUNCTION()
	void HandleQtySetMax();

	void AdjustQuantity(int32 Delta);
	void SetQuantity(int32 Value);
	int32 GetCurrentQuantity() const;

	UPROPERTY(BlueprintReadOnly, Category = "Stock")
	FName SelectedSymbol;

	UPROPERTY(BlueprintReadOnly, Category = "Stock")
	EStockTradeMode CurrentTradeMode = EStockTradeMode::Buy;

	UPROPERTY()
	TObjectPtr<UStockMarketSubsystem> CachedSubsystem;

	FTimerHandle MessageTimerHandle;
};
