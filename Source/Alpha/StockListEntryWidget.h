#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "StockListEntryWidget.generated.h"

class UTextBlock;
class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStockEntryClicked, FName, Symbol);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWatchlistToggled, FName, Symbol);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStockEntryHovered, FName, Symbol);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStockEntryUnhovered);

UCLASS()
class ALPHA_API UStockListEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnStockEntryClicked OnEntryClicked;

	UPROPERTY(BlueprintAssignable)
	FOnWatchlistToggled OnWatchlistToggled;

	// 마우스를 올렸을 때/뗐을 때. SelectButton의 Hovered/Unhovered 이벤트를 그대로 씁니다.
	UPROPERTY(BlueprintAssignable)
	FOnStockEntryHovered OnEntryHovered;

	UPROPERTY(BlueprintAssignable)
	FOnStockEntryUnhovered OnEntryUnhovered;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SymbolText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> NameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PriceText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> PercentChangeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> SelectButton;

	// 관심 주식 등록/해제용 별 버튼. 안에 텍스트("★"/"☆") 하나를 자식으로 넣어두면
	// NativeOnListItemObjectSet에서 상태에 따라 자동으로 바꿔줍니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> WatchlistButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> WatchlistIconText;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UFUNCTION()
	void HandleSelectButtonClicked();

	UFUNCTION()
	void HandleWatchlistButtonClicked();

	UFUNCTION()
	void HandleButtonHovered();

	UFUNCTION()
	void HandleButtonUnhovered();

private:
	UPROPERTY()
	FName CachedSymbol;
};
