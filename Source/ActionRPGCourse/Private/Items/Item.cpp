#include "Items/Item.h"
#include "DebugUtils.h"

AItem::AItem() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AItem::BeginPlay() {
	Super::BeginPlay();
}

void AItem::Tick(float DeltaTime) { 
	Super::Tick(DeltaTime);

	const UWorld* World = GetWorld();

	constexpr float MovementRate = 50.0f;
	AddActorWorldOffset(FVector(MovementRate * DeltaTime, 0.0f, 0.0f));

	constexpr float RotationRate = 45.0f;
	AddActorWorldRotation(FRotator(0.0f, RotationRate * DeltaTime, 0.0f));

	const FVector Location = GetActorLocation();
	const FVector Forward = GetActorForwardVector();
	const FVector ScaledForward = Forward * 50.0f;

	FDebugUtils::DrawSphereSingleFrame(World, Location, 25.0f);
	FDebugUtils::DrawVectorSingleFrame(World, Location, Location + ScaledForward);
}
