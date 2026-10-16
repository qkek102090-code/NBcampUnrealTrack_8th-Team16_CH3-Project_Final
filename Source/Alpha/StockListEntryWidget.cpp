#include "StockListEntryWidget.h"
#include "StockListItemObject.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UStockListEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SelectButton)
	{
		SelectButton->OnClicked.AddDynamic(this, &UStockListEntryWidget::HandleSelectButtonClicked);
		SelectButton->OnHovered.AddDynamic(this, &UStockListEntryWidget::HandleButtonHovered);
		SelectButton->OnUnhovered.AddDynamic(this, &UStockListEntryWidget::HandleButtonUnhovered);
	}
	if (WatchlistButton)
	{
		WatchlistButton->OnClicked.AddDynamic(this, &UStockListEntryWidget::HandleWatchlistButtonClicked);
	}
}

void UStockListEntryWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
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
		PriceText->SetText(FText::AsNumber(FMath::RoundToInt(Item->CurrentPrice)));
	}

	if (PercentChangeText)
	{
		const FString PercentString = FString::Printf(TEXT("%+.2f%%"), Item->PercentChange);
		PercentChangeText->SetText(FText::FromString(PercentString));

		if (Item->PercentChange > 0.f)
		{
			PercentChangeText->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.8f, 0.3f)));
		}
		else if (Item->PercentChange < 0.f)
		{
			PercentChangeText->SetColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.25f, 0.25f)));
		}
		else
		{
			PercentChangeText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		}
	}

	if (WatchlistIconText)
	{
		WatchlistIconText->SetText(FText::FromString(Item->bInWatchlist ? TEXT("★") : TEXT("☆")));
		WatchlistIconText->SetColorAndOpacity(FSlateColor(
			Item->bInWatchlist ? FLinearColor(1.0f, 0.8f, 0.1f) : FLinearColor(0.6f, 0.6f, 0.6f)
		));
	}
}

void UStockListEntryWidget::HandleSelectButtonClicked()
{
	OnEntryClicked.Broadcast(CachedSymbol);
}

void UStockListEntryWidget::HandleWatchlistButtonClicked()
{
	OnWatchlistToggled.Broadcast(CachedSymbol);
}

void UStockListEntryWidget::HandleButtonHovered()
{
	OnEntryHovered.Broadcast(CachedSymbol);
}

void UStockListEntryWidget::HandleButtonUnhovered()
{
	OnEntryUnhovered.Broadcast();
}
