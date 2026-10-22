#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
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
	UPROPERTY(VisibleInstanceOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
	
	UPROPERTY(EditAnywhere)
	float LifeSpanAfterDeath = 10.0f;
	
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
	TWeakObjectPtr<AActor> MoveTarget;
	
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WalkSpeed = 125.0f;
	
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float RunSpeed = 300.0f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float TargetReachedMinWait = 5.0f;
	
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float TargetReachedMaxWait = 10.0f;
	
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float TargetReachedAcceptanceRadius = 50.0f;
	
	FTimerHandle PatrolTimerHandle;
	
	/*********************************
	 *    Combat Related Variables   *
	 *********************************/ 
	
	UPROPERTY(VisibleInstanceOnly, Category = "Combat")
	TWeakObjectPtr<AActor> CombatTarget;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float CombatRadius = 2000.0f;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRadius = 200.0f;
	
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
	void MoveToTarget(AActor* Target, float AcceptanceRadius);
	
	void TransitionToState(EEnemyState NewState);
	
	UFUNCTION()
	void OnSeePawn(APawn* SeenPawn);
	
	/*********************************
	 *    Combat Related Functions   *
	 *********************************/ 
	
	void ShowHealthBar() const;
	void HideHealthBar() const;
	
	/* Target must not be null */
	bool IsInTargetRange(const AActor* Target, float Range) const;
};
