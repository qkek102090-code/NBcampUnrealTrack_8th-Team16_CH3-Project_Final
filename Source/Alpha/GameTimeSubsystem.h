#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameTimeSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGameTimeChanged, int32, Day, int32, Hour, int32, Minute);

UCLASS()
class ALPHA_API UGameTimeSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UPROPERTY(BlueprintAssignable)
	FOnGameTimeChanged OnGameTimeChanged;

	UFUNCTION(BlueprintCallable, Category = "Time")
	int32 GetCurrentDay() const { return CurrentDay; }

	UFUNCTION(BlueprintCallable, Category = "Time")
	int32 GetCurrentHour() const { return CurrentHour; }

	UFUNCTION(BlueprintCallable, Category = "Time")
	int32 GetCurrentMinute() const { return CurrentMinute; }

	UFUNCTION(BlueprintCallable, Category = "Time")
	FText GetFormattedTime() const;

protected:
	void AdvanceOneMinute();

	UPROPERTY()
	int32 CurrentDay = 1;

	UPROPERTY()
	int32 CurrentHour = 8;

	UPROPERTY()
	int32 CurrentMinute = 0;

	FTimerHandle TimeTimerHandle;

	// --- 시간 흐름 설정 ---
	UPROPERTY(EditDefaultsOnly, Category = "Time|Config")
	float RealSecondsPerGameMinute = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Time|Config")
	int32 StartHour = 8;
};
