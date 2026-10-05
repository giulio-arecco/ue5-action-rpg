#include "Breakables/BreakableActor.h"

#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"
#include "Kismet/GameplayStatics.h"

ABreakableActor::ABreakableActor():
	LifeSpanAfterBreak(20.0f)
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("BreakableActorGeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GeometryCollection->SetNotifyBreaks(true);
}

void ABreakableActor::BeginPlay() {
	Super::BeginPlay();
	GeometryCollection->OnChaosBreakEvent.AddUniqueDynamic(this, &ABreakableActor::OnBreak);
}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, const FVector& ImpactNormal) {
	UGameplayStatics::PlaySoundAtLocation(this, BreakSound, ImpactPoint);
}

void ABreakableActor::OnBreak(const FChaosBreakEvent& BreakEvent) {
	// Disable break notifies to avoid responding to subsequent breaks into sub pieces
	GeometryCollection->SetNotifyBreaks(false);
	GeometryCollection->OnChaosBreakEvent.RemoveDynamic(this, &ABreakableActor::OnBreak);
	
	SetLifeSpan(LifeSpanAfterBreak);
	
	UWorld* World = GetWorld();
	if (World != nullptr && TreasureClass != nullptr) {
		FVector Location = GetActorLocation();
		Location.Z += 50.0;
		World->SpawnActor<ATreasure>(TreasureClass, Location, FRotator::ZeroRotator);
	}
}

