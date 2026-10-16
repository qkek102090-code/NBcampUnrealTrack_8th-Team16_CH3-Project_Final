#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "StockMarketSubsystem.h"
#include "StockChartWidget.generated.h"

class SStockChartWidget;

UCLASS()
class ALPHA_API UStockChartWidget : public UWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	FLinearColor UpColor = FLinearColor(0.85f, 0.2f, 0.2f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	FLinearColor DownColor = FLinearColor(0.25f, 0.45f, 0.95f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	FLinearColor ShortMAColor = FLinearColor(0.35f, 0.85f, 0.55f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	FLinearColor LongMAColor = FLinearColor(0.95f, 0.55f, 0.2f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	int32 ShortMAPeriod = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	int32 LongMAPeriod = 20;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	int32 MaxVisibleCandles = 30;

	// PriceAxis의 Num Gridlines와 같은 값으로 맞춰두면 옆의 가격 눈금과 나란히 맞습니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	int32 NumGridlines = 6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	FLinearColor GridlineColor = FLinearColor(1.f, 1.f, 1.f, 0.08f);

	// 이 셋을 켜두면 별도의 PriceAxis 위젯 없이 이 차트 하나만으로 오른쪽에
	// 가격 숫자가 같이 표시됩니다. (기준선 + 숫자를 이 위젯 하나로 통합)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	bool bShowPriceLabels = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	FLinearColor PriceLabelColor = FLinearColor(0.85f, 0.85f, 0.85f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	FLinearColor CurrentPriceLabelColor = FLinearColor(0.15f, 0.55f, 0.95f);

	// 기준선을 화면 전체 폭이 아니라, 오른쪽 가격 숫자 옆 이 폭(px)만큼만 짧게 그립니다.
	// 캔들과 겹치지 않게 하려면 이 값을 조정하세요 (기본 70).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chart")
	float GridlineMarginWidth = 70.f;

	UFUNCTION(BlueprintCallable, Category = "Chart")
	void SetCandles(const TArray<FCandleData>& InCandles);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

private:
	TSharedPtr<SStockChartWidget> MyChart;
};
