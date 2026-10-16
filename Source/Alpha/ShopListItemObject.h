#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ShopListItemObject.generated.h"

UCLASS(BlueprintType)
class ALPHA_API UShopListItemObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Shop")
	FName ItemID;

	UPROPERTY(BlueprintReadOnly, Category = "Shop")
	FText DisplayName;

	UPROPERTY(BlueprintReadOnly, Category = "Shop")
	float Price = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Shop")
	float RequiredReputation = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Shop")
	bool bUnlocked = false;

	UPROPERTY(BlueprintReadOnly, Category = "Shop")
	bool bOwned = false;
};
