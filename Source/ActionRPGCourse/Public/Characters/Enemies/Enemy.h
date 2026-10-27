#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UPawnSensingComponent;
class AAIController;
class UHealthBarComponent;
class UAttributeComponent;

UCLASS()
class ACTIONRPGCOURSE_API AEnemy : public ABaseCharacter {
	GENERATED_BODY()

public:
	AEnemy();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const AActor* Hitter, const FVector& ImpactPoint, const FVector& ImpactNormal) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	
	UFUNCTION(BlueprintPure)
	FVector ComputeMotionWarpingTarget() const;
	
protected:
	virtual void PlayDeathMontage() const override;
	virtual void ReceiveDamage(float DamageAmount) const override;
	virtual void Die() override;
	virtual bool CanAttack() const override;
	virtual void Attack() override;
	virtual void HitReactMontageEnd() override;
	
private:
	UPROPERTY(VisibleInstanceOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
	
	UPROPERTY(VisibleInstanceOnly)
	EEnemyActionState ActionState = EEnemyActionState::EEAS_Unoccupied;
	
	UPROPERTY(EditAnywhere)
	float LifeSpanAfterDeath = 10.0f;
	
	UPROPERTY(EditAnywhere)
	double WarpTargetOffset = 50.0f;
	
	/*****************************
	 *         Components        *
	 ****************************/
	
	UPROPERTY()
	TObjectPtr<AAIController> AIController;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPawnSensingComponent> PawnSensingComponent;
	
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
	float TargetReachedAcceptanceRadius = 30.0f;
	
	FTimerHandle PatrolTimerHandle;
	
	/*********************************
	 *    Combat Related Variables   *
	 *********************************/ 
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName WeaponSocketName = "RightHandSocket";
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<AWeapon> WeaponClass;
	
	UPROPERTY(VisibleInstanceOnly, Category = "Combat")
	TWeakObjectPtr<AActor> CombatTarget;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChaseRadius = 2000.0f;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float CombatRadius = 150.0f;
	
	float AttackTimerMin = 0.75f;
	float AttackTimerMax = 1.2f;
	FTimerHandle AttackTimerHandle;
	
	/************************************
	 *    Animation Related Functions   *
	 ************************************/ 
	
	UFUNCTION(BlueprintCallable)
	void FreezeDeathAnimationAndSetDestroyTimer();
	
	/*****************************
	 *    AI Related Functions   *
	 ****************************/ 
	
	void InitPatrolling();
	void StartPatrolling();
	AActor* PickRandomPatrolTarget() const;
	void MoveToTarget(AActor* Target, float AcceptanceRadius);
	
	void TransitionToState(EEnemyState NewState);
	
	UFUNCTION()
	void OnSeePawn(APawn* SeenPawn);
	
	/*********************************
	 *    Combat Related Functions   *
	 *********************************/ 
	
	UFUNCTION(BlueprintCallable)
	void AttackMontageEnd();
	
	void AttackTimerEnd();
	
	void ShowHealthBar() const;
	void HideHealthBar() const;
	
	/* Target must not be null */
	bool IsInTargetRange(const AActor* Target, float Range) const;
};
