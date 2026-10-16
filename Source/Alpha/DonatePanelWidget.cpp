#include "DonatePanelWidget.h"
#include "DonationOrgListItemObject.h"
#include "DonationOrgEntryWidget.h"
#include "Components/ListView.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UDonatePanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GI = GetGameInstance())
	{
		CachedSubsystem = GI->GetSubsystem<UStockMarketSubsystem>();
	}

	if (!CachedSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("DonatePanelWidget: StockMarketSubsystem을 찾지 못했습니다."));
		return;
	}

	CachedSubsystem->OnReputationChanged.AddDynamic(this, &UDonatePanelWidget::HandleReputationChanged);
	CachedSubsystem->OnTradeResult.AddDynamic(this, &UDonatePanelWidget::HandleTradeResult);

	if (DonateButton)
	{
		DonateButton->OnClicked.AddDynamic(this, &UDonatePanelWidget::HandleDonateClicked);
	}

	if (OrgListView)
	{
		OrgListView->OnEntryWidgetGenerated().AddUObject(this, &UDonatePanelWidget::HandleEntryWidgetGenerated);
	}

	if (ReputationText)
	{
		ReputationText->SetText(FText::AsNumber(FMath::RoundToInt(CachedSubsystem->GetReputation())));
	}

	RefreshOrgList();
}

void UDonatePanelWidget::NativeDestruct()
{
	if (CachedSubsystem)
	{
		CachedSubsystem->OnReputationChanged.RemoveDynamic(this, &UDonatePanelWidget::HandleReputationChanged);
		CachedSubsystem->OnTradeResult.RemoveDynamic(this, &UDonatePanelWidget::HandleTradeResult);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MessageTimerHandle);
	}

	Super::NativeDestruct();
}

void UDonatePanelWidget::RefreshOrgList()
{
	if (!OrgListView || !CachedSubsystem)
	{
		return;
	}

	TArray<FDonationOrgRow> AllOrgs = CachedSubsystem->GetAllDonationOrgs();

	AllOrgs.Sort([](const FDonationOrgRow& A, const FDonationOrgRow& B)
	{
		return A.DonationAmount < B.DonationAmount;
	});

	TArray<UObject*> ListItems;
	for (const FDonationOrgRow& Org : AllOrgs)
	{
		UDonationOrgListItemObject* Item = NewObject<UDonationOrgListItemObject>(this);
		Item->OrgID = Org.OrgID;
		Item->OrgName = Org.OrgName;
		Item->Description = Org.Description;
		Item->DonationAmount = Org.DonationAmount;

		if (!Org.Icon.IsNull())
		{
			Item->Icon = Org.Icon.LoadSynchronous();
		}

		ListItems.Add(Item);
	}

	OrgListView->SetListItems(ListItems);
}

void UDonatePanelWidget::HandleEntryWidgetGenerated(UUserWidget& EntryWidget)
{
	if (UDonationOrgEntryWidget* Entry = Cast<UDonationOrgEntryWidget>(&EntryWidget))
	{
		Entry->OnOrgSelected.AddUniqueDynamic(this, &UDonatePanelWidget::HandleOrgSelected);
	}
}

void UDonatePanelWidget::HandleOrgSelected(FName OrgID)
{
	SelectedOrgID = OrgID;

	if (!CachedSubsystem)
	{
		return;
	}

	for (const FDonationOrgRow& Org : CachedSubsystem->GetAllDonationOrgs())
	{
		if (Org.OrgID == OrgID)
		{
			if (SelectedOrgText)
			{
				SelectedOrgText->SetText(Org.OrgName);
			}
			if (SelectedAmountText)
			{
				SelectedAmountText->SetText(FText::AsNumber(FMath::RoundToInt(Org.DonationAmount)));
			}
			break;
		}
	}
}

void UDonatePanelWidget::HandleDonateClicked()
{
	if (!CachedSubsystem)
	{
		return;
	}

	if (SelectedOrgID.IsNone())
	{
		if (MessageText)
		{
			MessageText->SetText(FText::FromString(TEXT("먼저 카드를 눌러 기부할 단체를 선택해주세요.")));
		}
		return;
	}

	CachedSubsystem->DonateToOrg(SelectedOrgID);
}

void UDonatePanelWidget::HandleReputationChanged(float NewReputation)
{
	if (ReputationText)
	{
		ReputationText->SetText(FText::AsNumber(FMath::RoundToInt(NewReputation)));
	}
}

void UDonatePanelWidget::HandleTradeResult(bool bSuccess, FName Symbol, FText Message)
{
	ShowTemporaryMessage(Message);
}

void UDonatePanelWidget::ShowTemporaryMessage(const FText& Message)
{
	if (!MessageText)
	{
		return;
	}

	MessageText->SetText(Message);
	MessageText->SetVisibility(ESlateVisibility::Visible);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(MessageTimerHandle, this, &UDonatePanelWidget::ClearMessageText, 3.0f, false);
	}
}

void UDonatePanelWidget::ClearMessageText()
{
	if (MessageText)
	{
		MessageText->SetVisibility(ESlateVisibility::Collapsed);
	}
}
