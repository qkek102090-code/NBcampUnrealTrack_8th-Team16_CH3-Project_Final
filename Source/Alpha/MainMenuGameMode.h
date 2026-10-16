#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

class UMainMenuWidget;

// 메인 메뉴 전용 레벨(예: "MainMenu")의 GameMode.
// 게임이 시작되자마자 메인 메뉴 위젯을 화면에 띄우고 마우스 커서를 보여줍니다.
UCLASS()
class ALPHA_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;
};
