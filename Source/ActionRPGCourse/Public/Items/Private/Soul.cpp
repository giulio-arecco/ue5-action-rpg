#include "Items/Public/Soul.h"

ASoul::ASoul() {
	PrimaryActorTick.bCanEverTick = true;
}

void ASoul::BeginPlay() {
	Super::BeginPlay();
	
}

void ASoul::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ASoul::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	// TODO: Add souls to Player
	
	if (ensureAlwaysMsgf(PickupSound != nullptr, TEXT("[%s] PickupSound is null in ASoul::OnSphereBeginOverlap"), *GetName())) {
		PlayPickupSound();
	}
	
	if (ensureAlwaysMsgf(PickupEffect != nullptr, TEXT("[%s] PickupEffect is null in ASoul::OnSphereBeginOverlap"), *GetName())) {
		SpawnPickupEffect();
	}
	
	Destroy();
}

