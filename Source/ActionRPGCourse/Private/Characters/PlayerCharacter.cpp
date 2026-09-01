#include "Characters/PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GroomComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Items/Weapons/Weapon.h"

APlayerCharacter::APlayerCharacter() : WeaponHandSocketName("hand_r_socket"), CharacterState(ECharacterState::ECS_Unequipped) {
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	// Set the character to orient its rotation to movement and update the character's default yaw rotation rate
	UCharacterMovementComponent* CharMovement = GetCharacterMovement();
	CharMovement->bOrientRotationToMovement = true;
	CharMovement->RotationRate = FRotator(0.0f, 480.0f, 0.0f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("PlayerCharacterSpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.0f;
	
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCharacterCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	// Setup GroomComponents
	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("PlayerCharacterHair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("PlayerCharacterEyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");
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
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::InteractTriggered);
		// We use the ACharacter::Jump function to handle jumping, since currently there's no need for a APlayerCharacter::JumpTriggered function 
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
	}
}

void APlayerCharacter::MoveTriggered(const FInputActionValue& Value) {
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if(GetController() != nullptr && MovementVector != FVector2D::ZeroVector) {
		// Get the Controller yaw 
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

		// Get the Controller forward direction
		const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// Move this character along its Controller forward direction
		AddMovementInput(ForwardDir, MovementVector.Y);

		// Get the Controller right direction
		const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// Move this character along its Controller right direction
		AddMovementInput(RightDir, MovementVector.X);
	}
}

void APlayerCharacter::LookTriggered(const FInputActionValue& Value) {
	const FVector2D LookVector = Value.Get<FVector2D>();

	if (GetController() != nullptr && LookVector != FVector2D::ZeroVector) {
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void APlayerCharacter::InteractTriggered() {
	const AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon != nullptr) {
		// TODO: Avoid equipping an already equipped overlapping item
		OverlappingWeapon->Equip(GetMesh(), WeaponHandSocketName);
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	}
}

