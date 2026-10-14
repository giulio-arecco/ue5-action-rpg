#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONRPGCOURSE_API UAttributeComponent : public UActorComponent {
	GENERATED_BODY()

public:
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ReceiveDamage(float DamageAmount);
	
	FORCEINLINE bool IsAlive() const { return Health > 0.0f; }
	FORCEINLINE float GetHealthPercent() const { return Health / MaxHealth; }

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Actor Attributes", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MaxHealth;
	
	// Current Health
	UPROPERTY(EditAnywhere, Category = "Actor Attributes", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float Health;
};
