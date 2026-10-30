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

