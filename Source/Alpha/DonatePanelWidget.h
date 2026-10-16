#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StockMarketSubsystem.h"
#include "DonatePanelWidget.generated.h"

class UButton;
class UTextBlock;
class UListView;
class UUserWidget;

UCLASS()
class ALPHA_API UDonatePanelWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UListView> OrgListView;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SelectedOrgText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SelectedAmountText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> DonateButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ReputationText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MessageText;

	UFUNCTION(BlueprintCallable, Category = "Donation")
	void RefreshOrgList();

	void HandleEntryWidgetGenerated(UUserWidget& EntryWidget);

	UFUNCTION()
	void HandleOrgSelected(FName OrgID);

	UFUNCTION()
	void HandleDonateClicked();

	UFUNCTION()
	void HandleReputationChanged(float NewReputation);

	UFUNCTION()
	void HandleTradeResult(bool bSuccess, FName Symbol, FText Message);

	void ShowTemporaryMessage(const FText& Message);

	void ClearMessageText();

	UPROPERTY(BlueprintReadOnly, Category = "Donation")
	FName SelectedOrgID;

	UPROPERTY()
	TObjectPtr<UStockMarketSubsystem> CachedSubsystem;

	FTimerHandle MessageTimerHandle;
};
