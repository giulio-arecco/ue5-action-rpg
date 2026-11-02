#include "Items/Treasure.h"

#include "Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"


ATreasure::ATreasure() {
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATreasure::BeginPlay() {
	Super::BeginPlay();
	
}

void ATreasure::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter != nullptr) {
		if (ensureAlwaysMsgf(PickupSound != nullptr, TEXT("[%s] PickupSound is null in ATreasure::OnSphereBeginOverlap"), *GetName())) {
			PlayPickupSound();
		}
		
		Destroy();
	}
}

void ATreasure::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	AddActorWorldRotation(FRotator(0.0, RotationRate * DeltaTime, 0.0));
}

