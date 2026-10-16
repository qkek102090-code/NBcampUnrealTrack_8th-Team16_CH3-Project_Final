#include "GameMenuWidget.h"
#include "Components/PanelWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "StockMarketWidget.h"

void UGameMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GI = GetGameInstance())
	{
		CachedSubsystem = GI->GetSubsystem<UStockMarketSubsystem>();
	}

	if (CachedSubsystem)
	{
		CachedSubsystem->OnAssetChanged.AddDynamic(this, &UGameMenuWidget::HandleAssetChanged);
		CachedSubsystem->OnReputationChanged.AddDynamic(this, &UGameMenuWidget::HandleReputationChanged);

		UpdateCashDisplay(CachedSubsystem->GetCash());
		UpdateReputationDisplay(CachedSubsystem->GetReputation());
	}

	if (UGameInstance* GI = GetGameInstance())
	{
		CachedTimeSubsystem = GI->GetSubsystem<UGameTimeSubsystem>();
	}

	if (CachedTimeSubsystem)
	{
		CachedTimeSubsystem->OnGameTimeChanged.AddDynamic(this, &UGameMenuWidget::HandleGameTimeChanged);

		if (TimeText)
		{
			TimeText->SetText(CachedTimeSubsystem->GetFormattedTime());
		}
	}

	if (StockTabButton)
	{
		StockTabButton->OnClicked.AddDynamic(this, &UGameMenuWidget::ShowStockTab);
	}
	if (ShopTabButton)
	{
		ShopTabButton->OnClicked.AddDynamic(this, &UGameMenuWidget::ShowShopTab);
	}
	if (DonateTabButton)
	{
		DonateTabButton->OnClicked.AddDynamic(this, &UGameMenuWidget::ShowDonateTab);
	}
	if (BankTabButton)
	{
		BankTabButton->OnClicked.AddDynamic(this, &UGameMenuWidget::ShowBankTab);
	}
	if (UpgradeTabButton)
	{
		UpgradeTabButton->OnClicked.AddDynamic(this, &UGameMenuWidget::ShowUpgradeTab);
	}
}

void UGameMenuWidget::NativeDestruct()
{
	if (CachedSubsystem)
	{
		CachedSubsystem->OnAssetChanged.RemoveDynamic(this, &UGameMenuWidget::HandleAssetChanged);
		CachedSubsystem->OnReputationChanged.RemoveDynamic(this, &UGameMenuWidget::HandleReputationChanged);
	}

	if (CachedTimeSubsystem)
	{
		CachedTimeSubsystem->OnGameTimeChanged.RemoveDynamic(this, &UGameMenuWidget::HandleGameTimeChanged);
	}

	Super::NativeDestruct();
}

void UGameMenuWidget::HandleAssetChanged()
{
	if (CachedSubsystem)
	{
		UpdateCashDisplay(CachedSubsystem->GetCash());
	}
}

void UGameMenuWidget::HandleReputationChanged(float NewReputation)
{
	UpdateReputationDisplay(NewReputation);
}

void UGameMenuWidget::HandleGameTimeChanged(int32 Day, int32 Hour, int32 Minute)
{
	if (TimeText && CachedTimeSubsystem)
	{
		TimeText->SetText(CachedTimeSubsystem->GetFormattedTime());
	}
}

void UGameMenuWidget::UpdateCashDisplay(float Cash)
{
	if (!CashText)
	{
		return;
	}

	CashText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.55f, 0.0f)));
	CashText->SetText(FText::Format(
		NSLOCTEXT("GameMenu", "CashFormat", "소지금 {0}"),
		FText::AsNumber(FMath::RoundToInt(Cash))
	));
}

void UGameMenuWidget::UpdateReputationDisplay(float Reputation)
{
	if (!ReputationText)
	{
		return;
	}

	ReputationText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.55f, 0.0f)));
	ReputationText->SetText(FText::Format(
		NSLOCTEXT("GameMenu", "ReputationFormat", "인지도 {0}"),
		FText::AsNumber(FMath::RoundToInt(Reputation))
	));
}

void UGameMenuWidget::ShowTabWidget(TSubclassOf<UUserWidget> WidgetClass, TObjectPtr<UUserWidget>& CachedInstance)
{
	if (!ContentArea)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMenuWidget: ContentArea가 지정되지 않았습니다."));
		return;
	}

	if (!WidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMenuWidget: 이 탭에 위젯 클래스가 지정되지 않았습니다."));
		return;
	}

	if (!CachedInstance)
	{
		CachedInstance = CreateWidget<UUserWidget>(this, WidgetClass);
	}

	if (!CachedInstance)
	{
		return;
	}

	ContentArea->ClearChildren();
	ContentArea->AddChild(CachedInstance);
}

void UGameMenuWidget::UpdateTabButtonHighlight(UButton* ActiveButton)
{
	const TArray<UButton*> AllTabButtons = { StockTabButton, ShopTabButton, DonateTabButton, BankTabButton, UpgradeTabButton };

	for (UButton* Button : AllTabButtons)
	{
		if (!Button)
		{
			continue;
		}

		// SetColorAndOpacity는 버튼 전체(배경+아이콘+텍스트 전부)를 색으로 물들여서,
		// 선택된 탭만 또렷하게, 나머지는 흐리게 보이게 만듭니다.
		Button->SetColorAndOpacity(Button == ActiveButton ? ActiveTabColor : InactiveTabColor);
	}
}

void UGameMenuWidget::ShowStockTab()
{
	ShowTabWidget(StockWidgetClass, StockWidgetInstance);
	UpdateTabButtonHighlight(StockTabButton);

	// 다른 탭에 갔다가 다시 이 탭으로 돌아온 경우에도, 예전에 상세 화면이 열려있던
	// 상태가 아니라 항상 홈(리스트) 화면부터 보이게 강제로 되돌립니다.
	if (UStockMarketWidget* StockWidget = Cast<UStockMarketWidget>(StockWidgetInstance))
	{
		StockWidget->ResetToHome();
	}
}

void UGameMenuWidget::ShowShopTab()
{
	ShowTabWidget(ShopWidgetClass, ShopWidgetInstance);
	UpdateTabButtonHighlight(ShopTabButton);
}

void UGameMenuWidget::ShowDonateTab()
{
	ShowTabWidget(DonateWidgetClass, DonateWidgetInstance);
	UpdateTabButtonHighlight(DonateTabButton);
}

void UGameMenuWidget::ShowBankTab()
{
	ShowTabWidget(BankWidgetClass, BankWidgetInstance);
	UpdateTabButtonHighlight(BankTabButton);
}

void UGameMenuWidget::ShowUpgradeTab()
{
	ShowTabWidget(UpgradeWidgetClass, UpgradeWidgetInstance);
	UpdateTabButtonHighlight(UpgradeTabButton);
}