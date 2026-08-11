#include "Items/Item.h"
#include "DebugUtils.h"

AItem::AItem(): MovementAmplitude(10.0f), MovementPeriod(2.5f), StartLocation(0.0f), RunningTime(0.0f) {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	SetRootComponent(ItemMesh);
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
}
