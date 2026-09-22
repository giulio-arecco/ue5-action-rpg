#include "Characters/PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GroomComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Items/Weapons/Weapon.h"

APlayerCharacter::APlayerCharacter() : 
	WeaponHandSocketName("hand_r_socket"), 
	CharacterState(ECharacterState::ECS_Unequipped), 
	ActionState(EActionState::EAS_Unoccupied) 
{
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
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::AttackTriggered);
		// We use the ACharacter::Jump function to handle jumping, since currently there's no need for a APlayerCharacter::JumpTriggered function 
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}
}

void APlayerCharacter::MoveTriggered(const FInputActionValue& Value) {
	if (ActionState == EActionState::EAS_Attacking) return;
	
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
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon != nullptr) {
		OverlappingWeapon->Equip(GetMesh(), WeaponHandSocketName);
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		OverlappingItem = nullptr;
	}
}

void APlayerCharacter::AttackTriggered() {
	if (CanAttack()) {
		ActionState = EActionState::EAS_Attacking;
		PlayAttackMontage();
	}
}

bool APlayerCharacter::CanAttack() const {
	return CharacterState != ECharacterState::ECS_Unequipped &&
		ActionState == EActionState::EAS_Unoccupied;
}

void APlayerCharacter::PlayAttackMontage() const {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr && AttackMontage != nullptr) {
		AnimInstance->Montage_Play(AttackMontage);

		FName SectionName = FName();
		switch(FMath::RandRange(0, 1)) {
		case 0:
			SectionName = FName("Attack1");
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("APlayerCharacter::AttackTriggered - Invalid Montage Section"));
		}

		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

void APlayerCharacter::OnAttackEndNotify() {
	ActionState = EActionState::EAS_Unoccupied;
}

