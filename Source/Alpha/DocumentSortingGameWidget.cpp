#include "DocumentSortingGameWidget.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UDocumentSortingGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GI = GetGameInstance())
	{
		CachedSubsystem = GI->GetSubsystem<UStockMarketSubsystem>();
	}

	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UDocumentSortingGameWidget::HandleStartButtonClicked);
	}

	if (ResultText)
	{
		ResultText->SetVisibility(ESlateVisibility::Collapsed);
	}

	ActiveItems.SetNum(3);
	CurrentSlotData.SetNum(3);
}

void UDocumentSortingGameWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CountdownTimerHandle);
	}

	Super::NativeDestruct();
}

void UDocumentSortingGameWidget::HandleStartButtonClicked()
{
	StartGame();
}

void UDocumentSortingGameWidget::StartGame()
{
	CorrectCount = 0;
	WrongCount = 0;
	RemainingTime = GameDuration;
	bIsGameRunning = true;

	if (ResultText)
	{
		ResultText->SetVisibility(ESlateVisibility::Collapsed);
	}

	UpdateScoreText();
	UpdateTimeText();

	// 1번(맨 왼쪽)~3번(맨 오른쪽)에 각각 위젯을 처음 한 번만 만들어두고, 이후로는
	// 이 위젯들의 데이터만 갱신합니다 (매번 새로 만들지 않음).
	UPanelWidget* Slots[3] = { Slot1, Slot2, Slot3 };

	for (int32 Index = 0; Index < 3; ++Index)
	{
		CurrentSlotData[Index] = MakeRandomItemData();

		if (!Slots[Index] || !ItemWidgetClass)
		{
			continue;
		}

		USortingItemWidget* ItemWidget = CreateWidget<USortingItemWidget>(this, ItemWidgetClass);
		if (!ItemWidget)
		{
			continue;
		}

		// 판정은 이 위젯이 아니라 부모(게임 위젯 전체)가 화면 전체에서 받으므로,
		// 이 카드는 클릭을 가로채지 않도록 Hit Test Invisible로 둡니다.
		ItemWidget->SetVisibility(ESlateVisibility::HitTestInvisible);

		Slots[Index]->ClearChildren();
		Slots[Index]->AddChild(ItemWidget);

		ActiveItems[Index] = ItemWidget;
	}

	RefreshSlotVisual(0);
	RefreshSlotVisual(1);
	RefreshSlotVisual(2);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			CountdownTimerHandle,
			this,
			&UDocumentSortingGameWidget::Tick_CountDown,
			1.0f,
			true
		);
	}
}

FSortingItemData UDocumentSortingGameWidget::MakeRandomItemData() const
{
	FSortingItemData NewData;
	NewData.ItemID = FName(*FString::Printf(TEXT("SortItem_%d"), FMath::RandHelper(1000000)));
	NewData.bIsRed = FMath::RandBool();
	NewData.DisplayText = NewData.bIsRed
		? FText::FromString(TEXT("빨강"))
		: FText::FromString(TEXT("파랑"));

	return NewData;
}

void UDocumentSortingGameWidget::RefreshSlotVisual(int32 SlotIndex)
{
	if (!ActiveItems.IsValidIndex(SlotIndex) || !ActiveItems[SlotIndex] || !CurrentSlotData.IsValidIndex(SlotIndex))
	{
		return;
	}

	ActiveItems[SlotIndex]->SetItemData(CurrentSlotData[SlotIndex]);
}

void UDocumentSortingGameWidget::HandlePlayerGuess(bool bWasLeftClick)
{
	if (!bIsGameRunning || !CurrentSlotData.IsValidIndex(2))
	{
		return;
	}

	// 판정 대상은 항상 3번 칸(인덱스 2, 맨 오른쪽)입니다.
	const bool bFrontIsRed = CurrentSlotData[2].bIsRed;
	const bool bIsCorrect = (bFrontIsRed == bWasLeftClick); // 빨강=좌클릭, 파랑=우클릭이 정답

	if (!bIsCorrect)
	{
		// 틀리면 아무 변화 없이 그대로 둡니다.
		WrongCount++;
		UpdateScoreText();
		return;
	}

	CorrectCount++;
	UpdateScoreText();

	// 컨베이어 이동: 3번 칸이 사라지고, 2번->3번, 1번->2번으로 밀리고, 1번 자리에 새 아이템이 생깁니다.
	// (1-2-3 -> n-1-2)
	CurrentSlotData[2] = CurrentSlotData[1];
	CurrentSlotData[1] = CurrentSlotData[0];
	CurrentSlotData[0] = MakeRandomItemData();

	RefreshSlotVisual(0);
	RefreshSlotVisual(1);
	RefreshSlotVisual(2);
}

void UDocumentSortingGameWidget::Tick_CountDown()
{
	RemainingTime -= 1.0f;
	UpdateTimeText();

	if (RemainingTime <= 0.f)
	{
		EndGame();
	}
}

void UDocumentSortingGameWidget::EndGame()
{
	bIsGameRunning = false;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CountdownTimerHandle);
	}

	if (Slot1) Slot1->ClearChildren();
	if (Slot2) Slot2->ClearChildren();
	if (Slot3) Slot3->ClearChildren();

	const float Reward = CorrectCount * RewardPerCorrectItem;

	if (CachedSubsystem && Reward > 0.f)
	{
		CachedSubsystem->AddCash(Reward);
	}

	if (ResultText)
	{
		ResultText->SetText(FText::Format(
			NSLOCTEXT("Minigame", "ResultFormat", "정답 {0}개 / 오답 {1}개\n보상 {2}원 지급"),
			FText::AsNumber(CorrectCount),
			FText::AsNumber(WrongCount),
			FText::AsNumber(FMath::RoundToInt(Reward))
		));
		ResultText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UDocumentSortingGameWidget::UpdateScoreText()
{
	if (ScoreText)
	{
		ScoreText->SetText(FText::Format(
			NSLOCTEXT("Minigame", "ScoreFormat", "정답 {0} / 오답 {1}"),
			FText::AsNumber(CorrectCount),
			FText::AsNumber(WrongCount)
		));
	}
}

void UDocumentSortingGameWidget::UpdateTimeText()
{
	if (TimeText)
	{
		TimeText->SetText(FText::AsNumber(FMath::Max(FMath::CeilToInt(RemainingTime), 0)));
	}
}
