#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StockMarketSubsystem.h"
#include "StockMarketWidget.generated.h"

class UListView;
class UTextBlock;
class UButton;
class UUserWidget;
class UStockChartWidget;
class UPriceAxisWidget;
class UMyStocksPanelWidget;
class UStockDetailWidget;
class UPanelWidget;

UENUM(BlueprintType)
enum class EStockSortMode : uint8
{
	None,
	Symbol,
	Name,
	Price,
	PercentChange
};

UENUM(BlueprintType)
enum class EStockViewMode : uint8
{
	AllStocks,
	MyStocks,
	Watchlist
};

UCLASS()
class ALPHA_API UStockMarketWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UListView> StockListView;

	// 리스트가 비어있을 때(관심 주식 없음, 보유 종목 없음 등) 표시되는 안내 문구
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EmptyStateText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CashText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> SortBySymbolButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> SortByNameButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> SortByPriceButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> SortByPercentChangeButton;

	// 각 정렬 버튼 안의 라벨 텍스트. 누를 때마다 "심볼순" -> "심볼순 ▲" -> "심볼순 ▼" ->
	// "심볼순"(기본, 정렬 안 함) 순서로 바뀝니다. 버튼 안에 배치된 TextBlock을 여기 연결하세요.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SortSymbolLabelText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SortNameLabelText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SortPriceLabelText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SortPercentChangeLabelText;

	// 종목 리스트에 마우스를 올렸을 때 잠깐 보여주는 미리보기 차트.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UStockChartWidget> HoverPreviewChart;

	// HoverPreviewChart와 같은 크기로 겹쳐서 배치하세요. PriceChart+PriceAxis 조합과 동일한 패턴입니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UPriceAxisWidget> HoverPreviewPriceAxis;

	// 미리보기 차트 위에 표시되는 정보 (종목명, 실시간 가격, 등락률)
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HoverPreviewNameText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HoverPreviewPriceText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HoverPreviewPercentText;

	// 홈 화면(리스트+정렬+탭 등)을 감싸는 컨테이너. 상세 화면을 열면 이 컨테이너를 숨깁니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> HomeContent;

	// 종목 클릭 시 들어가는 상세 화면. HomeContent와 형제로 배치하고, 기본 Visibility는
	// Collapsed로 두세요. WidgetSwitcher 없이 이 둘의 Visibility만 서로 바꿔서 전환합니다.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UStockDetailWidget> StockDetailWidget;

	// 주식 탭 안의 서브탭: 모든 주식 / 내 투자 / 관심 주식
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> AllStocksTabButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> WatchlistTabButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UButton> MyStocksToggleButton;

	// "모든 주식" 탭에서만 함께 보이는 내 투자(보유 종목) 미리보기 패널.
	UPROPERTY(BlueprintReadOnly, meta = (BindWidgetOptional))
	TObjectPtr<UMyStocksPanelWidget> MyStocksPanel;

	// 다른 탭(상점 등)에 갔다가 이 주식 탭으로 다시 돌아올 때, GameMenuWidget이 호출해서
	// 상세 화면이 열려있었더라도 항상 홈(리스트) 화면부터 보이게 강제로 되돌립니다.
	UFUNCTION(BlueprintCallable, Category = "Stock")
	void ResetToHome();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable, Category = "Stock")
	void RefreshStockList();

	void HandleEntryWidgetGenerated(UUserWidget& EntryWidget);

	UFUNCTION()
	void HandleEntryClicked(FName Symbol);

	UFUNCTION()
	void HandleEntryHovered(FName Symbol);

	UFUNCTION()
	void HandleEntryUnhovered();

	UFUNCTION()
	void HandleAssetChanged();

	UFUNCTION()
	void HandleAllPricesUpdated();

	UFUNCTION()
	void SortBySymbol();

	UFUNCTION()
	void SortByName();

	UFUNCTION()
	void SortByPrice();

	UFUNCTION()
	void SortByPercentChange();

	UFUNCTION()
	void ShowAllStocksTab();

	UFUNCTION()
	void ShowWatchlistTab();

	UFUNCTION()
	void ShowMyStocksTab();

	UFUNCTION()
	void HandleEntryWatchlistToggled(FName Symbol);

	UFUNCTION()
	void HandleWatchlistChanged();

	UFUNCTION()
	void HandleMyStocksPanelSelected(FName Symbol);

	// 상세 화면에서 뒤로가기를 눌렀을 때 홈으로 돌아옵니다.
	UFUNCTION()
	void HandleStockDetailBackRequested();

	void OpenStockDetail(FName Symbol);
	void UpdateViewTabHighlight(UButton* ActiveButton);

	// 미리보기 차트 + 위 정보(이름/가격/등락률)를 한 번에 갱신합니다.
	void UpdateHoverPreview(FName Symbol);

	// 정렬 버튼 4개 옆의 ▲/▼/= 표시를 지금 상태에 맞춰 다시 그립니다.
	void UpdateSortIndicators();

	UPROPERTY(BlueprintReadOnly, Category = "Stock")
	EStockSortMode CurrentSortMode = EStockSortMode::None;

	UPROPERTY(BlueprintReadOnly, Category = "Stock")
	bool bSortDescending = false;

	UPROPERTY(BlueprintReadOnly, Category = "Stock")
	EStockViewMode CurrentViewMode = EStockViewMode::AllStocks;

	// 지금 호버 중인 종목. 마우스가 리스트를 벗어나면 NAME_None으로 돌아갑니다.
	FName HoveredSymbol;

	UPROPERTY()
	TObjectPtr<UStockMarketSubsystem> CachedSubsystem;
};