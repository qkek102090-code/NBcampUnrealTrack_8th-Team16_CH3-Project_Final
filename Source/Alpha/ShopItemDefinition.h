#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ShopItemDefinition.generated.h"

UENUM(BlueprintType)
enum class EShopItemCategory : uint8
{
	RealEstate UMETA(DisplayName = "부동산"),
	Vehicle UMETA(DisplayName = "차량"),
	Costume UMETA(DisplayName = "코스튬"),
	Other UMETA(DisplayName = "기타")
};

USTRUCT(BlueprintType)
struct FShopItemRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	EShopItemCategory Category = EShopItemCategory::Other;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	float Price = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	float RequiredReputation = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	float ReputationReward = 0.f;
};
