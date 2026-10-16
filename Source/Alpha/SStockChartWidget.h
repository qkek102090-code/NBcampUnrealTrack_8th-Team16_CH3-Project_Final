#pragma once

#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"

struct FChartCandle
{
	float Open = 0.f;
	float High = 0.f;
	float Low = 0.f;
	float Close = 0.f;
};

class SStockChartWidget : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SStockChartWidget)
		: _UpColor(FLinearColor(0.85f, 0.2f, 0.2f))
		, _DownColor(FLinearColor(0.25f, 0.45f, 0.95f))
		, _ShortMAColor(FLinearColor(0.35f, 0.85f, 0.55f))
		, _LongMAColor(FLinearColor(0.95f, 0.55f, 0.2f))
		, _ShortMAPeriod(5)
		, _LongMAPeriod(20)
		, _MaxVisibleCandles(30)
		, _NumGridlines(6)
		, _GridlineColor(FLinearColor(1.f, 1.f, 1.f, 0.08f))
		, _bShowPriceLabels(true)
		, _PriceLabelColor(FLinearColor(0.85f, 0.85f, 0.85f))
		, _CurrentPriceLabelColor(FLinearColor(0.15f, 0.55f, 0.95f))
		, _GridlineMarginWidth(70.f)
	{}
		SLATE_ATTRIBUTE(FLinearColor, UpColor)
		SLATE_ATTRIBUTE(FLinearColor, DownColor)
		SLATE_ATTRIBUTE(FLinearColor, ShortMAColor)
		SLATE_ATTRIBUTE(FLinearColor, LongMAColor)
		SLATE_ATTRIBUTE(int32, ShortMAPeriod)
		SLATE_ATTRIBUTE(int32, LongMAPeriod)
		SLATE_ATTRIBUTE(int32, MaxVisibleCandles)
		SLATE_ATTRIBUTE(int32, NumGridlines)
		SLATE_ATTRIBUTE(FLinearColor, GridlineColor)
		// 이 셋을 추가해서, 별도의 PriceAxis 위젯 없이도 이 차트 하나만으로
		// 오른쪽에 가격 숫자가 같이 표시됩니다 (기준선 + 숫자를 한 위젯으로 통합).
		SLATE_ATTRIBUTE(bool, bShowPriceLabels)
		SLATE_ATTRIBUTE(FLinearColor, PriceLabelColor)
		SLATE_ATTRIBUTE(FLinearColor, CurrentPriceLabelColor)
		// 기준선을 화면 전체 폭이 아니라, 오른쪽 가격 숫자 옆의 이 폭(px)만큼만 짧게 그립니다.
		// 캔들과 안 겹치게 하려면 이 값을 조정하세요.
		SLATE_ATTRIBUTE(float, GridlineMarginWidth)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	void SetCandles(const TArray<FChartCandle>& InCandles);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;

private:
	TArray<FVector2D> BuildMovingAveragePoints(int32 Period, float SlotWidth, float MinPrice, float MaxPrice, float LocalHeight, int32 StartIndex) const;

	TArray<FChartCandle> Candles;
	TAttribute<FLinearColor> UpColor;
	TAttribute<FLinearColor> DownColor;
	TAttribute<FLinearColor> ShortMAColor;
	TAttribute<FLinearColor> LongMAColor;
	TAttribute<int32> ShortMAPeriod;
	TAttribute<int32> LongMAPeriod;
	TAttribute<int32> MaxVisibleCandles;
	TAttribute<int32> NumGridlines;
	TAttribute<FLinearColor> GridlineColor;
	TAttribute<bool> bShowPriceLabels;
	TAttribute<FLinearColor> PriceLabelColor;
	TAttribute<FLinearColor> CurrentPriceLabelColor;
	TAttribute<float> GridlineMarginWidth;
};
