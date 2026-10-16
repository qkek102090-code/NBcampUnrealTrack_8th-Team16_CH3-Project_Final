#include "BankPanelWidget.h"
#include "Components/SpinBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UBankPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GI = GetGameInstance())
	{
		CachedSubsystem = GI->GetSubsystem<UStockMarketSubsystem>();
	}

	if (!CachedSubsystem)
	{
		return;
	}

	CachedSubsystem->OnAssetChanged.AddDynamic(this, &UBankPanelWidget::HandleAssetChanged);
	CachedSubsystem->OnReputationChanged.AddDynamic(this, &UBankPanelWidget::HandleReputationChanged);
	CachedSubsystem->OnTradeResult.AddDynamic(this, &UBankPanelWidget::HandleTradeResult);

	if (BorrowButton)
	{
		BorrowButton->OnClicked.AddDynamic(this, &UBankPanelWidget::HandleBorrowClicked);
	}
	if (RepayButton)
	{
		RepayButton->OnClicked.AddDynamic(this, &UBankPanelWidget::HandleRepayClicked);
	}

	RefreshBankInfo();
}

void UBankPanelWidget::NativeDestruct()
{
	if (CachedSubsystem)
	{
		CachedSubsystem->OnAssetChanged.RemoveDynamic(this, &UBankPanelWidget::HandleAssetChanged);
		CachedSubsystem->OnReputationChanged.RemoveDynamic(this, &UBankPanelWidget::HandleReputationChanged);
		CachedSubsystem->OnTradeResult.RemoveDynamic(this, &UBankPanelWidget::HandleTradeResult);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MessageTimerHandle);
	}

	Super::NativeDestruct();
}

void UBankPanelWidget::RefreshBankInfo()
{
	if (!CachedSubsystem)
	{
		return;
	}

	if (ReputationText)
	{
		ReputationText->SetText(FText::Format(
			NSLOCTEXT("Bank", "ReputationFormat", "인지도 {0}"),
			FText::AsNumber(FMath::RoundToInt(CachedSubsystem->GetReputation()))
		));
	}
	if (MaxLoanAmountText)
	{
		MaxLoanAmountText->SetText(FText::Format(
			NSLOCTEXT("Bank", "MaxLoanFormat", "최대 대출 한도 {0}"),
			FText::AsNumber(FMath::RoundToInt(CachedSubsystem->GetMaxLoanAmount()))
		));
	}
	if (CurrentDebtText)
	{
		CurrentDebtText->SetText(FText::Format(
			NSLOCTEXT("Bank", "CurrentDebtFormat", "현재 채무 {0}"),
			FText::AsNumber(FMath::RoundToInt(CachedSubsystem->GetCurrentDebt()))
		));
	}
	if (AvailableLoanAmountText)
	{
		AvailableLoanAmountText->SetText(FText::Format(
			NSLOCTEXT("Bank", "AvailableLoanFormat", "대출 가능 금액 {0}"),
			FText::AsNumber(FMath::RoundToInt(CachedSubsystem->GetAvailableLoanAmount()))
		));
	}
}

void UBankPanelWidget::HandleBorrowClicked()
{
	if (!CachedSubsystem)
	{
		return;
	}

	const float Amount = BorrowAmountSpinBox ? BorrowAmountSpinBox->GetValue() : 0.f;
	CachedSubsystem->BorrowLoan(Amount);
}

void UBankPanelWidget::HandleRepayClicked()
{
	if (!CachedSubsystem)
	{
		return;
	}

	const float Amount = RepayAmountSpinBox ? RepayAmountSpinBox->GetValue() : 0.f;
	CachedSubsystem->RepayLoan(Amount);
}

void UBankPanelWidget::HandleAssetChanged()
{
	RefreshBankInfo();
}

void UBankPanelWidget::HandleReputationChanged(float NewReputation)
{
	RefreshBankInfo();
}

void UBankPanelWidget::HandleTradeResult(bool bSuccess, FName Symbol, FText Message)
{
	ShowTemporaryMessage(Message);

	if (bSuccess)
	{
		RefreshBankInfo();
	}
}

void UBankPanelWidget::ShowTemporaryMessage(const FText& Message)
{
	if (!MessageText)
	{
		return;
	}

	MessageText->SetText(Message);
	MessageText->SetVisibility(ESlateVisibility::Visible);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(MessageTimerHandle, this, &UBankPanelWidget::ClearMessageText, 3.0f, false);
	}
}

void UBankPanelWidget::ClearMessageText()
{
	if (MessageText)
	{
		MessageText->SetVisibility(ESlateVisibility::Collapsed);
	}
}