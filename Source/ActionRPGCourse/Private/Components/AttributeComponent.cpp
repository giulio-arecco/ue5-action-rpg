#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent() {
	PrimaryComponentTick.bCanEverTick = false;
}


void UAttributeComponent::BeginPlay() {
	Super::BeginPlay();
	Health = FMath::Clamp(Health, 0.0f, MaxHealth);
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttributeComponent::ReceiveDamage(float DamageAmount) {
	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
}

