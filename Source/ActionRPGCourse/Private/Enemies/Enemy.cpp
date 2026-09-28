#include "Enemies/Enemy.h"
#include "Components/CapsuleComponent.h"

AEnemy::AEnemy() {
	PrimaryActorTick.bCanEverTick = true;

	USkeletalMeshComponent* Skm = GetMesh();
	Skm->SetCollisionObjectType(ECC_WorldDynamic);
	Skm->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	Skm->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Skm->SetGenerateOverlapEvents(true);	
	
	UCapsuleComponent*  Capsule = GetCapsuleComponent();
	Capsule->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
}

void AEnemy::BeginPlay() {
	Super::BeginPlay();
}

void AEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::GetHit() {
}

