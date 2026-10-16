#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "MyStockEntryWidget.generated.h"

class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMyStockEntryClicked, FName, Symbol);

// "내 투자" 패널의 한 줄. 시작가 대비가 아니라 "내가 산 가격" 대비 등락을 보여줍니다.
UCLASS()
class ALPHA_API UMyStockEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnMyStockEntryClicked OnEntryClicked;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SymbolText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> NameText;

	// 현재가 한 줄 (예: "6,970원")
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PriceText;

	// 구매가 대비 등락 한 줄 (예: "+4,586원 (192.35%)"). 오르면 초록, 내리면 빨강으로 자동 색상.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> GainText;

	// 보유 수량 (예: "5주")
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> QuantityText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> SelectButton;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UFUNCTION()
	void HandleSelectButtonClicked();

private:
	UPROPERTY()
	FName CachedSymbol;
};
