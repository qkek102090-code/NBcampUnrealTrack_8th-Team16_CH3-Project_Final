#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DonationOrgListItemObject.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class ALPHA_API UDonationOrgListItemObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Donation")
	FName OrgID;

	UPROPERTY(BlueprintReadOnly, Category = "Donation")
	FText OrgName;

	UPROPERTY(BlueprintReadOnly, Category = "Donation")
	FText Description;

	UPROPERTY(BlueprintReadOnly, Category = "Donation")
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadOnly, Category = "Donation")
	float DonationAmount = 0.f;
};
