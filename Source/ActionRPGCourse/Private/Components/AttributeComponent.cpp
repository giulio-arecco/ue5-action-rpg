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
	if (!IsAlive()) return;
	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(GetHealthPercent());
}

void UAttributeComponent::AddGold(int32 GoldAmount) {
	Gold += GoldAmount;
	OnGoldChanged.Broadcast(Gold);
}

void UAttributeComponent::AddSouls(int32 NumberOfSouls) {
	Souls += NumberOfSouls;
	OnSoulsChanged.Broadcast(Souls);
}

