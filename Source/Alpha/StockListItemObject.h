#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StockListItemObject.generated.h"

UCLASS(BlueprintType)
class ALPHA_API UStockListItemObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Stock")
	FName Symbol;

	UPROPERTY(BlueprintReadOnly, Category = "Stock")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "Stock")
	float CurrentPrice = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Stock")
	float StartPrice = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Stock")
	float PercentChange = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Stock")
	bool bInWatchlist = false;

	UPROPERTY(BlueprintReadOnly, Category = "Stock")
	int32 HoldingQuantity = 0;

	// 평균 구매가 및 그 대비 등락 (내 투자 패널에서 씁니다)
	UPROPERTY(BlueprintReadOnly, Category = "Stock")
	float AverageBuyPrice = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Stock")
	float GainAmount = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Stock")
	float GainPercent = 0.f;
};
