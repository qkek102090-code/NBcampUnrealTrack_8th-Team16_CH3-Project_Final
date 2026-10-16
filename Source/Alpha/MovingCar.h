#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingCar.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class USplineComponent;

// 지정된 스플라인 경로를 따라 일정 속도로 움직이는 배경 차량.
// 플레이어가 조종하는 차가 아니라, 도시에 생동감을 주는 장식용 자동 주행 차량입니다.
UCLASS()
class ALPHA_API AMovingCar : public AActor
{
	GENERATED_BODY()

public:
	AMovingCar();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	// 실제로 스플라인 위치/방향을 받는 건 이 루트입니다. 이동 로직은 여기에만 관여합니다.
	UPROPERTY(VisibleAnywhere, Category = "Car")
	TObjectPtr<USceneComponent> CarRoot;

	// 차 메시는 CarRoot의 자식으로 붙어있어서, 이 컴포넌트의 회전(Relative Rotation)을
	// 디자이너에서 따로 조정하면 "메시가 원래 다른 방향을 보고 만들어진 문제"를 보정할 수 있습니다.
	// 예: 차가 진행 방향 기준 오른쪽을 보고 달리면, 여기 Yaw를 -90(또는 90)으로 맞춰보세요.
	UPROPERTY(VisibleAnywhere, Category = "Car")
	TObjectPtr<UStaticMeshComponent> CarMesh;

	// 이 차가 따라갈 길. Spline Component가 붙어있는 액터(레벨에 그려둔 "도로" 액터)를 지정하세요.
	UPROPERTY(EditAnywhere, Category = "Movement")
	TObjectPtr<AActor> SplinePathActor;

	// 이동 속도 (cm/초)
	UPROPERTY(EditAnywhere, Category = "Movement")
	float Speed = 300.f;

	// 끝까지 가면 다시 처음부터 반복할지
	UPROPERTY(EditAnywhere, Category = "Movement")
	bool bLoop = true;

	// 같은 스플라인 위에 차를 여러 대 놓을 때, 서로 겹치지 않게 시작 위치를 어긋나게 주는 값(cm)
	UPROPERTY(EditAnywhere, Category = "Movement")
	float StartDistanceOffset = 0.f;

private:
	UPROPERTY()
	TObjectPtr<USplineComponent> CachedSpline;

	float DistanceAlongSpline = 0.f;
};
