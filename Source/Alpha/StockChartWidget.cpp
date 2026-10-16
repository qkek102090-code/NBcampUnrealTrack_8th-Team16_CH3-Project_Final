#include "StockChartWidget.h"
#include "SStockChartWidget.h"

TSharedRef<SWidget> UStockChartWidget::RebuildWidget()
{
	MyChart = SNew(SStockChartWidget)
		.UpColor(UpColor)
		.DownColor(DownColor)
		.ShortMAColor(ShortMAColor)
		.LongMAColor(LongMAColor)
		.ShortMAPeriod(ShortMAPeriod)
		.LongMAPeriod(LongMAPeriod)
		.MaxVisibleCandles(MaxVisibleCandles)
		.NumGridlines(NumGridlines)
		.GridlineColor(GridlineColor)
		.bShowPriceLabels(bShowPriceLabels)
		.PriceLabelColor(PriceLabelColor)
		.CurrentPriceLabelColor(CurrentPriceLabelColor)
		.GridlineMarginWidth(GridlineMarginWidth);

	return MyChart.ToSharedRef();
}

void UStockChartWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	// MyChart가 SStockChartWidget을 강하게 붙잡고 있어서, 여기서 놓아주지 않으면
	// 이 UWidget이 파괴돼도 내부 Slate 위젯이 계속 메모리에 남아있게 됩니다.
	MyChart.Reset();
}

void UStockChartWidget::SetCandles(const TArray<FCandleData>& InCandles)
{
	if (!MyChart.IsValid())
	{
		return;
	}

	TArray<FChartCandle> ChartCandles;
	ChartCandles.Reserve(InCandles.Num());

	for (const FCandleData& Candle : InCandles)
	{
		FChartCandle ChartCandle;
		ChartCandle.Open = Candle.Open;
		ChartCandle.High = Candle.High;
		ChartCandle.Low = Candle.Low;
		ChartCandle.Close = Candle.Close;
		ChartCandles.Add(ChartCandle);
	}

	MyChart->SetCandles(ChartCandles);
	MyChart->Invalidate(EInvalidateWidget::Paint);
}
