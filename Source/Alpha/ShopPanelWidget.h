#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StockMarketSubsystem.h"
#include "ShopPanelWidget.generated.h"

class UListView;
class UTextBlock;
class UUserWidget;

UCLASS()
class ALPHA_API UShopPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UListView> ShopListView;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ReputationText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MessageText;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "Shop")
	void RefreshShopList();

	void HandleEntryWidgetGenerated(UUserWidget& EntryWidget);

	UFUNCTION()
	void HandlePurchaseClicked(FName ItemID);

	UFUNCTION()
	void HandleReputationChanged(float NewReputation);

	UFUNCTION()
	void HandleTradeResult(bool bSuccess, FName ItemID, FText Message);

	void ShowTemporaryMessage(const FText& Message);

	void ClearMessageText();

	UPROPERTY()
	TObjectPtr<UStockMarketSubsystem> CachedSubsystem;

	FTimerHandle MessageTimerHandle;
};
