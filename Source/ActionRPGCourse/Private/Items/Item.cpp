#include "Items/Item.h"
#include "DebugUtils.h"

AItem::AItem(): RunningTime(0.0f), MovementAmplitude(1.0f), MovementPeriod(2.5f) {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AItem::BeginPlay() {
	Super::BeginPlay();
}

void AItem::Tick(float DeltaTime) { 
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;
	
	const float DeltaZ = MovementAmplitude * FMath::Sin(RunningTime * 2 * UE_PI / MovementPeriod);
	
	FVector Location = GetActorLocation();
	Location.Z += DeltaZ;
	
	SetActorLocation(Location);

	constexpr float RotationRate = 45.0f;
	AddActorWorldRotation(FRotator(0.0f, RotationRate * DeltaTime, 0.0f));

	const FVector Forward = GetActorForwardVector();
	const FVector ScaledForward = Forward * 50.0f;

	const UWorld* World = GetWorld();
	FDebugUtils::DrawSphereSingleFrame(World, Location, 25.0f);
	FDebugUtils::DrawVectorSingleFrame(World, Location, Location + ScaledForward);
}
