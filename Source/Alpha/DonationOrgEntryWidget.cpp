#include "DonationOrgEntryWidget.h"
#include "DonationOrgListItemObject.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"

void UDonationOrgEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SelectButton)
	{
		SelectButton->OnClicked.AddDynamic(this, &UDonationOrgEntryWidget::HandleSelectButtonClicked);
	}

	if (DescriptionText)
	{
		DescriptionText->SetAutoWrapText(true);
		DescriptionText->SetWrapTextAt(CardWidth - 16.f);
	}
	if (OrgNameText)
	{
		OrgNameText->SetAutoWrapText(true);
		OrgNameText->SetWrapTextAt(CardWidth - 16.f);
	}
}

void UDonationOrgEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const UDonationOrgListItemObject* Item = Cast<UDonationOrgListItemObject>(ListItemObject);
	if (!Item)
	{
		return;
	}

	CachedOrgID = Item->OrgID;

	if (OrgNameText)
	{
		OrgNameText->SetText(Item->OrgName);
	}
	if (DescriptionText)
	{
		DescriptionText->SetText(Item->Description);
	}
	if (DonationAmountText)
	{
		DonationAmountText->SetText(FText::AsNumber(FMath::RoundToInt(Item->DonationAmount)));
	}
	if (IconImage)
	{
		if (Item->Icon)
		{
			IconImage->SetBrushFromTexture(Item->Icon);
			IconImage->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			IconImage->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UDonationOrgEntryWidget::HandleSelectButtonClicked()
{
	OnOrgSelected.Broadcast(CachedOrgID);
}
