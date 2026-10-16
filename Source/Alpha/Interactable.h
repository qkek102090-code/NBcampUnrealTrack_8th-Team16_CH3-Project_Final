#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(BlueprintType)
class ALPHA_API UInteractable : public UInterface
{
	GENERATED_BODY()
};

// 모니터, 나중에 추가할 다른 상호작용 오브젝트들이 전부 이 인터페이스를 구현하면 됩니다.
// Interact 안의 실제 동작(카메라 전환 등)은 블루프린트에서 자유롭게 구현하세요.
class ALPHA_API IInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Interaction")
	void Interact(APawn* InteractingPawn);
};
