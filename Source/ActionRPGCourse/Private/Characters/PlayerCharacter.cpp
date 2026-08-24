#include "Characters/PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Chaos/SoftsSpring.h"
#include "GameFramework/SpringArmComponent.h"

APlayerCharacter::APlayerCharacter() {
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("PlayerCharacterSpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.0f;
	
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCharacterCamera"));
	ViewCamera->SetupAttachment(SpringArm);
}

void APlayerCharacter::BeginPlay() {
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void APlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if(UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveTriggered);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::LookTriggered);
	}
}

void APlayerCharacter::MoveTriggered(const FInputActionValue& Value) {
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if(GetController() != nullptr && MovementVector != FVector2D::ZeroVector) {
		const FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, MovementVector.Y);
		const FVector Right = GetActorRightVector();
		AddMovementInput(Right, MovementVector.X);
	}
}

void APlayerCharacter::LookTriggered(const FInputActionValue& Value) {
	const FVector2D LookVector = Value.Get<FVector2D>();

	if (GetController() != nullptr && LookVector != FVector2D::ZeroVector) {
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

