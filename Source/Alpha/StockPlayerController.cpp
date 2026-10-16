#include "StockPlayerController.h"
#include "GameMenuWidget.h"
#include "GameClockWidget.h"
#include "DocumentSortingGameWidget.h"
#include "InteractPromptWidget.h"
#include "Interactable.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"

void AStockPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
	SetShowMouseCursor(false);

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("StockPlayerController: DefaultMappingContext가 지정되지 않았습니다."));
			}
		}
	}

	if (GameClockWidgetClass)
	{
		GameClockWidgetInstance = CreateWidget<UGameClockWidget>(this, GameClockWidgetClass);

		if (GameClockWidgetInstance)
		{
			GameClockWidgetInstance->AddToViewport();
		}
	}

	if (InteractPromptWidgetClass)
	{
		InteractPromptWidgetInstance = CreateWidget<UInteractPromptWidget>(this, InteractPromptWidgetClass);

		if (InteractPromptWidgetInstance)
		{
			InteractPromptWidgetInstance->SetPromptText(InteractPromptText);
			InteractPromptWidgetInstance->AddToViewport();
			InteractPromptWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void AStockPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (ToggleStockUIAction)
		{
			EnhancedInputComponent->BindAction(
				ToggleStockUIAction,
				ETriggerEvent::Started,
				this,
				&AStockPlayerController::ToggleStockMarketWidget
			);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("StockPlayerController: ToggleStockUIAction이 지정되지 않았습니다."));
		}

		if (ToggleSortingGameAction)
		{
			EnhancedInputComponent->BindAction(
				ToggleSortingGameAction,
				ETriggerEvent::Started,
				this,
				&AStockPlayerController::ToggleSortingGameWidget
			);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("StockPlayerController: ToggleSortingGameAction이 지정되지 않았습니다."));
		}

		if (SortLeftClickAction)
		{
			EnhancedInputComponent->BindAction(
				SortLeftClickAction,
				ETriggerEvent::Started,
				this,
				&AStockPlayerController::HandleSortLeftClick
			);
		}

		if (SortRightClickAction)
		{
			EnhancedInputComponent->BindAction(
				SortRightClickAction,
				ETriggerEvent::Started,
				this,
				&AStockPlayerController::HandleSortRightClick
			);
		}

		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(
				InteractAction,
				ETriggerEvent::Started,
				this,
				&AStockPlayerController::HandleInteract
			);
		}
	}
}

void AStockPlayerController::ToggleStockMarketWidget()
{
	if (!GameMenuWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("StockPlayerController: GameMenuWidgetClass가 지정되지 않았습니다."));
		return;
	}

	if (!StockMarketWidgetInstance)
	{
		StockMarketWidgetInstance = CreateWidget<UGameMenuWidget>(this, GameMenuWidgetClass);
	}

	if (!StockMarketWidgetInstance)
	{
		return;
	}

	bIsStockMarketWidgetOpen = !bIsStockMarketWidgetOpen;

	if (bIsStockMarketWidgetOpen)
	{
		StockMarketWidgetInstance->AddToViewport();

		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(StockMarketWidgetInstance->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);

		SetShowMouseCursor(true);
	}
	else
	{
		StockMarketWidgetInstance->RemoveFromParent();

		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);

		SetShowMouseCursor(false);
	}
}

void AStockPlayerController::ToggleSortingGameWidget()
{
	if (!SortingGameWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("StockPlayerController: SortingGameWidgetClass가 지정되지 않았습니다."));
		return;
	}

	if (!SortingGameWidgetInstance)
	{
		SortingGameWidgetInstance = CreateWidget<UDocumentSortingGameWidget>(this, SortingGameWidgetClass);
	}

	if (!SortingGameWidgetInstance)
	{
		return;
	}

	bIsSortingGameWidgetOpen = !bIsSortingGameWidgetOpen;

	if (bIsSortingGameWidgetOpen)
	{
		SortingGameWidgetInstance->AddToViewport();

		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(SortingGameWidgetInstance->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);

		SetShowMouseCursor(true);
	}
	else
	{
		SortingGameWidgetInstance->RemoveFromParent();

		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);

		SetShowMouseCursor(false);
	}
}

void AStockPlayerController::HandleSortLeftClick()
{
	if (bIsSortingGameWidgetOpen && SortingGameWidgetInstance)
	{
		SortingGameWidgetInstance->HandlePlayerGuess(true);
	}
}

void AStockPlayerController::HandleSortRightClick()
{
	if (bIsSortingGameWidgetOpen && SortingGameWidgetInstance)
	{
		SortingGameWidgetInstance->HandlePlayerGuess(false);
	}
}

void AStockPlayerController::SetCurrentInteractable(AActor* Interactable)
{
	CurrentInteractable = Interactable;

	if (InteractPromptWidgetInstance)
	{
		InteractPromptWidgetInstance->SetVisibility(ESlateVisibility::Visible);
	}
}

void AStockPlayerController::ClearCurrentInteractable(AActor* Interactable)
{
	// 지금 등록된 것과 같은 대상일 때만 지웁니다. (오버랩이 겹치는 상황에서 다른 물체가
	// 실수로 등록 해제되는 걸 방지)
	if (CurrentInteractable == Interactable)
	{
		CurrentInteractable = nullptr;

		if (InteractPromptWidgetInstance)
		{
			InteractPromptWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void AStockPlayerController::HandleInteract()
{
	if (CurrentInteractable && CurrentInteractable->Implements<UInteractable>())
	{
		// 상호작용이 실제로 일어나는 순간엔 안내 문구가 화면을 가리지 않게 숨깁니다.
		if (InteractPromptWidgetInstance)
		{
			InteractPromptWidgetInstance->SetVisibility(ESlateVisibility::Collapsed);
		}

		IInteractable::Execute_Interact(CurrentInteractable, GetPawn());
	}
}
