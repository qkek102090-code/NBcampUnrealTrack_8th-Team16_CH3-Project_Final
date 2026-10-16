#include "MovingCar.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"

AMovingCar::AMovingCar()
{
	PrimaryActorTick.bCanEverTick = true;

	CarRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CarRoot"));
	RootComponent = CarRoot;

	CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarMesh"));
	CarMesh->SetupAttachment(CarRoot);
}

void AMovingCar::BeginPlay()
{
	Super::BeginPlay();

	if (SplinePathActor)
	{
		CachedSpline = SplinePathActor->FindComponentByClass<USplineComponent>();

		if (!CachedSpline)
		{
			UE_LOG(LogTemp, Warning, TEXT("MovingCar: SplinePathActor에서 Spline Component를 찾지 못했습니다."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MovingCar: SplinePathActor가 지정되지 않았습니다."));
	}

	DistanceAlongSpline = StartDistanceOffset;
}

void AMovingCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CachedSpline)
	{
		return;
	}

	const float SplineLength = CachedSpline->GetSplineLength();
	if (SplineLength <= 0.f)
	{
		return;
	}

	DistanceAlongSpline += Speed * DeltaTime;

	if (bLoop)
	{
		DistanceAlongSpline = FMath::Fmod(DistanceAlongSpline, SplineLength);
		if (DistanceAlongSpline < 0.f)
		{
			DistanceAlongSpline += SplineLength;
		}
	}
	else
	{
		DistanceAlongSpline = FMath::Min(DistanceAlongSpline, SplineLength);
	}

	const FVector NewLocation = CachedSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
	const FRotator NewRotation = CachedSpline->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);

	SetActorLocationAndRotation(NewLocation, NewRotation);
}
