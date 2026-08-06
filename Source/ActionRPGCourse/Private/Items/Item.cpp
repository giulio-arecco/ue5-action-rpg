#include "Items/Item.h"
#include "DebugUtils.h"

AItem::AItem() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AItem::BeginPlay() {
	Super::BeginPlay();

	const UWorld* World = GetWorld();
	const FVector Location = GetActorLocation();
	const FVector Forward = GetActorForwardVector();
	const FVector ScaledForward = 50.0f * Forward; 
	
	FDebugUtils::DrawSphere(World, Location, 25.0f);
	FDebugUtils::DrawVector(World, Location, Location + ScaledForward);
}

void AItem::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}
