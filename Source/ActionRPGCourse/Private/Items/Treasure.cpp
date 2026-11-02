#include "Items/Treasure.h"

#include "Characters/PlayerCharacter.h"
#include "Components/AttributeComponent.h"


ATreasure::ATreasure() {
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ATreasure::BeginPlay() {
	Super::BeginPlay();
	
}

void ATreasure::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter == nullptr) return;
	
	UAttributeComponent* PlayerAttributes = PlayerCharacter->GetAttributeComponent();
	if (!ensureAlwaysMsgf(PlayerAttributes != nullptr, TEXT("[%s] PlayerAttributes is null in ATreasure::OnSphereBeginOverlap"), *GetName())) return;
	
	if (ensureAlwaysMsgf(GoldAmount != 0, TEXT("[%s] GoldAmount is 0 in ATreasure::OnSphereBeginOverlap"), *GetName())) {
		PlayerAttributes->AddGold(GoldAmount);
	}
	
	if (ensureAlwaysMsgf(PickupSound != nullptr, TEXT("[%s] PickupSound is null in ATreasure::OnSphereBeginOverlap"), *GetName())) {
		PlayPickupSound();
	}
	
	Destroy();
}

void ATreasure::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	AddActorWorldRotation(FRotator(0.0, RotationRate * DeltaTime, 0.0));
}

