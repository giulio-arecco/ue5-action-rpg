#include "Items/Item.h"

#include "Characters/PlayerCharacter.h"
#include "Components/SphereComponent.h"

AItem::AItem(): MovementAmplitude(10.0f), MovementPeriod(2.5f), StartLocation(0.0f), RunningTime(0.0f) {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	SetRootComponent(ItemMesh);

	ItemMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	ItemMesh->SetCollisionObjectType(ECC_WorldDynamic);
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
	ItemMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("ItemSphere"));
	Sphere->SetupAttachment(GetRootComponent());
	Sphere->SetSphereRadius(150.0f);
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
}

float AItem::TransformedSin() const {
	return MovementAmplitude * FMath::Sin(RunningTime * 2 * UE_PI / MovementPeriod);
}

float AItem::TransformedCos() const {
	return MovementAmplitude * FMath::Cos(RunningTime * 2 * UE_PI / MovementPeriod);
}

void AItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter != nullptr) {
		PlayerCharacter->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter != nullptr) {
		PlayerCharacter->SetOverlappingItem(nullptr);
	}
}