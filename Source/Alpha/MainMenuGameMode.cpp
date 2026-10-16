#include "MainMenuGameMode.h"
#include "MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!MainMenuWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("MainMenuGameMode: MainMenuWidgetClass가 지정되지 않았습니다."));
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		return;
	}

	UMainMenuWidget* MenuWidget = CreateWidget<UMainMenuWidget>(PC, MainMenuWidgetClass);
	if (!MenuWidget)
	{
		return;
	}

	MenuWidget->AddToViewport();

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(MenuWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(InputMode);
	PC->SetShowMouseCursor(true);
}
