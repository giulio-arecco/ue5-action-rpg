#include "Breakables/BreakableActor.h"

#include "GeometryCollection/GeometryCollectionComponent.h"

ABreakableActor::ABreakableActor():
	LifeSpanAfterBreak(15.0f)
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("BreakableActorGeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void ABreakableActor::BeginPlay() {
	Super::BeginPlay();
}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, const FVector& ImpactNormal) {
}

