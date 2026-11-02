#include "Items/Item.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "Interfaces/PickupInterface.h"
#include "Kismet/GameplayStatics.h"

AItem::AItem() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemRoot = CreateDefaultSubobject<USceneComponent>(TEXT("ItemRoot"));
	SetRootComponent(ItemRoot);
	
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetupAttachment(ItemRoot);
	
	// TODO: Add a PlayerOnlyQuery collision channel to avoid triggering unnecessary overlap events with other Actors.
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("ItemSphere"));
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Sphere->SetupAttachment(GetRootComponent());
	Sphere->SetSphereRadius(150.0f);
	
	ItemParticles = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemNiagaraPickup"));
	ItemParticles->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay() {
	Super::BeginPlay();

	StartLocation = GetActorLocation();

	// Bind a callback to the Sphere overlap delegates
	Sphere->OnComponentBeginOverlap.AddUniqueDynamic(this, &AItem::OnSphereBeginOverlap);
	Sphere->OnComponentEndOverlap.AddUniqueDynamic(this, &AItem::OnSphereEndOverlap);
}

void AItem::Tick(float DeltaTime) { 
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;
	
	if (ItemState == EItemState::EIS_Hovering) {
		SetActorLocation(FVector(StartLocation.X, StartLocation.Y, StartLocation.Z + TransformedCos()));
	}
}

void AItem::DeactivateItemParticles() const {
	if (ItemParticles != nullptr) {
		ItemParticles->Deactivate();
	}
}

float AItem::TransformedSin() const {
	return MovementAmplitude * FMath::Sin(RunningTime * 2 * UE_PI / MovementPeriod);
}

float AItem::TransformedCos() const {
	return MovementAmplitude * FMath::Cos(RunningTime * 2 * UE_PI / MovementPeriod);
}

void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface != nullptr) {
		PickupInterface->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);
	if (PickupInterface != nullptr) {
		PickupInterface->SetOverlappingItem(nullptr);
	}
}

void AItem::PlayPickupSound() const {
	if (PickupSound != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(this, PickupSound, GetActorLocation());
	}
}

void AItem::SpawnPickupEffect() const {
	if (PickupEffect != nullptr) {
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PickupEffect, GetActorLocation());
	}
}
