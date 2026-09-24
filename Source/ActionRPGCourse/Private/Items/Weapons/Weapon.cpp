#include "Items/Weapons/Weapon.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon() {
	Box = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Box->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay() {
	Super::BeginPlay();
}

void AWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AWeapon::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	Super::OnSphereBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::Equip(USceneComponent* InParent, const FName& InSocket) {
	if (InParent != nullptr) {
		AttachMeshToSocket(InParent, InSocket);
		
		GetSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		
		ItemState = EItemState::EIS_Equipped;
		
		if (EquipSound != nullptr) {
			UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
		}
	}
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocket) const {
	const FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocket);
}
