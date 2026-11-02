#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FHealthChanged, float /* New Health Percent */);
DECLARE_MULTICAST_DELEGATE_OneParam(FGoldChanged, int32 /* New Gold Amount */);
DECLARE_MULTICAST_DELEGATE_OneParam(FSoulsChanged, int32 /* New Souls Amount */);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONRPGCOURSE_API UAttributeComponent : public UActorComponent {
	GENERATED_BODY()

public:
	FHealthChanged OnHealthChanged;
	FGoldChanged OnGoldChanged;
	FSoulsChanged OnSoulsChanged;
	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ReceiveDamage(float DamageAmount);
	void AddGold(int32 GoldAmount);
	void AddSouls(int32 NumberOfSouls);
	
	FORCEINLINE bool IsAlive() const { return Health > 0.0f; }
	FORCEINLINE float GetHealthPercent() const { return Health / MaxHealth; }
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
	
	UPROPERTY(EditAnywhere, Category = "Attributes")
	int32 Gold = 0;
	
	UPROPERTY(EditAnywhere, Category = "Attributes")
	int32 Souls = 0;
};
