#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameTimeSubsystem.h"
#include "GameClockWidget.generated.h"

class UTextBlock;

UCLASS()
class ALPHA_API UGameClockWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ClockText;

	UFUNCTION()
	void HandleGameTimeChanged(int32 Day, int32 Hour, int32 Minute);

	UPROPERTY()
	TObjectPtr<UGameTimeSubsystem> CachedTimeSubsystem;
};
