#include "Characters/Enemies/Enemy.h"

#include "AIController.h"
#include "Characters/CharacterTypes.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

AEnemy::AEnemy() {
	PrimaryActorTick.bCanEverTick = true;
	
	HealthBarComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("EnemyHealthBarWidget"));
	HealthBarComponent->SetupAttachment(GetRootComponent());
	
	// TODO: Update PawnSensing to AI Perception
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("EnemyPawnSensingComponent"));
	PawnSensingComponent->SetPeripheralVisionAngle(60.0f);
	PawnSensingComponent->SightRadius = 1500.0f;
	
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
}

void AEnemy::BeginPlay() {
	Super::BeginPlay();
	
	checkf(AttributeComponent != nullptr, TEXT("AttributeComponent is null in AEnemy::BeginPlay"));
	checkf(HealthBarComponent != nullptr, TEXT("HealthBarComponent is null in AEnemy::BeginPlay"));
	checkf(PawnSensingComponent != nullptr, TEXT("PawnSensingComponent is null in AEnemy::BeginPlay"));

	Tags.Add(FName("Enemy"));
	
	if (WeaponClass != nullptr) {
		AWeapon* Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
		Weapon->Equip(GetMesh(), WeaponSocketName, this, this);
		EquippedWeapon = Weapon;
	}
	
	HealthBarComponent->SetHealthPercent(AttributeComponent->GetHealthPercent());
	HideHealthBar();
	
	PawnSensingComponent->OnSeePawn.AddUniqueDynamic(this, &AEnemy::OnSeePawn);
	
	InitPatrolling();
}

void AEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	const AActor* Target = CombatTarget.Get();
	
	if (ActionState != EEnemyActionState::EEAS_StateLocked) {
		switch (EnemyState) {
		case EEnemyState::EES_Patrolling:
			if (Target == nullptr) break;
			if (IsInTargetRange(Target, CombatRadius)) TransitionToState(EEnemyState::EES_InCombat);
			else if (IsInTargetRange(Target, ChaseRadius)) TransitionToState(EEnemyState::EES_Chasing);
			break;
		case EEnemyState::EES_Chasing:
			if (Target == nullptr) TransitionToState(EEnemyState::EES_Patrolling);
			else if (IsInTargetRange(Target, CombatRadius)) TransitionToState(EEnemyState::EES_InCombat);
			else if (!IsInTargetRange(Target, ChaseRadius)) TransitionToState(EEnemyState::EES_Patrolling);
			break;
		case EEnemyState::EES_InCombat:
			if (Target == nullptr) TransitionToState(EEnemyState::EES_Patrolling);
			else if (!IsInTargetRange(Target, ChaseRadius)) TransitionToState(EEnemyState::EES_Patrolling);
			else if (!IsInTargetRange(Target, CombatRadius)) TransitionToState(EEnemyState::EES_Chasing);
			break;
		case EEnemyState::EES_Dead:
			// The conditions to transition to the Dead state are checked in TakeDamage
			break;
		}
	}
}

bool AEnemy::IsInTargetRange(const AActor* Target, float Range) const {
	check(Target);
	
	const double DistSq = FVector::DistSquared(GetActorLocation(), Target->GetActorLocation());
	return DistSq < FMath::Square(Range);
}

void AEnemy::TransitionToState(EEnemyState NewState) {
	if (EnemyState == NewState) return;
	
	checkf(GetCharacterMovement() != nullptr, TEXT("[%s] CharacterMovement is null"), *GetName());
	checkf(AIController != nullptr, TEXT("[%s] AIController is null"), *GetName());
	
	switch (NewState) {
	case EEnemyState::EES_Patrolling:
		UE_LOG(LogTemp, Warning, TEXT("Patrolling"));
		
		EnemyState = EEnemyState::EES_Patrolling;
		CombatTarget = nullptr;
		HideHealthBar();
		if (AttackTimerHandle.IsValid()) GetWorldTimerManager().ClearTimer(AttackTimerHandle);
		StartPatrolling();
		
		break;
	case EEnemyState::EES_Chasing:
		UE_LOG(LogTemp, Warning, TEXT("Chasing"));
		
		EnemyState = EEnemyState::EES_Chasing;
			
		ShowHealthBar();
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		if (PatrolTimerHandle.IsValid()) GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
		if (AttackTimerHandle.IsValid()) GetWorldTimerManager().ClearTimer(AttackTimerHandle);
			
		MoveToTarget(CombatTarget.Get(), TargetReachedAcceptanceRadius);
		
		break;
	case EEnemyState::EES_InCombat:
		UE_LOG(LogTemp, Warning, TEXT("In Combat"));
		
		EnemyState = EEnemyState::EES_InCombat;
			
		ShowHealthBar();
		if (PatrolTimerHandle.IsValid()) GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
		 
		Attack();
		
		break;
	case EEnemyState::EES_Dead:
		Die();
		break;
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, const FVector& ImpactNormal) {
	Super::GetHit_Implementation(ImpactPoint, ImpactNormal);
	
	if (EnemyState != EEnemyState::EES_Dead && IsAlive()) {
		checkf(AIController != nullptr, TEXT("[%s] AIController is null"), *GetName());
		
		ActionState = EEnemyActionState::EEAS_StateLocked;
		if (AttackTimerHandle.IsValid()) GetWorldTimerManager().ClearTimer(AttackTimerHandle);
		AIController->StopMovement();
		
		PlayHitReactMontage(ImpactPoint);
	}
}

void AEnemy::HitReactMontageEnd() {
	ActionState = EEnemyActionState::EEAS_Unoccupied;
	
	if (!CanAttack()) return;
	Attack();
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	if (EnemyState == EEnemyState::EES_Dead) return 0.0f;
	
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (DamageCauser != nullptr) {
		// If a EventInstigator used the DamageCauser as a weapon, set the target to
		// the EventInstigator pawn, otherwise set it to the DamageCauser (actor)
		CombatTarget = (EventInstigator && EventInstigator->GetPawn()) ? EventInstigator->GetPawn() : DamageCauser;
	}
	
	ReceiveDamage(DamageAmount);
	if (!IsAlive()) TransitionToState(EEnemyState::EES_Dead);
	
	return DamageAmount;
}

void AEnemy::ReceiveDamage(float DamageAmount) const {
	checkf(AttributeComponent != nullptr, TEXT("[%s] AttributeComponent is null"), *GetName());
	checkf(HealthBarComponent != nullptr, TEXT("[%s] HealthBarComponent is null"), *GetName());
	
	Super::ReceiveDamage(DamageAmount);
	
	if (IsAlive()) {
		HealthBarComponent->SetHealthPercent(AttributeComponent->GetHealthPercent());
		ShowHealthBar();
	}
}

void AEnemy::Die() {
	Super::Die();
	
	UE_LOG(LogTemp, Warning, TEXT("Dead"));
		
	EnemyState = EEnemyState::EES_Dead;
		
	DetachFromControllerPendingDestroy();
	
	if (UCharacterMovementComponent* CharMove = GetCharacterMovement(); CharMove != nullptr) {
		CharMove->DisableMovement();
		CharMove->StopMovementImmediately();
	}
	
	if (PawnSensingComponent != nullptr) {
		PawnSensingComponent->SetSensingUpdatesEnabled(false);
	}
	
	if (UCapsuleComponent* CapsuleComp = GetCapsuleComponent(); CapsuleComp != nullptr) {
		CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	if (USkeletalMeshComponent* Skm = GetMesh(); Skm != nullptr) {
		Skm->SetGenerateOverlapEvents(false);
		Skm->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	if (EquippedWeapon != nullptr) {
		SetEquippedWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	HideHealthBar();
	if (PatrolTimerHandle.IsValid()) GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
	if (AttackTimerHandle.IsValid()) GetWorldTimerManager().ClearTimer(AttackTimerHandle);
	
	PlayDeathMontage();
	
	SetActorTickEnabled(false);
}

void AEnemy::PlayDeathMontage() const {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr && DeathMontage != nullptr) {
		AnimInstance->Montage_Play(DeathMontage);

		if (const int32 NumSections = DeathMontage->GetNumSections(); NumSections > 0) {
			const int32 RandomIndex = FMath::RandRange(0, NumSections - 1);
			const FName SectionName = DeathMontage->GetSectionName(RandomIndex);
			AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
		}
		
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		HideHealthBar();
	}
}

void AEnemy::FreezeDeathAnimationAndSetDestroyTimer() {
	if (USkeletalMeshComponent* Skm = GetMesh(); Skm != nullptr) {
		Skm->bPauseAnims = true;
		Skm->SetComponentTickEnabled(false);
	}
	
	// We use this approach instead of SetLifeSpan() because this way the timer does not get updated in the Actor's tick, 
	// meaning it can be disabled. We use K2_DestroyActor instead of Destroy to match the delegate signature.
	FTimerHandle DeathTimerHandle;
	GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &AActor::K2_DestroyActor, LifeSpanAfterDeath, false);
}

void AEnemy::InitPatrolling() {
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	
	AIController = CastChecked<AAIController>(GetController());
	
	if (PatrolTargets.Num() <= 0) return;
	
	UPathFollowingComponent* PathFollowingComp = AIController->GetPathFollowingComponent();
	checkf(PathFollowingComp != nullptr, TEXT("[%s] PathFollowingComponent is null"), *GetName());

	PathFollowingComp->OnRequestFinished.AddWeakLambda(this, [this] (FAIRequestID RequestID, const FPathFollowingResult& Result) {
		// Only set the timer if the movement was finished and the enemy is patrolling
		if (!Result.IsSuccess() || EnemyState != EEnemyState::EES_Patrolling) return;
		
		UE_LOG(LogTemp, Warning, TEXT("Path Following Request Finished"));

		const float WaitTime = FMath::RandRange(TargetReachedMinWait, TargetReachedMaxWait);
		const FTimerDelegate TargetReachedTimerDelegate = FTimerDelegate::CreateWeakLambda(this, [this] {
			AActor* NewTargetActor = PickRandomPatrolTarget();
			if (NewTargetActor != nullptr) MoveToTarget(NewTargetActor, TargetReachedAcceptanceRadius);;
		});

		GetWorldTimerManager().SetTimer(PatrolTimerHandle, TargetReachedTimerDelegate, WaitTime, false);
	});
	
	AActor* TargetActor = PickRandomPatrolTarget();
	if (TargetActor != nullptr) MoveToTarget(TargetActor, TargetReachedAcceptanceRadius);
}

void AEnemy::StartPatrolling() {
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
			
	if (PatrolTargets.Num() > 0) {
		AActor* PatrolTarget = PickRandomPatrolTarget();
		if (PatrolTarget != nullptr) MoveToTarget(PatrolTarget, TargetReachedAcceptanceRadius);
	}
	else {
		AIController->StopMovement();
	}
}

AActor* AEnemy::PickRandomPatrolTarget() const {
	const int32 NumPatrolTargets = PatrolTargets.Num();
	checkf(NumPatrolTargets > 0, TEXT("[%s] PatrolTargets is empty"), *GetName());
	
	AActor* CurrentMoveTarget = MoveTarget.Get();
	
	// Initial pick
	if (CurrentMoveTarget == nullptr) {
		return PatrolTargets[FMath::RandRange(0, NumPatrolTargets - 1)];
	}
	
	// If only 1 target exists:
	// return it on the first run, but return nullptr once already there.
	if (NumPatrolTargets == 1) {
		return (CurrentMoveTarget != PatrolTargets[0]) ? PatrolTargets[0] : nullptr;
	}
	
	const int32 CurrentIndex = PatrolTargets.IndexOfByKey(CurrentMoveTarget);
	
	// If MoveTarget is not in the array, pick a random target
	if (CurrentIndex == INDEX_NONE) {
		return PatrolTargets[FMath::RandRange(0, NumPatrolTargets - 1)];
	}
	// Otherwise, pick a different target using modulo offset
	const int32 RandomOffset = FMath::RandRange(1, NumPatrolTargets - 1);
	const int32 NextIndex = (CurrentIndex + RandomOffset) % NumPatrolTargets;
	
	checkf(PatrolTargets[NextIndex] != nullptr, TEXT("[%s] The picked target from PatrolTargets is null"), *GetName());
	
	return PatrolTargets[NextIndex];
}

void AEnemy::MoveToTarget(AActor* Target, float AcceptanceRadius) {
	checkf(Target != nullptr, TEXT("[%s] Target is null in MoveToTarget"), *GetName());
	
	MoveTarget = Target;
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	
	AIController->MoveTo(MoveRequest);
}

void AEnemy::OnSeePawn(APawn* SeenPawn) {
	if (!SeenPawn->ActorHasTag(FName("PlayerCharacter")) || EnemyState != EEnemyState::EES_Patrolling) return;
	
	UE_LOG(LogTemp, Warning, TEXT("Seen Character"));
	
	CombatTarget = SeenPawn;
}

bool AEnemy::CanAttack() const {
	const AActor* Target = CombatTarget.Get(); 
	return Target != nullptr && IsInTargetRange(Target, CombatRadius);
}

void AEnemy::Attack() {
	if (!ensureAlwaysMsgf(EnemyState == EEnemyState::EES_InCombat, TEXT("[%s] Tried attacking while not in the InCombat state"), *GetName())) return;
	checkf(AIController != nullptr, TEXT("[%s] AIController is null"), *GetName());

	Super::Attack();
	
	ActionState = EEnemyActionState::EEAS_StateLocked;
	AIController->StopMovement();
	PlayAttackMontage();
}

void AEnemy::AttackMontageEnd() {
	ActionState = EEnemyActionState::EEAS_Unoccupied;

	if (!CanAttack()) return;
	GetWorldTimerManager().SetTimer(AttackTimerHandle, this, &AEnemy::AttackTimerEnd, FMath::RandRange(AttackTimerMin, AttackTimerMax));
}

void AEnemy::AttackTimerEnd() {
	if (!CanAttack()) return;
	Attack();
}

void AEnemy::ShowHealthBar() const {
	checkf(HealthBarComponent != nullptr, TEXT("[%s] HealthBarComponent is null"), *GetName());
	
	if (!HealthBarComponent->IsVisible()) {
		HealthBarComponent->SetVisibility(true);
	}
}

void AEnemy::HideHealthBar() const {
	checkf(HealthBarComponent != nullptr, TEXT("[%s] HealthBarComponent is null"), *GetName());
	
	if (HealthBarComponent->IsVisible()) {
		HealthBarComponent->SetVisibility(false);
	}
}