#include "StockMarketWidget.h"
#include "StockListItemObject.h"
#include "StockListEntryWidget.h"
#include "StockChartWidget.h"
#include "PriceAxisWidget.h"
#include "MyStocksPanelWidget.h"
#include "StockDetailWidget.h"
#include "Components/ListView.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"

void UStockMarketWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UGameInstance* GI = GetGameInstance())
	{
		CachedSubsystem = GI->GetSubsystem<UStockMarketSubsystem>();
	}

	if (!CachedSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("StockMarketWidget: StockMarketSubsystem을 찾지 못했습니다."));
		return;
	}

	CachedSubsystem->OnAssetChanged.AddDynamic(this, &UStockMarketWidget::HandleAssetChanged);
	CachedSubsystem->OnAllPricesUpdated.AddDynamic(this, &UStockMarketWidget::HandleAllPricesUpdated);
	CachedSubsystem->OnWatchlistChanged.AddDynamic(this, &UStockMarketWidget::HandleWatchlistChanged);

	if (SortBySymbolButton)
	{
		SortBySymbolButton->OnClicked.AddDynamic(this, &UStockMarketWidget::SortBySymbol);
	}
	if (SortByNameButton)
	{
		SortByNameButton->OnClicked.AddDynamic(this, &UStockMarketWidget::SortByName);
	}
	if (SortByPriceButton)
	{
		SortByPriceButton->OnClicked.AddDynamic(this, &UStockMarketWidget::SortByPrice);
	}
	if (SortByPercentChangeButton)
	{
		SortByPercentChangeButton->OnClicked.AddDynamic(this, &UStockMarketWidget::SortByPercentChange);
	}

	if (AllStocksTabButton)
	{
		AllStocksTabButton->OnClicked.AddDynamic(this, &UStockMarketWidget::ShowAllStocksTab);
	}
	if (WatchlistTabButton)
	{
		WatchlistTabButton->OnClicked.AddDynamic(this, &UStockMarketWidget::ShowWatchlistTab);
	}
	if (MyStocksToggleButton)
	{
		MyStocksToggleButton->OnClicked.AddDynamic(this, &UStockMarketWidget::ShowMyStocksTab);
	}

	if (MyStocksPanel)
	{
		MyStocksPanel->OnStockSelected.AddDynamic(this, &UStockMarketWidget::HandleMyStocksPanelSelected);
	}

	if (StockDetailWidget)
	{
		StockDetailWidget->OnBackRequested.AddDynamic(this, &UStockMarketWidget::HandleStockDetailBackRequested);
	}

	if (StockListView)
	{
		StockListView->OnEntryWidgetGenerated().AddUObject(this, &UStockMarketWidget::HandleEntryWidgetGenerated);
	}

	if (CashText)
	{
		CashText->SetText(FText::AsNumber(FMath::RoundToInt(CachedSubsystem->GetCash())));
	}

	// 시작할 때는 홈(리스트)을 보여주고, 상세 화면은 숨겨둡니다.
	if (HomeContent)
	{
		HomeContent->SetVisibility(ESlateVisibility::Visible);
	}
	if (StockDetailWidget)
	{
		StockDetailWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	UpdateViewTabHighlight(AllStocksTabButton);

	if (MyStocksPanel)
	{
		MyStocksPanel->SetVisibility(ESlateVisibility::Visible);
	}

	RefreshStockList();
}

void UStockMarketWidget::NativeDestruct()
{
	if (CachedSubsystem)
	{
		CachedSubsystem->OnAssetChanged.RemoveDynamic(this, &UStockMarketWidget::HandleAssetChanged);
		CachedSubsystem->OnAllPricesUpdated.RemoveDynamic(this, &UStockMarketWidget::HandleAllPricesUpdated);
		CachedSubsystem->OnWatchlistChanged.RemoveDynamic(this, &UStockMarketWidget::HandleWatchlistChanged);
	}

	if (MyStocksPanel)
	{
		MyStocksPanel->OnStockSelected.RemoveDynamic(this, &UStockMarketWidget::HandleMyStocksPanelSelected);
	}

	if (StockDetailWidget)
	{
		StockDetailWidget->OnBackRequested.RemoveDynamic(this, &UStockMarketWidget::HandleStockDetailBackRequested);
	}

	Super::NativeDestruct();
}

void UStockMarketWidget::RefreshStockList()
{
	if (!StockListView || !CachedSubsystem)
	{
		return;
	}

	UpdateSortIndicators();

	TArray<FStockData> AllStocks = CachedSubsystem->GetAllStocks();

	if (CurrentViewMode == EStockViewMode::MyStocks)
	{
		AllStocks.RemoveAll([this](const FStockData& Stock)
			{
				return CachedSubsystem->GetHoldingQuantity(Stock.Symbol) <= 0;
			});
	}
	else if (CurrentViewMode == EStockViewMode::Watchlist)
	{
		AllStocks.RemoveAll([this](const FStockData& Stock)
			{
				return !CachedSubsystem->IsInWatchlist(Stock.Symbol);
			});
	}

	// "기본 정렬" 상태(None)면 아무 기준으로도 정렬하지 않고 원래 순서 그대로 둡니다.
	if (CurrentSortMode != EStockSortMode::None)
	{
		AllStocks.Sort([this](const FStockData& A, const FStockData& B)
			{
				switch (CurrentSortMode)
				{
				case EStockSortMode::Symbol:
					return bSortDescending
						? (A.Symbol.ToString() > B.Symbol.ToString())
						: (A.Symbol.ToString() < B.Symbol.ToString());

				case EStockSortMode::Name:
					return bSortDescending
						? (A.DisplayName.ToString() > B.DisplayName.ToString())
						: (A.DisplayName.ToString() < B.DisplayName.ToString());

				case EStockSortMode::Price:
					return bSortDescending
						? (A.CurrentPrice > B.CurrentPrice)
						: (A.CurrentPrice < B.CurrentPrice);

				case EStockSortMode::PercentChange:
				default:
				{
					const float PercentA = (A.StartPrice > 0.f) ? ((A.CurrentPrice - A.StartPrice) / A.StartPrice) : 0.f;
					const float PercentB = (B.StartPrice > 0.f) ? ((B.CurrentPrice - B.StartPrice) / B.StartPrice) : 0.f;
					return bSortDescending ? (PercentA > PercentB) : (PercentA < PercentB);
				}
				}
			});
	}

	TArray<UObject*> ListItems;
	for (const FStockData& Stock : AllStocks)
	{
		UStockListItemObject* Item = NewObject<UStockListItemObject>(this);
		Item->Symbol = Stock.Symbol;
		Item->DisplayName = Stock.DisplayName;
		Item->CurrentPrice = Stock.CurrentPrice;
		Item->StartPrice = Stock.StartPrice;
		Item->PercentChange = (Stock.StartPrice > 0.f)
			? ((Stock.CurrentPrice - Stock.StartPrice) / Stock.StartPrice) * 100.f
			: 0.f;
		Item->bInWatchlist = CachedSubsystem->IsInWatchlist(Stock.Symbol);
		Item->HoldingQuantity = CachedSubsystem->GetHoldingQuantity(Stock.Symbol);
		ListItems.Add(Item);
	}

	StockListView->SetListItems(ListItems);

	if (EmptyStateText)
	{
		if (AllStocks.Num() == 0)
		{
			FText Message;
			switch (CurrentViewMode)
			{
			case EStockViewMode::Watchlist:
				Message = NSLOCTEXT("Stock", "EmptyWatchlist", "관심을 표시한 종목이 없습니다.");
				break;
			case EStockViewMode::MyStocks:
				Message = NSLOCTEXT("Stock", "EmptyMyStocksTab", "구매한 주식이 없습니다.");
				break;
			default:
				Message = NSLOCTEXT("Stock", "EmptyAllStocks", "표시할 종목이 없습니다.");
				break;
			}

			EmptyStateText->SetText(Message);
			EmptyStateText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			EmptyStateText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 마우스로 아무것도 호버하고 있지 않으면, 리스트 맨 위 종목을 기본으로 보여줍니다.
	if (HoveredSymbol.IsNone() && AllStocks.Num() > 0)
	{
		UpdateHoverPreview(AllStocks[0].Symbol);
	}
}

void UStockMarketWidget::HandleEntryWidgetGenerated(UUserWidget& EntryWidget)
{
	if (UStockListEntryWidget* Entry = Cast<UStockListEntryWidget>(&EntryWidget))
	{
		Entry->OnEntryClicked.AddUniqueDynamic(this, &UStockMarketWidget::HandleEntryClicked);
		Entry->OnWatchlistToggled.AddUniqueDynamic(this, &UStockMarketWidget::HandleEntryWatchlistToggled);
		Entry->OnEntryHovered.AddUniqueDynamic(this, &UStockMarketWidget::HandleEntryHovered);
		Entry->OnEntryUnhovered.AddUniqueDynamic(this, &UStockMarketWidget::HandleEntryUnhovered);
	}
}

void UStockMarketWidget::HandleEntryClicked(FName Symbol)
{
	OpenStockDetail(Symbol);
}

void UStockMarketWidget::HandleEntryHovered(FName Symbol)
{
	HoveredSymbol = Symbol;
	UpdateHoverPreview(Symbol);
}

void UStockMarketWidget::HandleEntryUnhovered()
{
	HoveredSymbol = NAME_None;

	// 숨기는 대신, 지금 리스트 맨 위에 있는 종목으로 되돌립니다.
	RefreshStockList();
}

void UStockMarketWidget::HandleAssetChanged()
{
	if (CashText && CachedSubsystem)
	{
		CashText->SetText(FText::AsNumber(FMath::RoundToInt(CachedSubsystem->GetCash())));
	}
}

void UStockMarketWidget::HandleAllPricesUpdated()
{
	RefreshStockList();

	// 호버 중인 종목이 있으면 미리보기(차트+텍스트)도 최신 가격으로 갱신합니다.
	// (호버 안 하고 있을 때의 기본 미리보기는 RefreshStockList() 안에서 이미 처리됩니다)
	if (!HoveredSymbol.IsNone())
	{
		UpdateHoverPreview(HoveredSymbol);
	}
}

void UStockMarketWidget::SortBySymbol()
{
	if (CurrentSortMode != EStockSortMode::Symbol)
	{
		CurrentSortMode = EStockSortMode::Symbol;
		bSortDescending = false;
	}
	else if (!bSortDescending)
	{
		bSortDescending = true;
	}
	else
	{
		CurrentSortMode = EStockSortMode::None;
		bSortDescending = false;
	}
	RefreshStockList();
}

void UStockMarketWidget::SortByName()
{
	if (CurrentSortMode != EStockSortMode::Name)
	{
		CurrentSortMode = EStockSortMode::Name;
		bSortDescending = false;
	}
	else if (!bSortDescending)
	{
		bSortDescending = true;
	}
	else
	{
		CurrentSortMode = EStockSortMode::None;
		bSortDescending = false;
	}
	RefreshStockList();
}

void UStockMarketWidget::SortByPrice()
{
	if (CurrentSortMode != EStockSortMode::Price)
	{
		CurrentSortMode = EStockSortMode::Price;
		bSortDescending = false;
	}
	else if (!bSortDescending)
	{
		bSortDescending = true;
	}
	else
	{
		CurrentSortMode = EStockSortMode::None;
		bSortDescending = false;
	}
	RefreshStockList();
}

void UStockMarketWidget::SortByPercentChange()
{
	if (CurrentSortMode != EStockSortMode::PercentChange)
	{
		CurrentSortMode = EStockSortMode::PercentChange;
		bSortDescending = false;
	}
	else if (!bSortDescending)
	{
		bSortDescending = true;
	}
	else
	{
		CurrentSortMode = EStockSortMode::None;
		bSortDescending = false;
	}
	RefreshStockList();
}

void UStockMarketWidget::UpdateSortIndicators()
{
	// 라벨은 디자이너에서 따로 넣으신 정적 텍스트를 쓰시니, 여기서는 화살표만 표시합니다.
	// 이 열이 활성 상태가 아니면 빈 문자열(아무것도 안 보임), 활성 상태면 방향 화살표만 표시합니다.
	auto BuildIndicator = [this](EStockSortMode Mode) -> FString
		{
			if (CurrentSortMode != Mode)
			{
				return TEXT("");
			}
			return bSortDescending ? TEXT("▼") : TEXT("▲");
		};

	if (SortSymbolLabelText)
	{
		SortSymbolLabelText->SetText(FText::FromString(BuildIndicator(EStockSortMode::Symbol)));
	}
	if (SortNameLabelText)
	{
		SortNameLabelText->SetText(FText::FromString(BuildIndicator(EStockSortMode::Name)));
	}
	if (SortPriceLabelText)
	{
		SortPriceLabelText->SetText(FText::FromString(BuildIndicator(EStockSortMode::Price)));
	}
	if (SortPercentChangeLabelText)
	{
		SortPercentChangeLabelText->SetText(FText::FromString(BuildIndicator(EStockSortMode::PercentChange)));
	}
}

void UStockMarketWidget::ShowAllStocksTab()
{
	CurrentViewMode = EStockViewMode::AllStocks;
	UpdateViewTabHighlight(AllStocksTabButton);

	if (MyStocksPanel)
	{
		MyStocksPanel->SetVisibility(ESlateVisibility::Visible);
	}

	RefreshStockList();
}

void UStockMarketWidget::ShowWatchlistTab()
{
	CurrentViewMode = EStockViewMode::Watchlist;
	UpdateViewTabHighlight(WatchlistTabButton);

	if (MyStocksPanel)
	{
		MyStocksPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	RefreshStockList();
}

void UStockMarketWidget::ShowMyStocksTab()
{
	CurrentViewMode = EStockViewMode::MyStocks;
	UpdateViewTabHighlight(MyStocksToggleButton);

	if (MyStocksPanel)
	{
		MyStocksPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	RefreshStockList();
}

void UStockMarketWidget::UpdateViewTabHighlight(UButton* ActiveButton)
{
	const TArray<UButton*> AllTabButtons = { AllStocksTabButton, MyStocksToggleButton, WatchlistTabButton };

	for (UButton* Button : AllTabButtons)
	{
		if (!Button)
		{
			continue;
		}

		Button->SetBackgroundColor(Button == ActiveButton
			? FLinearColor(1.0f, 0.55f, 0.0f)
			: FLinearColor(0.6f, 0.6f, 0.6f));
	}
}

void UStockMarketWidget::HandleEntryWatchlistToggled(FName Symbol)
{
	if (CachedSubsystem)
	{
		CachedSubsystem->ToggleWatchlist(Symbol);
	}
}

void UStockMarketWidget::HandleWatchlistChanged()
{
	RefreshStockList();
}

void UStockMarketWidget::HandleMyStocksPanelSelected(FName Symbol)
{
	OpenStockDetail(Symbol);
}

void UStockMarketWidget::HandleStockDetailBackRequested()
{
	ResetToHome();
}

void UStockMarketWidget::ResetToHome()
{
	if (HomeContent)
	{
		HomeContent->SetVisibility(ESlateVisibility::Visible);
	}
	if (StockDetailWidget)
	{
		StockDetailWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	// 서브탭이 "관심 주식" 등에 머물러 있었을 수 있으니, 항상 "모든 주식"으로 되돌립니다.
	CurrentViewMode = EStockViewMode::AllStocks;
	UpdateViewTabHighlight(AllStocksTabButton);

	if (MyStocksPanel)
	{
		MyStocksPanel->SetVisibility(ESlateVisibility::Visible);
	}

	// 상세 화면에서 매수/매도를 했을 수 있으니, 홈으로 돌아오면 리스트를 최신 상태로 갱신합니다.
	RefreshStockList();
}

void UStockMarketWidget::OpenStockDetail(FName Symbol)
{
	if (!StockDetailWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("StockMarketWidget: StockDetailWidget이 바인딩되지 않았습니다. WBP_StockMarket에서 이름을 확인해주세요."));
		return;
	}

	StockDetailWidget->ShowStock(Symbol);
	StockDetailWidget->SetVisibility(ESlateVisibility::Visible);

	if (HomeContent)
	{
		HomeContent->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UStockMarketWidget::UpdateHoverPreview(FName Symbol)
{
	if (!CachedSubsystem || Symbol.IsNone())
	{
		return;
	}

	if (HoverPreviewChart)
	{
		const TArray<FCandleData> Candles = CachedSubsystem->GetCandleHistory(Symbol);
		HoverPreviewChart->SetCandles(Candles);
		HoverPreviewChart->SetVisibility(ESlateVisibility::Visible);

		// PriceChart+PriceAxis 조합과 동일하게, 여기서도 캔들 범위를 계산해서 축에 전달합니다.
		if (HoverPreviewPriceAxis && Candles.Num() > 0)
		{
			const int32 TargetVisible = FMath::Max(HoverPreviewChart->MaxVisibleCandles, 1);
			const int32 VisibleCount = FMath::Min(Candles.Num(), TargetVisible);
			const int32 StartIndex = Candles.Num() - VisibleCount;

			float MinPrice = Candles[StartIndex].Low;
			float MaxPrice = Candles[StartIndex].High;
			for (int32 Index = StartIndex; Index < Candles.Num(); ++Index)
			{
				MinPrice = FMath::Min(MinPrice, Candles[Index].Low);
				MaxPrice = FMath::Max(MaxPrice, Candles[Index].High);
			}

			HoverPreviewPriceAxis->SetPriceRange(MinPrice, MaxPrice, CachedSubsystem->GetCurrentPrice(Symbol));
		}
	}

	const float CurrentPrice = CachedSubsystem->GetCurrentPrice(Symbol);
	const float StartPrice = CachedSubsystem->GetStartPrice(Symbol);

	if (HoverPreviewNameText)
	{
		HoverPreviewNameText->SetText(FText::Format(
			NSLOCTEXT("Stock", "PreviewNameFormat", "{0}  {1}"),
			FText::FromName(Symbol),
			CachedSubsystem->GetStockDisplayName(Symbol)
		));
	}

	if (HoverPreviewPriceText)
	{
		HoverPreviewPriceText->SetText(FText::AsNumber(FMath::RoundToInt(CurrentPrice)));
	}

	if (HoverPreviewPercentText)
	{
		const float PercentChange = (StartPrice > 0.f)
			? ((CurrentPrice - StartPrice) / StartPrice) * 100.f
			: 0.f;

		HoverPreviewPercentText->SetText(FText::FromString(FString::Printf(TEXT("%+.2f%%"), PercentChange)));

		if (PercentChange > 0.f)
		{
			HoverPreviewPercentText->SetColorAndOpacity(FSlateColor(FLinearColor(0.2f, 0.8f, 0.3f)));
		}
		else if (PercentChange < 0.f)
		{
			HoverPreviewPercentText->SetColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.25f, 0.25f)));
		}
		else
		{
			HoverPreviewPercentText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		}
	}
}