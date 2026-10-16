#include "MyStocksPanelWidget.h"
#include "StockListItemObject.h"
#include "MyStockEntryWidget.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"

void UMyStocksPanelWidget::NativeConstruct()
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

	CachedSubsystem->OnAssetChanged.AddDynamic(this, &UMyStocksPanelWidget::HandleAssetChanged);
	CachedSubsystem->OnAllPricesUpdated.AddDynamic(this, &UMyStocksPanelWidget::HandleAllPricesUpdated);

	if (MyStocksListView)
	{
		MyStocksListView->OnEntryWidgetGenerated().AddUObject(this, &UMyStocksPanelWidget::HandleEntryWidgetGenerated);
	}

	RefreshMyStocksList();
}

void UMyStocksPanelWidget::NativeDestruct()
{
	if (CachedSubsystem)
	{
		CachedSubsystem->OnAssetChanged.RemoveDynamic(this, &UMyStocksPanelWidget::HandleAssetChanged);
		CachedSubsystem->OnAllPricesUpdated.RemoveDynamic(this, &UMyStocksPanelWidget::HandleAllPricesUpdated);
	}

	Super::NativeDestruct();
}

void UMyStocksPanelWidget::RefreshMyStocksList()
{
	if (!MyStocksListView || !CachedSubsystem)
	{
		return;
	}

	TArray<UObject*> ListItems;

	// 전체 종목을 훑으면서, 리스트 항목 생성과 동시에 총 평가액/총 매수원가를 누적합니다.
	float TotalValue = 0.f;
	float TotalCost = 0.f;

	for (const FStockData& Stock : CachedSubsystem->GetAllStocks())
	{
		const int32 HoldingQty = CachedSubsystem->GetHoldingQuantity(Stock.Symbol);
		if (HoldingQty <= 0)
		{
			continue;
		}

		const float AverageBuyPrice = CachedSubsystem->GetAverageBuyPrice(Stock.Symbol);

		UStockListItemObject* Item = NewObject<UStockListItemObject>(this);
		Item->Symbol = Stock.Symbol;
		Item->DisplayName = Stock.DisplayName;
		Item->CurrentPrice = Stock.CurrentPrice;
		Item->StartPrice = Stock.StartPrice;
		Item->bInWatchlist = CachedSubsystem->IsInWatchlist(Stock.Symbol);
		Item->HoldingQuantity = HoldingQty;
		Item->AverageBuyPrice = AverageBuyPrice;

		// 시작가 기준이 아니라 "내가 산 가격" 기준으로 등락을 계산합니다.
		Item->GainAmount = Stock.CurrentPrice - AverageBuyPrice;
		Item->GainPercent = (AverageBuyPrice > 0.f)
			? (Item->GainAmount / AverageBuyPrice) * 100.f
			: 0.f;

		ListItems.Add(Item);

		TotalValue += Stock.CurrentPrice * HoldingQty;
		TotalCost += AverageBuyPrice * HoldingQty;
	}

	MyStocksListView->SetListItems(ListItems);

	if (EmptyStateText)
	{
		if (ListItems.Num() == 0)
		{
			EmptyStateText->SetText(NSLOCTEXT("Stock", "EmptyMyStocksPanel", "구매한 주식이 없습니다."));
			EmptyStateText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			EmptyStateText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (TotalValueText)
	{
		TotalValueText->SetText(FText::Format(
			NSLOCTEXT("Stock", "TotalValueFormat", "총 평가액 {0}원"),
			FText::AsNumber(FMath::RoundToInt(TotalValue))
		));
	}

	if (TotalGainText)
	{
		const float TotalGainAmount = TotalValue - TotalCost;
		const float TotalGainPercent = (TotalCost > 0.f) ? (TotalGainAmount / TotalCost) * 100.f : 0.f;
		const int32 TotalGainAmountRounded = FMath::RoundToInt(TotalGainAmount);

		const FString GainString = FString::Printf(
			TEXT("%s%s원 (%+.2f%%)"),
			TotalGainAmountRounded >= 0 ? TEXT("+") : TEXT(""),
			*FText::AsNumber(TotalGainAmountRounded).ToString(),
			TotalGainPercent
		);
		TotalGainText->SetText(FText::FromString(GainString));

		if (TotalGainAmount > 0.f)
		{
			TotalGainText->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.8f, 0.3f)));
		}
		else if (TotalGainAmount < 0.f)
		{
			TotalGainText->SetColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.25f, 0.25f)));
		}
		else
		{
			TotalGainText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		}
	}
}

void UMyStocksPanelWidget::HandleEntryWidgetGenerated(UUserWidget& EntryWidget)
{
	if (UMyStockEntryWidget* Entry = Cast<UMyStockEntryWidget>(&EntryWidget))
	{
		Entry->OnEntryClicked.AddUniqueDynamic(this, &UMyStocksPanelWidget::HandleEntryClicked);
	}
}

void UMyStocksPanelWidget::HandleEntryClicked(FName Symbol)
{
	OnStockSelected.Broadcast(Symbol);
}

void UMyStocksPanelWidget::HandleAssetChanged()
{
	RefreshMyStocksList();
}

void UMyStocksPanelWidget::HandleAllPricesUpdated()
{
	RefreshMyStocksList();
}
