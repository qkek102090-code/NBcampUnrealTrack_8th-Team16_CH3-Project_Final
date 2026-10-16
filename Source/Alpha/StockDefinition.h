#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StockDefinition.generated.h"

USTRUCT(BlueprintType)
struct FStockTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stock")
	FName Symbol;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stock")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stock")
	float StartPrice = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stock")
	FName Sector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stock")
	float VolatilityMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stock")
	float TradingActivity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stock")
	float GrowthBias = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stock")
	float RangeReversionStrength = 0.02f;
};
