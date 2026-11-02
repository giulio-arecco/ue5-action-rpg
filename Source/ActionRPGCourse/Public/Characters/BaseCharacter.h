#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class UAttributeComponent;
class AWeapon;

UCLASS()
class ACTIONRPGCOURSE_API ABaseCharacter : public ACharacter, public IHitInterface {
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	virtual void GetHit_Implementation(const AActor* Hitter, const FVector& ImpactPoint, const FVector& ImpactNormal) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> AttributeComponent;
	
	UPROPERTY(VisibleInstanceOnly, Category = "Combat|Equipment")
	TObjectPtr<AWeapon> EquippedWeapon;
	
	/************************************
	 *    Animation Related Variables   *
	 ************************************/
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float LifeSpanAfterDeath = 10.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Montages")
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY(EditAnywhere, Category = "Animation|Montages")
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	UPROPERTY(EditAnywhere, Category = "Animation|Montages")
	TObjectPtr<UAnimMontage> DeathMontage;
	
	/******************
	 *    Functions   *
	 ******************/
	
	UFUNCTION(BlueprintCallable)
	virtual void HitReactMontageEnd();
	
	virtual void PlayAttackMontage() const;
	virtual void PlayHitReactMontage(const FVector& HitterPosition) const;
	virtual bool IsAlive() const;
	virtual void ReceiveDamage(float DamageAmount) const;
	virtual bool CanAttack() const;
	virtual void Attack();
	virtual void Die();
	virtual void PlayDeathMontage() const;
	
	UFUNCTION(BlueprintCallable)
	void SetEquippedWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled) const;
	
	UFUNCTION(BlueprintCallable)
	void PauseAnimationsAndSetDestroyTimer();
	
	void PauseAnimations() const;
	double ComputeAngleTo(const FVector& InPoint) const;
	void PlayHitSound(const FVector& Location) const;
	void SpawnHitParticles(const FVector& Location, const FVector& Normal) const;
	
private:
	/*****************************
	 *   Hit Related Variables   *
	 ****************************/ 
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<USoundBase> HitSound;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UParticleSystem> HitParticles;
	
	/******************
	 *    Functions   *
	 ******************/
	
	void PlayRandomMontageSection(UAnimMontage* Montage) const;
};
