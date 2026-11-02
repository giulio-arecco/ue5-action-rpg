#include "Items/Public/Soul.h"

#include "Characters/PlayerCharacter.h"
#include "Components/AttributeComponent.h"

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
	const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter == nullptr) return;
	
	UAttributeComponent* PlayerAttributes = PlayerCharacter->GetAttributeComponent();
	if (!ensureAlwaysMsgf(PlayerAttributes != nullptr, TEXT("[%s] PlayerAttributes is null in ASoul::OnSphereBeginOverlap"), *GetName())) return;
	
	if (ensureAlwaysMsgf(NumberOfSouls != 0, TEXT("[%s] NumberOfSouls is 0 in ASoul::OnSphereBeginOverlap"), *GetName())) {
		PlayerAttributes->AddSouls(NumberOfSouls);
	}
	
	if (ensureAlwaysMsgf(PickupSound != nullptr, TEXT("[%s] PickupSound is null in ASoul::OnSphereBeginOverlap"), *GetName())) {
		PlayPickupSound();
	}
	
	if (ensureAlwaysMsgf(PickupEffect != nullptr, TEXT("[%s] PickupEffect is null in ASoul::OnSphereBeginOverlap"), *GetName())) {
		SpawnPickupEffect();
	}
	
	Destroy();
}

