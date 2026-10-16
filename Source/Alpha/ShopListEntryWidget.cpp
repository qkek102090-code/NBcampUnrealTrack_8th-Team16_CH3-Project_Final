#include "ShopListEntryWidget.h"
#include "ShopListItemObject.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UShopListEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PurchaseButton)
	{
		PurchaseButton->OnClicked.AddDynamic(this, &UShopListEntryWidget::HandlePurchaseButtonClicked);
	}
}

void UShopListEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const UShopListItemObject* Item = Cast<UShopListItemObject>(ListItemObject);
	if (!Item)
	{
		return;
	}

	CachedItemID = Item->ItemID;

	if (NameText)
	{
		NameText->SetText(Item->DisplayName);
	}
	if (PriceText)
	{
		PriceText->SetText(FText::AsNumber(FMath::RoundToInt(Item->Price)));
	}
	if (LockText)
	{
		if (Item->bUnlocked)
		{
			LockText->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			LockText->SetVisibility(ESlateVisibility::Visible);
			LockText->SetText(FText::Format(
				NSLOCTEXT("Shop", "LockedFormat", "인지도 {0} 필요"),
				FText::AsNumber(FMath::RoundToInt(Item->RequiredReputation))
			));
		}
	}
	if (PurchaseButton)
	{
		PurchaseButton->SetIsEnabled(Item->bUnlocked && !Item->bOwned);
	}
}

void UShopListEntryWidget::HandlePurchaseButtonClicked()
{
	OnPurchaseClicked.Broadcast(CachedItemID);
}
