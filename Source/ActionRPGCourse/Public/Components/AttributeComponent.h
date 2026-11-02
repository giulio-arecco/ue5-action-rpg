#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FHealthChanged, float /* New Health Percent */);
DECLARE_MULTICAST_DELEGATE_OneParam(FStaminaChanged, float /* New Stamina Percent */);
DECLARE_MULTICAST_DELEGATE_OneParam(FGoldChanged, int32 /* New Gold Amount */);
DECLARE_MULTICAST_DELEGATE_OneParam(FSoulsChanged, int32 /* New Souls Amount */);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONRPGCOURSE_API UAttributeComponent : public UActorComponent {
	GENERATED_BODY()

public:
	FHealthChanged OnHealthChanged;
	FStaminaChanged OnStaminaChanged;
	FGoldChanged OnGoldChanged;
	FSoulsChanged OnSoulsChanged;
	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ReceiveDamage(float DamageAmount);
	void ConsumeStamina(float StaminaAmount);
	void AddStamina(float StaminaAmount);
	void AddGold(int32 GoldAmount);
	void AddSouls(int32 NumberOfSouls);
	
	FORCEINLINE bool IsAlive() const { return Health > 0.0f; }
	FORCEINLINE float GetHealthPercent() const { return Health / MaxHealth; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE float GetStaminaPercent() const { return Stamina / MaxStamina; }
	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Attributes", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MaxHealth = 100.0f;
	
	// Current Health
	UPROPERTY(EditAnywhere, Category = "Attributes", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float Health = MaxHealth;
	
	UPROPERTY(EditAnywhere, Category = "Attributes", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MaxStamina = 100.0f;
	
	// Current Stamina
	UPROPERTY(EditAnywhere, Category = "Attributes", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float Stamina = MaxStamina;
	
	UPROPERTY(EditAnywhere, Category = "Attributes", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float StaminaRegenRate = 0.0f;
	
	UPROPERTY(EditAnywhere, Category = "Attributes", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float StaminaRegenPause = 0.5f;
	
	UPROPERTY(EditAnywhere, Category = "Attributes")
	int32 Gold = 0;
	
	UPROPERTY(EditAnywhere, Category = "Attributes")
	int32 Souls = 0;
	
	FTimerHandle StaminaRegenPauseTimerHandle;
};
