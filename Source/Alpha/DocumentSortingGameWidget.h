#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SortingItemWidget.h"
#include "StockMarketSubsystem.h"
#include "DocumentSortingGameWidget.generated.h"

class UPanelWidget;
class UTextBlock;
class UButton;

// 컨베이어 벨트 방식 분류 미니게임.
// 슬롯 3개(1번=맨 왼쪽/새로 들어옴, 2번=중간, 3번=맨 오른쪽/판정 대상)가 있고,
// 판정은 항상 3번 칸의 색깔로만 이뤄집니다: 빨강이면 좌클릭, 파랑이면 우클릭
// (화면 아무 곳이나 클릭하면 됩니다 - 특정 위치를 클릭할 필요 없음).
// 맞추면 3번 칸이 사라지고 1,2번이 오른쪽으로 한 칸씩 밀리며 1번 자리에 새 아이템이 생깁니다.
// 틀리면 아무 변화도 없습니다.
UCLASS()
class ALPHA_API UDocumentSortingGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Minigame")
	void StartGame();

	// 마우스 좌/우클릭을 Enhanced Input Action으로 받아서 PlayerController가 이 함수를 호출합니다.
	// (Slate의 화면 전체 클릭 감지 대신, 이미 검증된 Enhanced Input 경로를 사용합니다)
	UFUNCTION(BlueprintCallable, Category = "Minigame")
	void HandlePlayerGuess(bool bWasLeftClick);

	// 1번(맨 왼쪽) ~ 3번(맨 오른쪽, 판정 대상) 슬롯. 각각 빈 Panel(Overlay 등)을 배치하세요.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> Slot1;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> Slot2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> Slot3;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TimeText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ScoreText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ResultText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> StartButton;

	UPROPERTY(EditAnywhere, Category = "Minigame")
	TSubclassOf<USortingItemWidget> ItemWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Minigame")
	float GameDuration = 30.f;

	UPROPERTY(EditAnywhere, Category = "Minigame")
	float RewardPerCorrectItem = 5000.f;

	// 지금 게임이 진행 중인지. PlayerController가 클릭을 무시할지 판단할 때 씁니다.
	bool IsGameRunning() const { return bIsGameRunning; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleStartButtonClicked();

	FSortingItemData MakeRandomItemData() const;
	void RefreshSlotVisual(int32 SlotIndex);
	void Tick_CountDown();
	void EndGame();
	void UpdateScoreText();
	void UpdateTimeText();

	// 인덱스 0=1번(맨 왼쪽), 1=2번(중간), 2=3번(맨 오른쪽, 판정 대상)
	UPROPERTY()
	TArray<TObjectPtr<USortingItemWidget>> ActiveItems;

	UPROPERTY()
	TArray<FSortingItemData> CurrentSlotData;

	bool bIsGameRunning = false;
	int32 CorrectCount = 0;
	int32 WrongCount = 0;
	float RemainingTime = 0.f;

	FTimerHandle CountdownTimerHandle;

	UPROPERTY()
	TObjectPtr<UStockMarketSubsystem> CachedSubsystem;
};
