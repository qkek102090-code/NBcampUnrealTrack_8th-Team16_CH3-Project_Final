#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Engine/DataTable.h"
#include "AlphaGameMode.generated.h"

UCLASS(minimalapi)
class AAlphaGameMode : public AGameModeBase
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category = "Stock")
	UDataTable* StockDataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Stock")
	UDataTable* DonationOrgDataTable;

	virtual void BeginPlay() override;

public:
	AAlphaGameMode();
};



