#include "Characters/BaseCharacter.h"

#include "Components/AttributeComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"

ABaseCharacter::ABaseCharacter() {
	PrimaryActorTick.bCanEverTick = true;
	
	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("EnemyAttributes"));
}

void ABaseCharacter::BeginPlay() {
	Super::BeginPlay();
}

void ABaseCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ABaseCharacter::Destroyed() {
	Super::Destroyed();
	
	if (EquippedWeapon != nullptr) {
		EquippedWeapon->Destroy();
	}
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ABaseCharacter::SetEquippedWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled) const {
	if (EquippedWeapon != nullptr) {
		EquippedWeapon->SetWeaponBoxCollisionEnabled(CollisionEnabled);
		EquippedWeapon->EmptyActorsToIgnore();
	}
}

void ABaseCharacter::PlayAttackMontage() const {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr && AttackMontage != nullptr) {
		AnimInstance->Montage_Play(AttackMontage);

		if (const int32 NumSections = AttackMontage->GetNumSections(); NumSections > 0) {
			const int32 RandomIndex = FMath::RandRange(0, NumSections - 1);
			const FName SectionName = AttackMontage->GetSectionName(RandomIndex);
			AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
		}
	}
}

double ABaseCharacter::ComputeAngleToImpact(const FVector& ImpactPoint) const {
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

void ABaseCharacter::PlayHitSound(const FVector& Location) const {
	if (!ensureAlwaysMsgf(HitSound != nullptr, TEXT("[%s] HitSound is null"), *GetName())) return;
	UGameplayStatics::PlaySoundAtLocation(this, HitSound, Location);
}

void ABaseCharacter::SpawnHitParticles(const FVector& Location, const FVector& Normal) const {
	if (!ensureAlwaysMsgf(HitParticles != nullptr, TEXT("[%s] HitParticles is null"), *GetName())) return;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, Location, Normal.Rotation());
}

bool ABaseCharacter::IsAlive() const {
	checkf(AttributeComponent != nullptr, TEXT("[%s] AttributeComponent is null"), *GetName());
	return AttributeComponent->IsAlive();
}

void ABaseCharacter::ReceiveDamage(const float DamageAmount) const {
	checkf(AttributeComponent != nullptr, TEXT("[%s] AttributeComponent is null"), *GetName());
	AttributeComponent->ReceiveDamage(DamageAmount);
}

bool ABaseCharacter::CanAttack() const { return false; }
void ABaseCharacter::Attack() {}
void ABaseCharacter::Die() {}
void ABaseCharacter::PlayHitReactMontage(const FVector& ImpactPoint) const {}
void ABaseCharacter::PlayDeathMontage() const {}



