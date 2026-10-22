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
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> AttributeComponent;
	
	UPROPERTY(VisibleInstanceOnly, Category = "Equipment")
	TObjectPtr<AWeapon> EquippedWeapon;
	
	/*****************************
	 *   Hit Related Variables   *
	 ****************************/ 
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundBase> HitSound;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> HitParticles;
	
	/************************************
	 *    Animation Related Variables   *
	 ************************************/
	
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
	void SetEquippedWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled) const;
	
	void PlayAttackMontage() const;
	double ComputeAngleToImpact(const FVector& ImpactPoint) const;
	
	virtual bool CanAttack() const;
	virtual void Attack();
	virtual void Die();
	virtual void PlayHitReactMontage(const FVector& ImpactPoint) const;
	virtual void PlayDeathMontage() const;
};
