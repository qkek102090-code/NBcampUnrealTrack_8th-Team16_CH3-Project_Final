#include "SStockChartWidget.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"
#include "Framework/Application/SlateApplication.h"
#include "Fonts/FontMeasure.h"

void SStockChartWidget::Construct(const FArguments& InArgs)
{
	UpColor = InArgs._UpColor;
	DownColor = InArgs._DownColor;
	ShortMAColor = InArgs._ShortMAColor;
	LongMAColor = InArgs._LongMAColor;
	ShortMAPeriod = InArgs._ShortMAPeriod;
	LongMAPeriod = InArgs._LongMAPeriod;
	MaxVisibleCandles = InArgs._MaxVisibleCandles;
	NumGridlines = InArgs._NumGridlines;
	GridlineColor = InArgs._GridlineColor;
	bShowPriceLabels = InArgs._bShowPriceLabels;
	PriceLabelColor = InArgs._PriceLabelColor;
	CurrentPriceLabelColor = InArgs._CurrentPriceLabelColor;
	GridlineMarginWidth = InArgs._GridlineMarginWidth;
}

void SStockChartWidget::SetCandles(const TArray<FChartCandle>& InCandles)
{
	Candles = InCandles;
}

FVector2D SStockChartWidget::ComputeDesiredSize(float) const
{
	return FVector2D(300.f, 150.f);
}

TArray<FVector2D> SStockChartWidget::BuildMovingAveragePoints(int32 Period, float SlotWidth, float MinPrice, float MaxPrice, float LocalHeight, int32 StartIndex) const
{
	TArray<FVector2D> Points;

	if (Period <= 0 || Candles.Num() < Period)
	{
		return Points;
	}

	const float PriceRange = MaxPrice - MinPrice;

	float RollingSum = 0.f;
	for (int32 Index = 0; Index < Candles.Num(); ++Index)
	{
		RollingSum += Candles[Index].Close;
		if (Index >= Period)
		{
			RollingSum -= Candles[Index - Period].Close;
		}

		if (Index >= Period - 1 && Index >= StartIndex)
		{
			const float Average = RollingSum / static_cast<float>(Period);
			const float Normalized = (Average - MinPrice) / PriceRange;
			const float SlotPosition = static_cast<float>(Index - StartIndex);
			const float CenterX = (SlotPosition + 0.5f) * SlotWidth;
			const float Y = (1.f - Normalized) * LocalHeight;
			Points.Add(FVector2D(CenterX, Y));
		}
	}

	return Points;
}

int32 SStockChartWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (Candles.Num() == 0)
	{
		return LayerId;
	}

	const int32 TargetVisibleCandles = FMath::Max(MaxVisibleCandles.Get(), 1);
	const int32 VisibleCount = FMath::Min(Candles.Num(), TargetVisibleCandles);
	const int32 StartIndex = Candles.Num() - VisibleCount;

	float MinPrice = Candles[StartIndex].Low;
	float MaxPrice = Candles[StartIndex].High;
	for (int32 Index = StartIndex; Index < Candles.Num(); ++Index)
	{
		MinPrice = FMath::Min(MinPrice, Candles[Index].Low);
		MaxPrice = FMath::Max(MaxPrice, Candles[Index].High);
	}

	if (FMath::IsNearlyEqual(MinPrice, MaxPrice))
	{
		const float Margin = FMath::Max(MaxPrice * 0.01f, 1.f);
		MinPrice -= Margin;
		MaxPrice += Margin;
	}

	const FVector2D LocalSize = AllottedGeometry.GetLocalSize();

	const float SlotWidth = LocalSize.X / static_cast<float>(TargetVisibleCandles);

	const float BodyWidth = FMath::Max(SlotWidth * 0.6f, 1.f);
	const float WickThickness = FMath::Max(SlotWidth * 0.12f, 1.f);
	const float MinBodyHeight = 1.5f;

	auto PriceToY = [&](float Price) -> float
	{
		const float Normalized = (Price - MinPrice) / (MaxPrice - MinPrice);
		return (1.f - Normalized) * LocalSize.Y;
	};

	int32 CurrentLayer = LayerId;

	// --- 가격 기준선 + 가격 숫자 라벨 그리기 (캔들보다 먼저 그려서 배경처럼 뒤에 깔리게) ---
	// 별도의 PriceAxis 위젯 없이, 이 차트 하나만으로 기준선과 오른쪽 가격 숫자를 같이 보여줍니다.
	const int32 GridCount = FMath::Max(NumGridlines.Get(), 2);

	// 현재가(마지막 캔들의 종가)와 가장 가까운 눈금을 찾아서 그 자리만 강조합니다.
	const float CurrentPriceForLabel = Candles.Last().Close;
	int32 ClosestGridIndex = 0;
	float ClosestGridDistance = TNumericLimits<float>::Max();

	TArray<float> GridPrices;
	GridPrices.Reserve(GridCount);
	for (int32 GridIndex = 0; GridIndex < GridCount; ++GridIndex)
	{
		const float T = static_cast<float>(GridIndex) / static_cast<float>(GridCount - 1);
		const float GridPrice = MaxPrice - T * (MaxPrice - MinPrice);
		GridPrices.Add(GridPrice);

		const float Distance = FMath::Abs(GridPrice - CurrentPriceForLabel);
		if (Distance < ClosestGridDistance)
		{
			ClosestGridDistance = Distance;
			ClosestGridIndex = GridIndex;
		}
	}

	const FSlateFontInfo PriceLabelFont = FCoreStyle::GetDefaultFontStyle("Regular", 11);
	const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	const bool bDrawLabels = bShowPriceLabels.Get();

	// 캔들이 있는 넓은 구간은 그대로 두고, 기준선은 오른쪽 가격 숫자 옆의 짧은 구간에만 그려서
	// 캔들 바디와 겹치지 않게 합니다.
	const float GridlineStartX = FMath::Max(LocalSize.X - GridlineMarginWidth.Get(), 0.f);

	for (int32 GridIndex = 0; GridIndex < GridCount; ++GridIndex)
	{
		const float GridPrice = GridPrices[GridIndex];
		const float Y = PriceToY(GridPrice);

		TArray<FVector2D> GridPoints;
		GridPoints.Add(FVector2D(GridlineStartX, Y));
		GridPoints.Add(FVector2D(LocalSize.X, Y));

		FSlateDrawElement::MakeLines(
			OutDrawElements,
			CurrentLayer,
			AllottedGeometry.ToPaintGeometry(),
			GridPoints,
			ESlateDrawEffect::None,
			GridlineColor.Get(),
			true,
			1.0f
		);

		if (!bDrawLabels)
		{
			continue;
		}

		const bool bIsClosest = (GridIndex == ClosestGridIndex);
		const FString LabelString = FText::AsNumber(FMath::RoundToInt(GridPrice)).ToString();
		const FVector2D TextSize = FontMeasure->Measure(LabelString, PriceLabelFont);
		const float LabelX = LocalSize.X - TextSize.X - 6.f;
		const float LabelY = Y - TextSize.Y * 0.5f;

		if (bIsClosest)
		{
			const FSlateBrush* WhiteBrush = FCoreStyle::Get().GetBrush(TEXT("WhiteBrush"));
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				CurrentLayer + 1,
				AllottedGeometry.ToPaintGeometry(
					FVector2D(TextSize.X + 10.f, TextSize.Y + 4.f),
					FSlateLayoutTransform(FVector2D(LabelX - 5.f, LabelY - 2.f))
				),
				WhiteBrush,
				ESlateDrawEffect::None,
				CurrentPriceLabelColor.Get()
			);
		}

		FSlateDrawElement::MakeText(
			OutDrawElements,
			CurrentLayer + 2,
			AllottedGeometry.ToPaintGeometry(TextSize, FSlateLayoutTransform(FVector2D(LabelX, LabelY))),
			LabelString,
			PriceLabelFont,
			ESlateDrawEffect::None,
			bIsClosest ? FLinearColor::White : PriceLabelColor.Get()
		);
	}

	CurrentLayer += 3;

	// --- 캔들 그리기 (StartIndex ~ 끝까지만, 즉 화면에 보이는 구간만) ---
	for (int32 Index = StartIndex; Index < Candles.Num(); ++Index)
	{
		const FChartCandle& Candle = Candles[Index];
		const float SlotPosition = static_cast<float>(Index - StartIndex);
		const float CenterX = (SlotPosition + 0.5f) * SlotWidth;
		const bool bIsUp = Candle.Close >= Candle.Open;
		const FLinearColor Color = bIsUp ? UpColor.Get() : DownColor.Get();

		TArray<FVector2D> WickPoints;
		WickPoints.Add(FVector2D(CenterX, PriceToY(Candle.High)));
		WickPoints.Add(FVector2D(CenterX, PriceToY(Candle.Low)));

		FSlateDrawElement::MakeLines(
			OutDrawElements,
			CurrentLayer,
			AllottedGeometry.ToPaintGeometry(),
			WickPoints,
			ESlateDrawEffect::None,
			Color,
			true,
			WickThickness
		);

		float BodyTop = PriceToY(FMath::Max(Candle.Open, Candle.Close));
		float BodyBottom = PriceToY(FMath::Min(Candle.Open, Candle.Close));

		if (BodyBottom - BodyTop < MinBodyHeight)
		{
			BodyTop = BodyBottom - MinBodyHeight;
		}

		TArray<FVector2D> BodyPoints;
		BodyPoints.Add(FVector2D(CenterX, BodyTop));
		BodyPoints.Add(FVector2D(CenterX, BodyBottom));

		FSlateDrawElement::MakeLines(
			OutDrawElements,
			CurrentLayer + 1,
			AllottedGeometry.ToPaintGeometry(),
			BodyPoints,
			ESlateDrawEffect::None,
			Color,
			true,
			BodyWidth
		);
	}

	CurrentLayer += 2;

	// --- 이동평균선 그리기 (캔들 위에 겹쳐서, 더 위 레이어에) ---
	const float MALineThickness = 1.5f;

	const TArray<FVector2D> ShortMAPoints = BuildMovingAveragePoints(ShortMAPeriod.Get(), SlotWidth, MinPrice, MaxPrice, LocalSize.Y, StartIndex);
	if (ShortMAPoints.Num() >= 2)
	{
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			CurrentLayer,
			AllottedGeometry.ToPaintGeometry(),
			ShortMAPoints,
			ESlateDrawEffect::None,
			ShortMAColor.Get(),
			true,
			MALineThickness
		);
	}

	const TArray<FVector2D> LongMAPoints = BuildMovingAveragePoints(LongMAPeriod.Get(), SlotWidth, MinPrice, MaxPrice, LocalSize.Y, StartIndex);
	if (LongMAPoints.Num() >= 2)
	{
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			CurrentLayer + 1,
			AllottedGeometry.ToPaintGeometry(),
			LongMAPoints,
			ESlateDrawEffect::None,
			LongMAColor.Get(),
			true,
			MALineThickness
		);
	}

	return CurrentLayer + 2;
}
