#include "ShopPanelWidget.h"
#include "ShopListItemObject.h"
#include "ShopListEntryWidget.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"

void UShopPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GI = GetGameInstance())
	{
		CachedSubsystem = GI->GetSubsystem<UStockMarketSubsystem>();
	}

	if (!CachedSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("ShopPanelWidget: StockMarketSubsystem을 찾지 못했습니다."));
		return;
	}

	CachedSubsystem->OnReputationChanged.AddDynamic(this, &UShopPanelWidget::HandleReputationChanged);
	CachedSubsystem->OnTradeResult.AddDynamic(this, &UShopPanelWidget::HandleTradeResult);

	if (ShopListView)
	{
		ShopListView->OnEntryWidgetGenerated().AddUObject(this, &UShopPanelWidget::HandleEntryWidgetGenerated);
	}

	if (ReputationText)
	{
		ReputationText->SetText(FText::AsNumber(FMath::RoundToInt(CachedSubsystem->GetReputation())));
	}

	RefreshShopList();
}

void UShopPanelWidget::NativeDestruct()
{
	if (CachedSubsystem)
	{
		CachedSubsystem->OnReputationChanged.RemoveDynamic(this, &UShopPanelWidget::HandleReputationChanged);
		CachedSubsystem->OnTradeResult.RemoveDynamic(this, &UShopPanelWidget::HandleTradeResult);
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MessageTimerHandle);
	}

	Super::NativeDestruct();
}

void UShopPanelWidget::RefreshShopList()
{
	if (!ShopListView || !CachedSubsystem)
	{
		return;
	}

	TArray<UObject*> ListItems;
	for (const FShopItemRow& ShopItem : CachedSubsystem->GetAllShopItems())
	{
		UShopListItemObject* Item = NewObject<UShopListItemObject>(this);
		Item->ItemID = ShopItem.ItemID;
		Item->DisplayName = ShopItem.DisplayName;
		Item->Price = ShopItem.Price;
		Item->RequiredReputation = ShopItem.RequiredReputation;
		Item->bUnlocked = CachedSubsystem->IsItemUnlocked(ShopItem.ItemID);
		Item->bOwned = CachedSubsystem->IsItemOwned(ShopItem.ItemID);
		ListItems.Add(Item);
	}

	ShopListView->SetListItems(ListItems);
}

void UShopPanelWidget::HandleEntryWidgetGenerated(UUserWidget& EntryWidget)
{
	if (UShopListEntryWidget* Entry = Cast<UShopListEntryWidget>(&EntryWidget))
	{
		Entry->OnPurchaseClicked.AddUniqueDynamic(this, &UShopPanelWidget::HandlePurchaseClicked);
	}
}

void UShopPanelWidget::HandlePurchaseClicked(FName ItemID)
{
	if (CachedSubsystem)
	{
		CachedSubsystem->PurchaseShopItem(ItemID);
	}
}

void UShopPanelWidget::HandleReputationChanged(float NewReputation)
{
	if (ReputationText)
	{
		ReputationText->SetText(FText::AsNumber(FMath::RoundToInt(NewReputation)));
	}

	RefreshShopList();
}

void UShopPanelWidget::HandleTradeResult(bool bSuccess, FName ItemID, FText Message)
{
	ShowTemporaryMessage(Message);

	if (bSuccess)
	{
		RefreshShopList();
	}
}

void UShopPanelWidget::ShowTemporaryMessage(const FText& Message)
{
	if (!MessageText)
	{
		return;
	}

	MessageText->SetText(Message);
	MessageText->SetVisibility(ESlateVisibility::Visible);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(MessageTimerHandle, this, &UShopPanelWidget::ClearMessageText, 3.0f, false);
	}
}

void UShopPanelWidget::ClearMessageText()
{
	if (MessageText)
	{
		MessageText->SetVisibility(ESlateVisibility::Collapsed);
	}
}
