#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StockMarketSubsystem.h"
#include "BankPanelWidget.generated.h"

class USpinBox;
class UButton;
class UTextBlock;

UCLASS()
class ALPHA_API UBankPanelWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ReputationText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MaxLoanAmountText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CurrentDebtText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> AvailableLoanAmountText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<USpinBox> BorrowAmountSpinBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> BorrowButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<USpinBox> RepayAmountSpinBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> RepayButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MessageText;

	UFUNCTION(BlueprintCallable, Category = "Bank")
	void RefreshBankInfo();

	UFUNCTION()
	void HandleBorrowClicked();

	UFUNCTION()
	void HandleRepayClicked();

	UFUNCTION()
	void HandleAssetChanged();

	UFUNCTION()
	void HandleReputationChanged(float NewReputation);

	UFUNCTION()
	void HandleTradeResult(bool bSuccess, FName Symbol, FText Message);

	void ShowTemporaryMessage(const FText& Message);
	void ClearMessageText();

	UPROPERTY()
	TObjectPtr<UStockMarketSubsystem> CachedSubsystem;

	FTimerHandle MessageTimerHandle;
};
