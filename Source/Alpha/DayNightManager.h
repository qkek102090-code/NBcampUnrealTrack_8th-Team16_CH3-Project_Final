#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DayNightManager.generated.h"

class UGameTimeSubsystem;

// 레벨에 하나 배치하는 낮/밤 관리자.
// GameTimeSubsystem의 시간 변경 이벤트를 구독해서, 매 인게임 분마다
// 태양(디렉셔널 라이트) 각도를 갱신하고, 밤이 되면 등록해둔 조명들을 켭니다.
UCLASS()
class ALPHA_API ADayNightManager : public AActor
{
	GENERATED_BODY()

public:
	ADayNightManager();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 태양 역할을 할 디렉셔널 라이트 액터. 그 라이트의 "Atmosphere Sun Light"가 켜져
	// 있으면, 이 각도만 바꿔도 하늘/구름 색까지 언리얼이 알아서 같이 바꿔줍니다.
	UPROPERTY(EditAnywhere, Category = "DayNight")
	TObjectPtr<AActor> SunLight;

	// 밤에 켜지는 조명들 (가로등 등). 원하는 만큼 등록하세요.
	UPROPERTY(EditAnywhere, Category = "DayNight")
	TArray<TObjectPtr<AActor>> NightLights;

	// 이 시각(포함)부터 낮으로 칩니다.
	UPROPERTY(EditAnywhere, Category = "DayNight")
	int32 SunriseHour = 6;

	// 이 시각(포함)부터 밤으로 칩니다.
	UPROPERTY(EditAnywhere, Category = "DayNight")
	int32 SunsetHour = 19;

	UFUNCTION()
	void HandleGameTimeChanged(int32 Day, int32 Hour, int32 Minute);

	void UpdateSunRotation(int32 Hour, int32 Minute);
	void UpdateNightLights(int32 Hour);

	UPROPERTY()
	TObjectPtr<UGameTimeSubsystem> CachedTimeSubsystem;

	bool bIsCurrentlyNight = false;
};
