#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}


void UAttributeComponent::BeginPlay() {
	Super::BeginPlay();
	Health = FMath::Clamp(Health, 0.0f, MaxHealth);
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (StaminaRegenRate > 0.0f && Stamina < MaxStamina) {
		const UWorld* World = GetWorld();
		
		if (!ensureMsgf(World != nullptr, TEXT("[%s] World is null in UAttributeComponent::TickComponent"), *GetName())) return;
		if (World->GetTimerManager().IsTimerActive(StaminaRegenPauseTimerHandle)) return;
		
		AddStamina(StaminaRegenRate * DeltaTime);
	}
}

void UAttributeComponent::ReceiveDamage(float DamageAmount) {
	if (!IsAlive()) return;
	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(GetHealthPercent());
}

void UAttributeComponent::ConsumeStamina(float StaminaAmount) {
	const UWorld* World = GetWorld();
	if (!ensureAlwaysMsgf(World != nullptr, TEXT("[%s] World is null in UAttributeComponent::ConsumeStamina"), *GetName())) return;

	Stamina = FMath::Clamp(Stamina - StaminaAmount, 0.0f, MaxStamina);
	OnStaminaChanged.Broadcast(GetStaminaPercent());

	World->GetTimerManager().SetTimer(StaminaRegenPauseTimerHandle, StaminaRegenPause, false);
}

void UAttributeComponent::AddStamina(float StaminaAmount) {
	Stamina = FMath::Clamp(Stamina + StaminaAmount, 0.0f, MaxStamina);
	OnStaminaChanged.Broadcast(GetStaminaPercent());
}

void UAttributeComponent::AddGold(int32 GoldAmount) {
	Gold += GoldAmount;
	OnGoldChanged.Broadcast(Gold);
}

void UAttributeComponent::AddSouls(int32 NumberOfSouls) {
	Souls += NumberOfSouls;
	OnSoulsChanged.Broadcast(Souls);
}

