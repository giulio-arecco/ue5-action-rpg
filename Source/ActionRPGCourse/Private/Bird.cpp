#include "Bird.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"

ABird::ABird() {
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BirdCapsule"));
	Capsule->SetCapsuleSize(15.0f, 20.0f);
	// Replaces the DefaultRootComponent in the component hierarchy
	SetRootComponent(Capsule);

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdSkeletalMesh"));
	// Makes BirdMesh a child component of the RootComponent
	BirdMesh->SetupAttachment(GetRootComponent());

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("BirdSpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("BirdCamera"));
	// Makes BirdMesh a child component of the SpringArm component
	Camera->SetupAttachment(SpringArm);

	// Makes this Pawn pitch and yaw follow its Controller pitch and yaw
	bUseControllerRotationPitch = true;
	bUseControllerRotationYaw = true;
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
		// Set up callbacks for the input actions
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABird::MoveTriggered);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABird::LookTriggered);
	}
}

void ABird::MoveTriggered(const FInputActionValue& Value) {
	// DirectionValue can currently store 0.0f, 1.0f or -1.0f
	const float DirectionValue = Value.Get<float>();
	
	if (GetController() != nullptr && DirectionValue != 0.0f) {
		const FVector Forward = GetActorForwardVector();
		// Apply the movement along the Forward direction, scaled by DirectionValue
		AddMovementInput(Forward, DirectionValue);
	}
}

void ABird::LookTriggered(const FInputActionValue& Value) {
	const FVector2D LookAxisValue = Value.Get<FVector2D>();
	
	if (GetController() != nullptr && LookAxisValue != FVector2D::ZeroVector) {
		// Add the input value to the pawn's controller rotation (the pawn must be set up to follow the controller's rotation)
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

