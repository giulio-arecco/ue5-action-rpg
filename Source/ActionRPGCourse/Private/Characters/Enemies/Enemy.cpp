#include "Characters/Enemies/Enemy.h"

#include "AIController.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"

AEnemy::AEnemy() {
	PrimaryActorTick.bCanEverTick = true;
	
	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("EnemyAttributes"));

	HealthBarComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("EnemyHealthBarWidget"));
	HealthBarComponent->SetupAttachment(GetRootComponent());
	
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
	
	HealthBarComponent->SetHealthPercent(AttributeComponent->GetHealthPercent());
	HideHealthBar();
	
	AIController = CastChecked<AAIController>(GetController());
	
	// Patrolling logic
	if (PatrolTargets.Num() <= 0) return;
	
	MoveTarget = PickRandomPatrolTarget();
	if (MoveTarget == nullptr) return;
	
	UPathFollowingComponent* PathFollowingComp = AIController->GetPathFollowingComponent();
	checkf(PathFollowingComp != nullptr, TEXT("[%s] PathFollowingComponent is null"), *GetName());

	PathFollowingComp->OnRequestFinished.AddWeakLambda(this, [this](FAIRequestID RequestID, const FPathFollowingResult& Result) {
		if (!Result.IsSuccess()) return;
		// TODO: Return depending on the Enemy state

		const float WaitTime = FMath::RandRange(TargetReachedMinWait, TargetReachedMaxWait);
		const FTimerDelegate TargetReachedTimerDelegate = FTimerDelegate::CreateWeakLambda(this, [this] {
			MoveTarget = PickRandomPatrolTarget();
			if (MoveTarget != nullptr) MoveToTarget(MoveTarget.Get(), 30.0f);;
		});

		GetWorldTimerManager().SetTimer(TargetReachedTimerHandle, TargetReachedTimerDelegate, WaitTime, false);
	});
	
	MoveToTarget(MoveTarget.Get(), 30.0f);
}

void AEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	// Only check the distance to the CombatTarget if it's not null
	if (const AActor* Target = CombatTarget.Get(); Target != nullptr) {
		if (!IsWithinCombatRadius(Target)) {
			CombatTarget = nullptr;
			HideHealthBar();
		}
	}
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, const FVector& ImpactNormal) {
	if (AttributeComponent != nullptr) {
		if (AttributeComponent->IsAlive()) {
			PlayHitReactMontage(ImpactPoint);
		}
		else {
			PlayDeathMontage();
		}
	}
	
	if (HitSound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}
	
	if (HitParticles != nullptr) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint, ImpactNormal.Rotation());
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (DamageCauser != nullptr) {
		// If a EventInstigator used the DamageCauser as a weapon, set the target to
		// the EventInstigator pawn, otherwise set it to the DamageCauser (actor)
		CombatTarget = (EventInstigator && EventInstigator->GetPawn()) ? EventInstigator->GetPawn() : DamageCauser;
	}
	
	if (AttributeComponent != nullptr) {
		AttributeComponent->ReceiveDamage(DamageAmount);
		
		if (HealthBarComponent != nullptr) {
			if (AttributeComponent->IsAlive()) {
				ShowHealthBar();
				HealthBarComponent->SetHealthPercent(AttributeComponent->GetHealthPercent());
			}
			else HideHealthBar();
		}
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
	USkeletalMeshComponent* Skm = GetMesh();
	if (Skm != nullptr) {
		Skm->bPauseAnims = true;
		Skm->SetComponentTickEnabled(false);
		Skm->SetGenerateOverlapEvents(false);
		Skm->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetActorTickEnabled(false);
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

void AEnemy::ShowHealthBar() const {
	if (HealthBarComponent != nullptr && !HealthBarComponent->IsVisible()) {
		HealthBarComponent->SetVisibility(true);
	}
}

void AEnemy::HideHealthBar() const {
	if (HealthBarComponent != nullptr && HealthBarComponent->IsVisible()) {
		HealthBarComponent->SetVisibility(false);
	}
}

bool AEnemy::IsWithinCombatRadius(const AActor* Target) const {
	check(Target);
	
	const double DistSq = FVector::DistSquared(GetActorLocation(), Target->GetActorLocation());
	return DistSq < FMath::Square(CombatRadius);
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

void AEnemy::MoveToTarget(const AActor* Target, float AcceptanceRadius) const {
	checkf(Target != nullptr, TEXT("[%s] Target is null in MoveToTarget"), *GetName());
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AcceptanceRadius);
	
	FNavPathSharedPtr NavPath;
	AIController->MoveTo(MoveRequest, &NavPath);
	
	// Debug
	for (const FNavPathPoint& Point : NavPath->GetPathPoints()) {
		DrawDebugSphere(GetWorld(), Point.Location, 25.0f, 12, FColor::Green, false, 10.0f);
	}
}
