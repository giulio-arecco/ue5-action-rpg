#include "Enemies/Enemy.h"

#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Kismet/GameplayStatics.h"

AEnemy::AEnemy() {
	PrimaryActorTick.bCanEverTick = true;
	
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("EnemyAttributes"));

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
}

void AEnemy::BeginPlay() {
	Super::BeginPlay();
	if (Attributes != nullptr && HealthBarComponent != nullptr) {
		HealthBarComponent->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, const FVector& ImpactNormal) {
	if (Attributes != nullptr) {
		if (Attributes->IsAlive()) {
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
	
	if (Attributes != nullptr && HealthBarComponent != nullptr) {
		Attributes->ReceiveDamage(DamageAmount);
		HealthBarComponent->SetHealthPercent(Attributes->GetHealthPercent());
	}
	
	return DamageAmount;
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

void AEnemy::PlayDeathMontage() {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr && DeathMontage != nullptr) {
		AnimInstance->Montage_Play(DeathMontage);

		if (const int32 NumSections = DeathMontage->GetNumSections(); NumSections > 0) {
			const int32 RandomIndex = FMath::RandRange(0, NumSections - 1);
			const FName SectionName = DeathMontage->GetSectionName(RandomIndex);
			AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
		}
		
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (HealthBarComponent != nullptr) HealthBarComponent->SetVisibility(false);
		
		// Deferred cleanup (once the death animation completes)
		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindWeakLambda(this, [this](UAnimMontage* Montage, bool bInterrupted) {
			if (!bInterrupted) {
				USkeletalMeshComponent* Mesh = GetMesh();
				Mesh->bPauseAnims = true;
				Mesh->SetComponentTickEnabled(false);
				Mesh->SetGenerateOverlapEvents(false);
				Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				SetActorTickEnabled(false);
			}
		});
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, DeathMontage);
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

