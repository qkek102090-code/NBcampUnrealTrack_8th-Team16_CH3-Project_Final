#include "AlphaGameMode.h"
#include "AlphaCharacter.h"
#include "StockMarketSubsystem.h"
#include "UObject/ConstructorHelpers.h"

AAlphaGameMode::AAlphaGameMode()
	: Super()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}

void AAlphaGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UStockMarketSubsystem* StockSubsystem = GI->GetSubsystem<UStockMarketSubsystem>())
		{
			StockSubsystem->LoadStocksFromDataTable(StockDataTable);
			StockSubsystem->LoadDonationOrgsFromDataTable(DonationOrgDataTable);

			// 게임을 처음 열었을 때 캔들차트가 밋밋하지 않게, 화면에 보이기 전에
			// 가격을 30틱 미리 돌려서 어느 정도 변동이 쌓인 상태로 시작합니다.
			StockSubsystem->PreSimulatePriceHistory(150);
		}
	}
}