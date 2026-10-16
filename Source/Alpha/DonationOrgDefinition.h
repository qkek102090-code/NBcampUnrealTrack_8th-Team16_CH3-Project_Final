#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DonationOrgDefinition.generated.h"

class UTexture2D;

USTRUCT(BlueprintType)
struct FDonationOrgRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Donation")
	FName OrgID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Donation")
	FText OrgName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Donation")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Donation")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Donation")
	float DonationAmount = 50000.f;
};
