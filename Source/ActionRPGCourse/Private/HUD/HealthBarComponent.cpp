#include "HUD/HealthBarComponent.h"

UHealthBarComponent::UHealthBarComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthBarComponent::BeginPlay() {
	Super::BeginPlay();
}

void UHealthBarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

