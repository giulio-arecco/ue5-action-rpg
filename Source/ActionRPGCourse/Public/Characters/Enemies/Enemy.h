#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UPawnSensingComponent;
class AAIController;
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
	UPROPERTY(EditAnywhere)
	float LifeSpanAfterDeath;
	
	/*****************************
	 *         Components        *
	 ****************************/
	
	UPROPERTY()
	TObjectPtr<AAIController> AIController;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> AttributeComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPawnSensingComponent> PawnSensingComponent;
	
	/*****************************
	 *     Animation Montages    *
	 ****************************/ 
	
	UPROPERTY(EditAnywhere, Category = "Animation|Montages")
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	UPROPERTY(EditAnywhere, Category = "Animation|Montages")
	TObjectPtr<UAnimMontage> DeathMontage;
	
	/*****************************
	 *   Hit Related Variables   *
	 ****************************/ 
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundBase> HitSound;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> HitParticles;
	
	/*****************************
	 *    AI Related Variables   *
	 ****************************/ 
	
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolTargets;
	
	UPROPERTY(VisibleInstanceOnly, Category = "AI Navigation")
	TObjectPtr<AActor> MoveTarget;
	
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float TargetReachedMinWait;
	
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float TargetReachedMaxWait;
	
	FTimerHandle TargetReachedTimerHandle;
	
	/*********************************
	 *    Combat Related Variables   *
	 *********************************/ 
	
	UPROPERTY(VisibleInstanceOnly)
	TWeakObjectPtr<AActor> CombatTarget;
	
	UPROPERTY(EditAnywhere)
	float CombatRadius;
	
	/************************************
	 *    Animation Related Functions   *
	 ************************************/ 
	
	void PlayHitReactMontage(const FVector& ImpactPoint) const;
	double ComputeAngleToImpact(const FVector& ImpactPoint) const;
	
	void PlayDeathMontage() const;
	
	UFUNCTION(BlueprintCallable)
	void FreezeDeathAnimationAndSetDestroyTimer();
	
	/*****************************
	 *    AI Related Functions   *
	 ****************************/ 
	
	AActor* PickRandomPatrolTarget() const;
	void MoveToTarget(const AActor* Target, float AcceptanceRadius) const;
	
	UFUNCTION()
	void OnSeePawn();
	
	/*********************************
	 *    Combat Related Functions   *
	 *********************************/ 
	
	void ShowHealthBar() const;
	
	void HideHealthBar() const;
	
	/* Target must not be null */
	bool IsWithinCombatRadius(const AActor* Target) const;
};
