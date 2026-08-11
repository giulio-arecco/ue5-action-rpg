#include "Bird.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

ABird::ABird() {
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BirdCapsuleComponent"));
	Capsule->SetCapsuleSize(15.0f, 20.0f);
	// Replaces the DefaultRootComponent in the component hierarchy
	SetRootComponent(Capsule);

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdSkeletalMeshComponent"));
	// Makes BirdMesh a child component of the RootComponent
	BirdMesh->SetupAttachment(GetRootComponent());

	// Makes the Player0 PlayerController the possessor of this Pawn, allowing it to control it
	AutoPossessPlayer = EAutoReceiveInput::Player0;
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

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ABird::MoveForward);
}

// The "MoveForward" AxisMapping (set up in the editor project settings) callback
void ABird::MoveForward(float Value) {
	UE_LOG(LogTemp, Warning, TEXT("Value: %f"), Value);
}

