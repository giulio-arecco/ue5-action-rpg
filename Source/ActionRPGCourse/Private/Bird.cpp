#include "Bird.h"
#include "Components/CapsuleComponent.h"

ABird::ABird() {
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BirdCapsuleComponent"));
	Capsule->SetCapsuleSize(15.0f, 20.0f);

	SetRootComponent(Capsule);
}

void ABird::BeginPlay() {
	Super::BeginPlay();
	
}

void ABird::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

