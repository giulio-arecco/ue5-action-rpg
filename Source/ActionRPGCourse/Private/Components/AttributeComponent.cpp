#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent():
	MaxHealth(100.0f),
	Health(MaxHealth)
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UAttributeComponent::BeginPlay() {
	Super::BeginPlay();
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

