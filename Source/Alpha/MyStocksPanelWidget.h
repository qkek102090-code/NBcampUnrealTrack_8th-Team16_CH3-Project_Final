#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StockMarketSubsystem.h"
#include "MyStocksPanelWidget.generated.h"

class UListView;
class UUserWidget;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMyStockSelected, FName, Symbol);

// "모든 주식" 탭에 항상 같이 떠 있는 작은 보유 종목 미리보기 패널.
// 여기서 종목을 클릭하면 OnStockSelected가 발생하고, 부모(StockMarketWidget)가
// 그걸 받아서 상세 화면으로 진입시킵니다.
UCLASS()
class ALPHA_API UMyStocksPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnMyStockSelected OnStockSelected;

	UFUNCTION(BlueprintCallable, Category = "Stock")
	void RefreshMyStocksList();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// 블루프린트 자식 위젯에서 이 이름으로 ListView를 배치해주세요.
	// Entry Widget Class는 WBP_MyStockEntry(부모: MyStockEntryWidget)로 지정하세요.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UListView> MyStocksListView;

	// 보유 중인 모든 종목의 현재가 기준 평가액 합계
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TotalValueText;

	// 평가액 합계에서 매수 원가 합계를 뺀 손익 (금액 + 퍼센트)
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TotalGainText;

	// 보유 중인 종목이 하나도 없을 때 표시되는 안내 문구
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EmptyStateText;

	void HandleEntryWidgetGenerated(UUserWidget& EntryWidget);

	UFUNCTION()
	void HandleEntryClicked(FName Symbol);

	UFUNCTION()
	void HandleAssetChanged();

	UFUNCTION()
	void HandleAllPricesUpdated();

	UPROPERTY()
	TObjectPtr<UStockMarketSubsystem> CachedSubsystem;
};
