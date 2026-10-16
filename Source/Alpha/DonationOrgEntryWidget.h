#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "DonationOrgEntryWidget.generated.h"

class UTextBlock;
class UButton;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDonationOrgSelected, FName, OrgID);

UCLASS()
class ALPHA_API UDonationOrgEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnDonationOrgSelected OnOrgSelected;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> OrgNameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DescriptionText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DonationAmountText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> SelectButton;

	UPROPERTY(EditAnywhere, Category = "Card")
	float CardWidth = 220.f;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UFUNCTION()
	void HandleSelectButtonClicked();

private:
	UPROPERTY()
	FName CachedOrgID;
};
