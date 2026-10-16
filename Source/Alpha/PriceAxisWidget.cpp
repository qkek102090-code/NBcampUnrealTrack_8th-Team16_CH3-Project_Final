#include "PriceAxisWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Blueprint/WidgetTree.h"

void UPriceAxisWidget::SetPriceRange(float MinPrice, float MaxPrice, float CurrentPrice)
{
	if (!AxisList && WidgetTree)
	{
		AxisList = Cast<UVerticalBox>(WidgetTree->FindWidget(TEXT("AxisList")));

		if (!AxisList)
		{
			UE_LOG(LogTemp, Warning, TEXT("[PriceAxisDebug3] RootWidget=%s, 이 위젯의 클래스: %s"),
				WidgetTree->RootWidget ? TEXT("valid") : TEXT("NULL"),
				*GetClass()->GetName());

			WidgetTree->ForEachWidget([](UWidget* Widget)
				{
					if (Widget)
					{
						UE_LOG(LogTemp, Warning, TEXT("[PriceAxisDebug3] 트리 안의 위젯: '%s' (클래스: %s)"),
							*Widget->GetName(), *Widget->GetClass()->GetName());
					}
				});
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[PriceAxisDebug2] SetPriceRange 호출됨. AxisList=%s WidgetTree=%s Min=%f Max=%f"),
		AxisList ? TEXT("valid") : TEXT("NULL"),
		WidgetTree ? TEXT("valid") : TEXT("NULL"),
		MinPrice, MaxPrice);

	if (!AxisList || !WidgetTree)
	{
		return;
	}

	AxisList->ClearChildren();

	if (FMath::IsNearlyEqual(MinPrice, MaxPrice))
	{
		const float Margin = FMath::Max(MaxPrice * 0.01f, 1.f);
		MinPrice -= Margin;
		MaxPrice += Margin;
	}

	const float PriceRange = MaxPrice - MinPrice;
	const int32 Count = FMath::Max(NumGridlines, 2);

	TArray<float> Levels;
	Levels.Reserve(Count);

	int32 ClosestIndex = 0;
	float ClosestDistance = TNumericLimits<float>::Max();

	for (int32 Index = 0; Index < Count; ++Index)
	{
		const float T = static_cast<float>(Index) / static_cast<float>(Count - 1);
		const float Price = MaxPrice - T * PriceRange;
		Levels.Add(Price);

		const float Distance = FMath::Abs(Price - CurrentPrice);
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestIndex = Index;
		}
	}

	for (int32 Index = 0; Index < Count; ++Index)
	{
		UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Label->SetText(FText::AsNumber(FMath::RoundToInt(Levels[Index])));

		UWidget* RowWidget = Label;

		if (Index == ClosestIndex)
		{
			UBorder* Badge = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
			Badge->SetBrushColor(CurrentPriceBadgeColor);
			Badge->SetPadding(FMargin(6.f, 2.f));
			Label->SetColorAndOpacity(FSlateColor(FLinearColor::White));
			Badge->SetContent(Label);
			RowWidget = Badge;
		}

		if (UVerticalBoxSlot* BoxSlot = AxisList->AddChildToVerticalBox(RowWidget))
		{
			BoxSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			BoxSlot->SetHorizontalAlignment(HAlign_Right);
			BoxSlot->SetVerticalAlignment(VAlign_Center);
		}

		// 마지막 눈금 뒤에는 구분선을 안 그립니다.
		if (Index < Count - 1)
		{
			// Border에 내용물 없이 좌우 Padding으로 가로 길이를, 상하 Padding으로 두께를 만듭니다.
			// HAlign_Fill을 안 쓰기 때문에, 부모(AxisList) 폭과 상관없이 DividerLength로 고정됩니다.
			UBorder* Divider = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass());
			Divider->SetBrushColor(DividerColor);
			Divider->SetPadding(FMargin(DividerLength * 0.5f, 0.75f));

			if (UVerticalBoxSlot* DividerSlot = AxisList->AddChildToVerticalBox(Divider))
			{
				DividerSlot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
				DividerSlot->SetHorizontalAlignment(DividerAlignment);
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[PriceAxisDebug2] 채워진 자식 개수: %d, AxisList 실제 크기: %s"),
		AxisList->GetChildrenCount(),
		*AxisList->GetCachedGeometry().GetLocalSize().ToString());
}