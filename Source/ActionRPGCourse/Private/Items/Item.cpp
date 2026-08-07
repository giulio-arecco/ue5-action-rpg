#include "Items/Item.h"
#include "DebugUtils.h"

AItem::AItem(): MovementAmplitude(10.0f), MovementPeriod(2.5f), RunningTime(0.0f) {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AItem::BeginPlay() {
	Super::BeginPlay();

	StartLocation = GetActorLocation();
}

float AItem::TransformedSin() const {
	return MovementAmplitude * FMath::Sin(RunningTime * 2 * UE_PI / MovementPeriod);
}

float AItem::TransformedCos() const {
	return MovementAmplitude * FMath::Cos(RunningTime * 2 * UE_PI / MovementPeriod);
}

void AItem::Tick(float DeltaTime) { 
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;
	
	const float DeltaZ = TransformedSin();
	
	FVector NewLocation = StartLocation;
	NewLocation.Z += DeltaZ;
	
	SetActorLocation(NewLocation);

	constexpr float RotationRate = 45.0f;
	AddActorWorldRotation(FRotator(0.0f, RotationRate * DeltaTime, 0.0f));

	const FVector Forward = GetActorForwardVector();
	const FVector ScaledForward = Forward * 50.0f;

	const UWorld* World = GetWorld();
	FDebugUtils::DrawSphereSingleFrame(World, NewLocation, 25.0f);
	FDebugUtils::DrawVectorSingleFrame(World, NewLocation, NewLocation + ScaledForward);
}
