#include "Items/Weapons/Weapon.h"

#include "NiagaraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon():
	BoxTraceHalfSize(5.0f, 5.0f, 5.0f),
	Damage(20.0f)
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(GetRootComponent());
	
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponBoxTraceStart"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponBoxTraceEnd"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay() {
	Super::BeginPlay();
	WeaponBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AWeapon::OnBoxBeginOverlap);
}

void AWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AWeapon::Equip(USceneComponent* InParent, const FName& InSocket, AActor* NewOwner, APawn* NewInstigator) {
	if (!ensureAlwaysMsgf(InParent != nullptr, TEXT("Weapon Equip called with null InParent."))) {
		return;
	}
	
	AttachMeshToSocket(InParent, InSocket);
	GetSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemState = EItemState::EIS_Equipped;

	SetOwner(NewOwner);
	SetInstigator(NewInstigator);

	if (EquipSound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}

	DeactivatePickupVFX();
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocket) const {
	const FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocket);
}

void AWeapon::SetWeaponBoxCollisionEnabled(ECollisionEnabled::Type CollisionEnabled) const {
	if (WeaponBox != nullptr) {
		WeaponBox->SetCollisionEnabled(CollisionEnabled);
	}
}

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();
	
	FHitResult BoxHit;
	const bool HitSomething = UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		BoxTraceHalfSize,
		BoxTraceStart->GetComponentRotation(),
		TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		BoxHit,
		true
	);
	
	if (!HitSomething) return;

	if (AActor* HitActor = BoxHit.GetActor(); HitActor != nullptr) {
		// Calls HitActor.TakeDamage
		UGameplayStatics::ApplyDamage(
			HitActor, 
			Damage, 
			GetInstigator()->GetController(), 
			this,
			UDamageType::StaticClass()
		);
		
		if (HitActor->Implements<UHitInterface>()) {
			IHitInterface::Execute_GetHit(HitActor, BoxHit.ImpactPoint, BoxHit.ImpactNormal);
		}
		
		ActorsToIgnore.AddUnique(HitActor);
	}
	
	CreateWeaponFields(BoxHit.ImpactPoint);
}
