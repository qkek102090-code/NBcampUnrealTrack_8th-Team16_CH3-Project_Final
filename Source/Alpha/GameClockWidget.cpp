#include "GameClockWidget.h"
#include "Components/TextBlock.h"

void UGameClockWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GI = GetGameInstance())
	{
		CachedTimeSubsystem = GI->GetSubsystem<UGameTimeSubsystem>();
	}

	if (!CachedTimeSubsystem)
	{
		return;
	}

	CachedTimeSubsystem->OnGameTimeChanged.AddDynamic(this, &UGameClockWidget::HandleGameTimeChanged);

	if (ClockText)
	{
		ClockText->SetText(CachedTimeSubsystem->GetFormattedTime());
	}
}

void UGameClockWidget::NativeDestruct()
{
	if (CachedTimeSubsystem)
	{
		CachedTimeSubsystem->OnGameTimeChanged.RemoveDynamic(this, &UGameClockWidget::HandleGameTimeChanged);
	}

	Super::NativeDestruct();
}

void UGameClockWidget::HandleGameTimeChanged(int32 Day, int32 Hour, int32 Minute)
{
	if (ClockText && CachedTimeSubsystem)
	{
		ClockText->SetText(CachedTimeSubsystem->GetFormattedTime());
	}
}
