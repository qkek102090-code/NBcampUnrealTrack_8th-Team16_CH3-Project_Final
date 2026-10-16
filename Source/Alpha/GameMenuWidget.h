#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StockMarketSubsystem.h"
#include "GameTimeSubsystem.h"
#include "GameMenuWidget.generated.h"

class UPanelWidget;
class UButton;
class UTextBlock;
class UStockMarketWidget;

UCLASS()
class ALPHA_API UGameMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> ContentArea;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CashText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ReputationText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TimeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> StockTabButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> ShopTabButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> DonateTabButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> BankTabButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> UpgradeTabButton;

	UPROPERTY(EditDefaultsOnly, Category = "Tabs")
	TSubclassOf<UUserWidget> StockWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Tabs")
	TSubclassOf<UUserWidget> ShopWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Tabs")
	TSubclassOf<UUserWidget> DonateWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Tabs")
	TSubclassOf<UUserWidget> BankWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Tabs")
	TSubclassOf<UUserWidget> UpgradeWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Tabs")
	FLinearColor ActiveTabColor = FLinearColor(1.0f, 0.55f, 0.0f);

	// 흰색(1,1,1,1)은 "곱해도 원본 그대로"라는 뜻이라, 비활성 탭이 활성화 전이랑
	// 똑같은 모습을 유지합니다. 회색 등으로 바꾸면 원본 색이 어둡게 물듭니다.
	UPROPERTY(EditAnywhere, Category = "Tabs")
	FLinearColor InactiveTabColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);

	UFUNCTION()
	void ShowStockTab();

	UFUNCTION()
	void ShowShopTab();

	UFUNCTION()
	void ShowDonateTab();

	UFUNCTION()
	void ShowBankTab();

	UFUNCTION()
	void ShowUpgradeTab();

	UFUNCTION()
	void HandleAssetChanged();

	UFUNCTION()
	void HandleReputationChanged(float NewReputation);

	UFUNCTION()
	void HandleGameTimeChanged(int32 Day, int32 Hour, int32 Minute);

private:
	void UpdateCashDisplay(float Cash);
	void UpdateReputationDisplay(float Reputation);

	void ShowTabWidget(TSubclassOf<UUserWidget> WidgetClass, TObjectPtr<UUserWidget>& CachedInstance);

	void UpdateTabButtonHighlight(UButton* ActiveButton);

	UPROPERTY()
	TObjectPtr<UStockMarketSubsystem> CachedSubsystem;

	UPROPERTY()
	TObjectPtr<UGameTimeSubsystem> CachedTimeSubsystem;

	UPROPERTY()
	TObjectPtr<UUserWidget> StockWidgetInstance;

	UPROPERTY()
	TObjectPtr<UUserWidget> ShopWidgetInstance;

	UPROPERTY()
	TObjectPtr<UUserWidget> DonateWidgetInstance;

	UPROPERTY()
	TObjectPtr<UUserWidget> BankWidgetInstance;

	UPROPERTY()
	TObjectPtr<UUserWidget> UpgradeWidgetInstance;
};