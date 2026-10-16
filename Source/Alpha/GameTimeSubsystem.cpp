#include "GameTimeSubsystem.h"

void UGameTimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CurrentHour = StartHour;
	CurrentMinute = 0;
	CurrentDay = 1;

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UWorld* World = GI->GetWorld())
		{
			World->GetTimerManager().SetTimer(
				TimeTimerHandle,
				this,
				&UGameTimeSubsystem::AdvanceOneMinute,
				RealSecondsPerGameMinute,
				true
			);
		}
	}
}

void UGameTimeSubsystem::Deinitialize()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UWorld* World = GI->GetWorld())
		{
			World->GetTimerManager().ClearTimer(TimeTimerHandle);
		}
	}

	Super::Deinitialize();
}

void UGameTimeSubsystem::AdvanceOneMinute()
{
	CurrentMinute++;

	if (CurrentMinute >= 60)
	{
		CurrentMinute = 0;
		CurrentHour++;

		if (CurrentHour >= 24)
		{
			CurrentHour = 0;
			CurrentDay++;
		}
	}

	OnGameTimeChanged.Broadcast(CurrentDay, CurrentHour, CurrentMinute);
}

FText UGameTimeSubsystem::GetFormattedTime() const
{
	FNumberFormattingOptions Options;
	Options.SetMinimumIntegralDigits(2);
	Options.SetUseGrouping(false);

	return FText::Format(
		NSLOCTEXT("Time", "ClockFormat", "{0}:{1}"),
		FText::AsNumber(CurrentHour, &Options),
		FText::AsNumber(CurrentMinute, &Options)
	);
}