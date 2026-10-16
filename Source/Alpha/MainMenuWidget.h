#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;

// 게임 시작 메뉴 (새로운 게임 / 계속하기 / 불러오기 / 설정 / 종료).
// 별도의 메인 메뉴 레벨에서, AMainMenuGameMode가 이 위젯을 화면에 띄웁니다.
UCLASS()
class ALPHA_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> NewGameButton;

	// 세이브 시스템이 생기기 전까지는 새로운 게임과 동일하게 동작합니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> ContinueButton;

	// 세이브 시스템이 생기면 여기 연결하세요. 지금은 로그만 남깁니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> LoadGameButton;

	// 설정 화면이 생기면 여기 연결하세요. 지금은 로그만 남깁니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> SettingsButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> QuitButton;

protected:
	virtual void NativeConstruct() override;

	// "새로운 게임"을 누르면 이동할 실제 플레이 레벨 이름
	UPROPERTY(EditAnywhere, Category = "MainMenu")
	FName GameplayLevelName = TEXT("PlayGround");

	UFUNCTION()
	void HandleNewGameClicked();

	UFUNCTION()
	void HandleContinueClicked();

	UFUNCTION()
	void HandleLoadGameClicked();

	UFUNCTION()
	void HandleSettingsClicked();

	UFUNCTION()
	void HandleQuitClicked();
};
