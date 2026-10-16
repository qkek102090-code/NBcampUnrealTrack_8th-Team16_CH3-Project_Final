#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (NewGameButton)
	{
		NewGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleNewGameClicked);
	}
	if (ContinueButton)
	{
		ContinueButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleContinueClicked);
	}
	if (LoadGameButton)
	{
		LoadGameButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleLoadGameClicked);
	}
	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleSettingsClicked);
	}
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleQuitClicked);
	}
}

void UMainMenuWidget::HandleNewGameClicked()
{
	UGameplayStatics::OpenLevel(this, GameplayLevelName);
}

void UMainMenuWidget::HandleContinueClicked()
{
	// 세이브 시스템이 생기기 전까지는 새로운 게임과 동일하게 처리합니다.
	// 나중에 세이브 시스템을 만들면, 여기서 마지막 저장 데이터를 불러온 뒤 레벨을 여는 식으로 바꾸면 됩니다.
	UGameplayStatics::OpenLevel(this, GameplayLevelName);
}

void UMainMenuWidget::HandleLoadGameClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("MainMenuWidget: 불러오기 기능은 세이브 시스템을 만든 뒤 여기에 연결하세요."));
}

void UMainMenuWidget::HandleSettingsClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("MainMenuWidget: 설정 화면은 아직 만들어지지 않았습니다."));
}

void UMainMenuWidget::HandleQuitClicked()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		UKismetSystemLibrary::QuitGame(this, PC, EQuitPreference::Quit, false);
	}
}
