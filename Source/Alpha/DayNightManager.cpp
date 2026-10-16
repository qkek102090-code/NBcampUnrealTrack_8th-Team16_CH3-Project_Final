#include "DayNightManager.h"
#include "GameTimeSubsystem.h"

ADayNightManager::ADayNightManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADayNightManager::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GI = GetGameInstance())
	{
		CachedTimeSubsystem = GI->GetSubsystem<UGameTimeSubsystem>();
	}

	if (!CachedTimeSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("DayNightManager: GameTimeSubsystem을 찾지 못했습니다."));
		return;
	}

	CachedTimeSubsystem->OnGameTimeChanged.AddDynamic(this, &ADayNightManager::HandleGameTimeChanged);

	// 시작하자마자 지금 시각 기준으로 한 번 맞춰둡니다 (다음 분이 될 때까지 기다리지 않고).
	HandleGameTimeChanged(
		CachedTimeSubsystem->GetCurrentDay(),
		CachedTimeSubsystem->GetCurrentHour(),
		CachedTimeSubsystem->GetCurrentMinute()
	);
}

void ADayNightManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CachedTimeSubsystem)
	{
		CachedTimeSubsystem->OnGameTimeChanged.RemoveDynamic(this, &ADayNightManager::HandleGameTimeChanged);
	}

	Super::EndPlay(EndPlayReason);
}

void ADayNightManager::HandleGameTimeChanged(int32 Day, int32 Hour, int32 Minute)
{
	UpdateSunRotation(Hour, Minute);
	UpdateNightLights(Hour);
}

void ADayNightManager::UpdateSunRotation(int32 Hour, int32 Minute)
{
	if (!SunLight)
	{
		return;
	}

	// 0~24시를 360도 회전으로 매핑합니다.
	// 6시(일출)=0도(지평선), 12시(정오)=-90도(머리 위), 18시(일몰)=180도(반대편 지평선),
	// 0시(자정)=90도(지평선 아래 반대쪽)가 되도록 만든 식입니다.
	const float HourFraction = Hour + (Minute / 60.f);
	const float Pitch = 90.f - (HourFraction / 24.f) * 360.f;

	SunLight->SetActorRotation(FRotator(Pitch, 0.f, 0.f));
}

void ADayNightManager::UpdateNightLights(int32 Hour)
{
	const bool bShouldBeNight = (Hour < SunriseHour) || (Hour >= SunsetHour);

	// 상태가 안 바뀌었으면 매번 반복해서 켜고 끄지 않게 건너뜁니다.
	if (bShouldBeNight == bIsCurrentlyNight)
	{
		return;
	}

	bIsCurrentlyNight = bShouldBeNight;

	for (AActor* Light : NightLights)
	{
		if (Light)
		{
			Light->SetActorHiddenInGame(!bIsCurrentlyNight);
		}
	}
}
