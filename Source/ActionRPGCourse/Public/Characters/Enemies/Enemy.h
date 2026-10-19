#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UAttributeComponent;

UCLASS()
class ACTIONRPGCOURSE_API AEnemy : public ACharacter, public IHitInterface {
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, const FVector& ImpactNormal) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Animation|Montages")
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	UPROPERTY(EditAnywhere, Category = "Animation|Montages")
	TObjectPtr<UAnimMontage> DeathMontage;
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundBase> HitSound;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> HitParticles;
	
	// Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TObjectPtr<AActor> PatrolTarget;
	
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolTargets;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> Attributes;
	
		
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarComponent;
	
	UPROPERTY(VisibleInstanceOnly)
	TWeakObjectPtr<AActor> CombatTarget;
	
	UPROPERTY(EditAnywhere)
	float CombatRadius;
	
	UPROPERTY(EditAnywhere)
	float LifeSpanAfterDeath;
	
	UFUNCTION(BlueprintCallable)
	void FreezeDeathAnimationAndSetDestroyTimer();
	
	void PlayHitReactMontage(const FVector& ImpactPoint) const;
	
	void ShowHealthBar() const;
	
	void HideHealthBar() const;
	
	void PlayDeathMontage() const;
	
	double ComputeAngleToImpact(const FVector& ImpactPoint) const;
	
	/* Target must not be null */
	bool IsWithinCombatRadius(const AActor* Target) const;
};
