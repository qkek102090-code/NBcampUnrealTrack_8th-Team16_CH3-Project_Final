#include "MyStockEntryWidget.h"
#include "StockListItemObject.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UMyStockEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SelectButton)
	{
		SelectButton->OnClicked.AddDynamic(this, &UMyStockEntryWidget::HandleSelectButtonClicked);
	}
}

void UMyStockEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	const UStockListItemObject* Item = Cast<UStockListItemObject>(ListItemObject);
	if (!Item)
	{
		return;
	}

	CachedSymbol = Item->Symbol;

	if (SymbolText)
	{
		SymbolText->SetText(FText::FromName(Item->Symbol));
	}
	if (NameText)
	{
		NameText->SetText(Item->DisplayName);
	}
	if (PriceText)
	{
		PriceText->SetText(FText::Format(
			NSLOCTEXT("Stock", "PriceWonFormat", "{0}원"),
			FText::AsNumber(FMath::RoundToInt(Item->CurrentPrice))
		));
	}

	if (QuantityText)
	{
		QuantityText->SetText(FText::Format(
			NSLOCTEXT("Stock", "QuantityFormat", "{0}주"),
			FText::AsNumber(Item->HoldingQuantity)
		));
	}

	if (GainText)
	{
		const int32 GainAmountRounded = FMath::RoundToInt(Item->GainAmount);
		const FString GainString = FString::Printf(
			TEXT("%s%s원 (%+.2f%%)"),
			GainAmountRounded >= 0 ? TEXT("+") : TEXT(""),
			*FText::AsNumber(GainAmountRounded).ToString(),
			Item->GainPercent
		);
		GainText->SetText(FText::FromString(GainString));

		if (Item->GainAmount > 0.f)
		{
			GainText->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.8f, 0.3f)));
		}
		else if (Item->GainAmount < 0.f)
		{
			GainText->SetColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.25f, 0.25f)));
		}
		else
		{
			GainText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		}
	}
}

void UMyStockEntryWidget::HandleSelectButtonClicked()
{
	OnEntryClicked.Broadcast(CachedSymbol);
}
