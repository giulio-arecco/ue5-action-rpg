#include "Items/Weapons/AWeapon.h"

AAWeapon::AAWeapon() {
	PrimaryActorTick.bCanEverTick = true;
}

void AAWeapon::BeginPlay() {
	Super::BeginPlay();
	
}

void AAWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AAWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AAWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

