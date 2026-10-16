#include "SortingItemWidget.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"

void USortingItemWidget::SetItemData(const FSortingItemData& InData)
{
	CachedItemID = InData.ItemID;

	if (ItemLabelText)
	{
		ItemLabelText->SetText(InData.DisplayText);
	}

	if (ColorBorder)
	{
		ColorBorder->SetBrushColor(InData.bIsRed
			? FLinearColor(0.85f, 0.2f, 0.2f)
			: FLinearColor(0.25f, 0.45f, 0.95f));
	}
}

FReply USortingItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		OnItemSorted.Broadcast(CachedItemID, true);
		return FReply::Handled();
	}

	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		OnItemSorted.Broadcast(CachedItemID, false);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}
