#include "Items/Weapons/Weapon.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Interfaces/HitInterface.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon() {
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(ItemMesh);
	
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponBoxTraceStart"));
	BoxTraceStart->SetupAttachment(ItemMesh);
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponBoxTraceEnd"));
	BoxTraceEnd->SetupAttachment(ItemMesh);
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
	
	AttachWeaponToSocket(InParent, InSocket);
	GetSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemState = EItemState::EIS_Equipped;

	SetOwner(NewOwner);
	SetInstigator(NewInstigator);

	/* 
	 * TODO: Move the equip sound playing elsewhere so that it only plays when a Character picks up a Weapon.
	 * Right now it can be played even if Equip is called in BeginPlay to start with the Weapon already in hand.
	*/
	if (EquipSound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}

	DeactivatePickupVFX();
}

void AWeapon::AttachWeaponToSocket(USceneComponent* InParent, const FName& InSocket) const {
	ItemMesh->ResetRelativeTransform();
	const FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemRoot->AttachToComponent(InParent, TransformRules, InSocket);
}

void AWeapon::SetWeaponBoxCollisionEnabled(ECollisionEnabled::Type CollisionEnabled) const {
	if (WeaponBox != nullptr) {
		WeaponBox->SetCollisionEnabled(CollisionEnabled);
	}
}

bool AWeapon::DoBoxTrace(FHitResult& OutHitResult) {
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();
	
	ActorsToIgnore.AddUnique(GetOwner());

	const bool HitSomething = UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		BoxTraceHalfSize,
		BoxTraceStart->GetComponentRotation(),
		TraceTypeQuery1,
		false,
		ActorsToIgnore,
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		OutHitResult,
		true
	);
		
	if (HitSomething) ActorsToIgnore.AddUnique(OutHitResult.GetActor());
	return HitSomething;
}

void AWeapon::HitActor(AActor* ActorToHit, const FVector& ImpactPoint, const FVector& ImpactNormal) {
	if (!ensureAlwaysMsgf(ActorToHit != nullptr, TEXT("[%s] ActorToHit was null in HitActor"), *GetName())) return;
	
	// Calls HitActor.TakeDamage
	UGameplayStatics::ApplyDamage(
		ActorToHit, 
		Damage, 
		GetInstigator()->GetController(), 
		this,
		UDamageType::StaticClass()
	);
	
	if (ActorToHit->Implements<UHitInterface>()) {
		IHitInterface::Execute_GetHit(ActorToHit, GetOwner(), ImpactPoint, ImpactNormal);
	}
}

void AWeapon::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	FHitResult BoxHitResult;
	const bool HitSomething = DoBoxTrace(BoxHitResult);

	if (GetOwner()->ActorHasTag(FName("Enemy")) && OtherActor->ActorHasTag(FName("Enemy")) 
		|| !HitSomething) return;
	
	if (AActor* ActorToHit = BoxHitResult.GetActor(); ActorToHit != nullptr) {
		HitActor(ActorToHit, BoxHitResult.ImpactPoint, BoxHitResult.ImpactNormal);
	}
	
	CreateWeaponFields(BoxHitResult.ImpactPoint);
}
