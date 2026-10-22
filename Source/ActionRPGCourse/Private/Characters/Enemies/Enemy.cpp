#include "Characters/Enemies/Enemy.h"

#include "AIController.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

AEnemy::AEnemy() {
	PrimaryActorTick.bCanEverTick = true;
	
	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("EnemyAttributes"));

	HealthBarComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("EnemyHealthBarWidget"));
	HealthBarComponent->SetupAttachment(GetRootComponent());
	
	// TODO: Update PawnSensing to AI Perception
	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("EnemyPawnSensingComponent"));
	PawnSensingComponent->SetPeripheralVisionAngle(60.0f);
	PawnSensingComponent->SightRadius = 1500.0f;
	
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	Capsule->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	USkeletalMeshComponent* Skm = GetMesh();
	Skm->SetCollisionObjectType(ECC_WorldDynamic);
	Skm->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Skm->SetCollisionResponseToAllChannels(ECR_Ignore);
	Skm->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Skm->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Skm->SetGenerateOverlapEvents(true);	
	
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
	
	HealthBarComponent->SetHealthPercent(AttributeComponent->GetHealthPercent());
	HideHealthBar();
	
	PawnSensingComponent->OnSeePawn.AddUniqueDynamic(this, &AEnemy::OnSeePawn);
	
	// Patrolling logic
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	
	AIController = CastChecked<AAIController>(GetController());
	
	if (PatrolTargets.Num() <= 0) return;
	
	UPathFollowingComponent* PathFollowingComp = AIController->GetPathFollowingComponent();
	checkf(PathFollowingComp != nullptr, TEXT("[%s] PathFollowingComponent is null"), *GetName());

	PathFollowingComp->OnRequestFinished.AddWeakLambda(this, [this] (FAIRequestID RequestID, const FPathFollowingResult& Result) {
		// Only set the timer if the movement was finished and the enemy is patrolling
		if (!Result.IsSuccess() || EnemyState != EEnemyState::EES_Patrolling) return;
		
		UE_LOG(LogTemp, Warning, TEXT("OnRequestFinished"));

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

void AEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	AActor* Target = CombatTarget.Get();
	
	switch (EnemyState) {
	// The conditions to transition to the Dead state are checked in TakeDamage
	case EEnemyState::EES_Patrolling:
		if (Target == nullptr) break;
		if (IsInTargetRange(Target, AttackRadius)) TransitionToState(EEnemyState::EES_Attacking);
		else if (IsInTargetRange(Target, CombatRadius)) TransitionToState(EEnemyState::EES_Chasing);
		break;
	case EEnemyState::EES_Chasing:
		if (Target == nullptr) TransitionToState(EEnemyState::EES_Patrolling);
		else if (IsInTargetRange(Target, AttackRadius)) TransitionToState(EEnemyState::EES_Attacking);
		else if (!IsInTargetRange(Target, CombatRadius)) TransitionToState(EEnemyState::EES_Patrolling);
		break;
	case EEnemyState::EES_Attacking:
		if (Target == nullptr) TransitionToState(EEnemyState::EES_Patrolling);
		else if (!IsInTargetRange(Target, CombatRadius)) TransitionToState(EEnemyState::EES_Patrolling);
		else if (!IsInTargetRange(Target, AttackRadius)) TransitionToState(EEnemyState::EES_Chasing);
		break;
	case EEnemyState::EES_Dead:
		break;
	}
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
			
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
			
		if (PatrolTargets.Num() > 0) {
			AActor* PatrolTarget = PickRandomPatrolTarget();
			if (PatrolTarget != nullptr) MoveToTarget(PatrolTarget, TargetReachedAcceptanceRadius);
		}
		else {
			AIController->StopMovement();
		}
		
		break;
	case EEnemyState::EES_Chasing:
		UE_LOG(LogTemp, Warning, TEXT("Chasing"));
		
		EnemyState = EEnemyState::EES_Chasing;
			
		ShowHealthBar();
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
			
		MoveToTarget(CombatTarget.Get(), TargetReachedAcceptanceRadius);
		
		break;
	case EEnemyState::EES_Attacking:
		UE_LOG(LogTemp, Warning, TEXT("Attacking"));
		
		EnemyState = EEnemyState::EES_Attacking;
			
		ShowHealthBar();
		GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
		
		AIController->StopMovement();
		
		break;
	case EEnemyState::EES_Dead:
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
	
		HideHealthBar();
		GetWorldTimerManager().ClearTimer(PatrolTimerHandle);
	
		PlayDeathMontage();
	
		SetActorTickEnabled(false);
		break;
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, const FVector& ImpactNormal) {
	checkf(AttributeComponent != nullptr, TEXT("[%s] AttributeComponent is null in GetHit_Implementation."), *GetName());
	
	if (EnemyState != EEnemyState::EES_Dead && AttributeComponent->IsAlive()) {
		PlayHitReactMontage(ImpactPoint);
	}
	
	if (HitSound != nullptr) UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	if (HitParticles != nullptr) UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint, ImpactNormal.Rotation());
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	if (EnemyState == EEnemyState::EES_Dead) return 0.0f;
	
	checkf(AttributeComponent != nullptr, TEXT("[%s] AttributeComponent is null"), *GetName());
	checkf(HealthBarComponent != nullptr, TEXT("[%s] HealthBarComponent is null"), *GetName());
	
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (DamageCauser != nullptr) {
		// If a EventInstigator used the DamageCauser as a weapon, set the target to
		// the EventInstigator pawn, otherwise set it to the DamageCauser (actor)
		CombatTarget = (EventInstigator && EventInstigator->GetPawn()) ? EventInstigator->GetPawn() : DamageCauser;
	}
	
	AttributeComponent->ReceiveDamage(DamageAmount);
	
	if (AttributeComponent->IsAlive()) {
		HealthBarComponent->SetHealthPercent(AttributeComponent->GetHealthPercent());
		ShowHealthBar();

		if (const AActor* Target = CombatTarget.Get(); Target != nullptr) {
			if (IsInTargetRange(Target, AttackRadius)) TransitionToState(EEnemyState::EES_Attacking);
			else if (IsInTargetRange(Target, CombatRadius)) TransitionToState(EEnemyState::EES_Chasing);
		}
	}
	else {
		TransitionToState(EEnemyState::EES_Dead);
	}
	
	return DamageAmount;
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

void AEnemy::PlayHitReactMontage(const FVector& ImpactPoint) const {
	const double Theta = ComputeAngleToImpact(ImpactPoint);
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr && HitReactMontage != nullptr) {
		AnimInstance->Montage_Play(HitReactMontage);
		
		FName Section;
		if (Theta >= -45.0 && Theta < 45.0) {
			Section = FName("FromFront");
		}
		else if (Theta >= 45.0 && Theta < 135.0) {
			Section = FName("FromRight");
		}
		else if (Theta >= -135.0 && Theta < -45.0) {
			Section = FName("FromLeft");
		}
		else {
			Section = FName("FromBack");
		}
		
		AnimInstance->Montage_JumpToSection(Section);
	}
}

double AEnemy::ComputeAngleToImpact(const FVector& ImpactPoint) const {
	const FVector Location = GetActorLocation();
	const FVector Up = GetActorUpVector();
	const FVector ToHit = FVector::VectorPlaneProject(ImpactPoint - Location, Up).GetSafeNormal();
	
	if (ToHit.IsNearlyZero()) return 0.0;
	
	const FVector Forward = GetActorForwardVector(); // already normalized
	const FVector Cross = FVector::CrossProduct(Forward, ToHit);
	// Since Cross is aligned with Up and Up is a unit vector, their dot product is: 
	// |Cross| * |Up| * cosAlpha = |Cross| * 1 * (±1) = ±|Cross|
	// and the sign information is needed for Atan2
	const double SinTheta = FVector::DotProduct(Cross, Up);
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	
	// We use Atan2 over Acos or Atan because:
	// 1. Eliminates division-by-zero: standard atan(y/x) breaks when CosTheta == 0;
	//    Atan2 safely inspects both coordinates natively without explicit division.
	// 2. Immune to float drift NaNs: Acos produces NaN if DotProduct exceeds 1.0 due to precision drift;
	//    Atan2 is well-defined across all real coordinate pairs (x, y) != (0, 0).
	// 3. Polar numerical stability: Acos derivative diverges to infinity at 0° (x = 1) and 180° (x = -1), exaggerating small errors,
	//    whereas Atan2 provides uniform numerical accuracy around the entire circle
	return FMath::RadiansToDegrees(FMath::Atan2(SinTheta, CosTheta));
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
	if (!SeenPawn->ActorHasTag(FName("PlayerCharacter"))
		|| EnemyState == EEnemyState::EES_Attacking
		|| EnemyState == EEnemyState::EES_Chasing
		|| EnemyState == EEnemyState::EES_Dead) 
		return;
	
	UE_LOG(LogTemp, Warning, TEXT("Seen Character"));
	
	CombatTarget = SeenPawn;
	TransitionToState(EEnemyState::EES_Chasing);
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

bool AEnemy::IsInTargetRange(const AActor* Target, float Range) const {
	check(Target);
	
	const double DistSq = FVector::DistSquared(GetActorLocation(), Target->GetActorLocation());
	return DistSq < FMath::Square(Range);
}