#include "Bird.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
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

	// Get the APlayerController from this pawn's AController
	if(const APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		// Get the UEnhancedInputLocalPlayerSubsystem from the ULocalPlayer class, passing the PlayerController pointer we just got
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			// Add the UInputMappingContext that's been set up in the editor
			Subsystem->AddMappingContext(BirdMappingContext, 0);
		}
	}
}

void ABird::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Cast the UInputComponent to the derived UEnhancedInputComponent class
	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		// Set up callbacks for the MoveAction input action
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABird::MoveTriggered);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ABird::MoveCompleted);
	}
}

void ABird::MoveTriggered(const FInputActionValue& Value) {
	const bool CurrentValue = Value.Get<bool>();
	UE_LOG(LogTemp, Warning, TEXT("MoveAction Triggered (Value: %s)"), CurrentValue ? TEXT("true") : TEXT("false"));
}

void ABird::MoveCompleted(const FInputActionValue& Value) {
	const bool CurrentValue = Value.Get<bool>();
	UE_LOG(LogTemp, Warning, TEXT("MoveAction Completed (Value: %s)"), CurrentValue ? TEXT("true") : TEXT("false"));
}

