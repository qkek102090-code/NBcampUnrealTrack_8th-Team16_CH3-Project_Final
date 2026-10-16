#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "StockPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class UGameMenuWidget;
class UGameClockWidget;
class UDocumentSortingGameWidget;
class UInteractPromptWidget;

UCLASS()
class ALPHA_API AStockPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleStockUIAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ToggleSortingGameAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> SortLeftClickAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> SortRightClickAction;

	// 상호작용(E키) 입력. 모니터 등 IInteractable을 구현한 오브젝트와 상호작용할 때 씁니다.
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, Category = "Stock")
	TSubclassOf<UGameMenuWidget> GameMenuWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Time")
	TSubclassOf<UGameClockWidget> GameClockWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Minigame")
	TSubclassOf<UDocumentSortingGameWidget> SortingGameWidgetClass;

	// 상호작용 범위 안에 들어오면 자동으로 뜨는 안내 문구 위젯 클래스 (예: WBP_InteractPrompt)
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TSubclassOf<UInteractPromptWidget> InteractPromptWidgetClass;

	// 안내 문구에 표시할 텍스트 (예: "업무 시작하기 E")
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	FText InteractPromptText = FText::FromString(TEXT("상호작용 E"));

	UFUNCTION()
	void ToggleStockMarketWidget();

	UFUNCTION(BlueprintCallable, Category = "Minigame")
	void ToggleSortingGameWidget();

	UFUNCTION()
	void HandleSortLeftClick();

	UFUNCTION()
	void HandleSortRightClick();

	// 상호작용 오브젝트(모니터 등)가 자신의 트리거에서 오버랩이 시작/종료될 때 호출해서
	// "지금 상호작용 가능한 대상이 뭔지"를 등록/해제합니다.
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetCurrentInteractable(AActor* Interactable);

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ClearCurrentInteractable(AActor* Interactable);

	UFUNCTION()
	void HandleInteract();

private:
	UPROPERTY()
	TObjectPtr<UGameMenuWidget> StockMarketWidgetInstance;

	UPROPERTY()
	TObjectPtr<UGameClockWidget> GameClockWidgetInstance;

	UPROPERTY()
	TObjectPtr<UDocumentSortingGameWidget> SortingGameWidgetInstance;

	UPROPERTY()
	TObjectPtr<UInteractPromptWidget> InteractPromptWidgetInstance;

	// 지금 범위 안에 있어서 E키를 누르면 반응할 대상. 여러 개가 겹치는 경우는 고려하지 않고,
	// 가장 최근에 들어온 것 하나만 기억합니다.
	UPROPERTY()
	TObjectPtr<AActor> CurrentInteractable;

	bool bIsStockMarketWidgetOpen = false;
	bool bIsSortingGameWidgetOpen = false;
};
