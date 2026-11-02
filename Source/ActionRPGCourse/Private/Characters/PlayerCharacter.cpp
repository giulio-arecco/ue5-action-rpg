#include "Characters/PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GroomComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Items/Weapons/Weapon.h"

APlayerCharacter::APlayerCharacter() {
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
	checkf(AttributeComponent != nullptr, TEXT("[%s] AttributeComponent is null"), *GetName());
	
	Super::BeginPlay();
	
	Tags.Add(FName("PlayerCharacter"));

	const APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()); 
	checkf(Subsystem != nullptr, TEXT("[%s] Local Player Subsystem is null"), *GetName());
	
	Subsystem->AddMappingContext(DefaultMappingContext, 0);
}

void APlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::MoveTriggered);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::LookTriggered);
	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::InteractStarted);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerCharacter::AttackStarted);
	EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &APlayerCharacter::DodgeStarted);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::Jump);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
}

void APlayerCharacter::GetHit_Implementation(const AActor* Hitter, const FVector& ImpactPoint, const FVector& ImpactNormal) {
	if (ActionState == ECharacterActionState::ECAS_Dead || ActionState == ECharacterActionState::ECAS_Dodging) return;
		
	Super::GetHit_Implementation(Hitter, ImpactPoint, ImpactNormal);
	
	ActionState = ECharacterActionState::ECAS_ReactingToHit;
	PlayHitReactMontage(Hitter->GetActorLocation());
}

void APlayerCharacter::HitReactMontageEnd() {
	Super::HitReactMontageEnd();
	ActionState = ECharacterActionState::ECAS_Unoccupied;
}

void APlayerCharacter::MoveTriggered(const FInputActionValue& Value) {
	if (ActionState != ECharacterActionState::ECAS_Unoccupied) return;
	
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

void APlayerCharacter::Jump() {
	if (ActionState != ECharacterActionState::ECAS_Unoccupied) return;
	Super::Jump();
}

void APlayerCharacter::LookTriggered(const FInputActionValue& Value) {
	const FVector2D LookVector = Value.Get<FVector2D>();

	if (GetController() != nullptr && LookVector != FVector2D::ZeroVector) {
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

void APlayerCharacter::InteractStarted() {
	if (ActionState != ECharacterActionState::ECAS_Unoccupied || GetCharacterMovement()->IsFalling()) return;
	
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon != nullptr) {
		EquipWeapon(OverlappingWeapon);
	}
	else {
		if (CanArm()) Arm();
		else if (CanDisarm()) Disarm();
	}
}

void APlayerCharacter::DodgeStarted() {
	if (ActionState != ECharacterActionState::ECAS_Unoccupied || GetCharacterMovement()->IsFalling()) return;
	ActionState = ECharacterActionState::ECAS_Dodging;
	PlayDodgeMontage();
}

void APlayerCharacter::PlayDodgeMontage() const {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	if (!ensureMsgf(AnimInstance != nullptr, TEXT("[%s] AnimInstance is null"), *GetName())) return;
	if (!ensureMsgf(DodgeMontage != nullptr, TEXT("[%s] DodgeMontage is null"), *GetName())) return;
	
	AnimInstance->Montage_Play(DodgeMontage);
	// AnimInstance->Montage_JumpToSection(FName("Default"), DodgeMontage);
}

void APlayerCharacter::EquipWeapon(AWeapon* OverlappingWeapon) {
	OverlappingWeapon->Equip(GetMesh(), WeaponHandSocketName, this, this);
	CharacterState = ECharacterState::ECS_Armed;
	EquippedWeapon = OverlappingWeapon;
	OverlappingItem = nullptr;
}

bool APlayerCharacter::CanArm() const {
	return ActionState == ECharacterActionState::ECAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Disarmed &&
		!GetCharacterMovement()->IsFalling() &&
		EquippedWeapon != nullptr;
}

bool APlayerCharacter::CanDisarm() const {
	return ActionState == ECharacterActionState::ECAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Armed &&
		!GetCharacterMovement()->IsFalling() &&
		EquippedWeapon != nullptr;
}

void APlayerCharacter::Arm() {
	// The CharacterState is set on the AnimNotify at the end of the Arm montage section
	ActionState = ECharacterActionState::ECAS_ArmingOrDisarming;
	PlayArmMontage(FName("Equip"));
}

void APlayerCharacter::Disarm() {
	// The CharacterState is set on the AnimNotify at the end of the Disarm montage section
	ActionState = ECharacterActionState::ECAS_ArmingOrDisarming;
	PlayArmMontage(FName("Unequip"));
}

void APlayerCharacter::PlayArmMontage(FName SectionName) const {
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	if (!ensureMsgf(AnimInstance != nullptr, TEXT("[%s] AnimInstance is null"), *GetName())) return;
	if (!ensureMsgf(EquipMontage != nullptr, TEXT("[%s] EquipMontage is null"), *GetName())) return;
	
	AnimInstance->Montage_Play(EquipMontage);
	AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
}

void APlayerCharacter::AttackStarted() {
	Attack();
}

bool APlayerCharacter::CanAttack() const {
	return CharacterState == ECharacterState::ECS_Armed &&
		ActionState == ECharacterActionState::ECAS_Unoccupied &&
		!GetCharacterMovement()->IsFalling();
}

void APlayerCharacter::Attack() {
	if (CanAttack()) {
		Super::Attack();
		
		ActionState = ECharacterActionState::ECAS_Attacking;
		PlayAttackMontage();
	}
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	if (ActionState == ECharacterActionState::ECAS_Dead || ActionState == ECharacterActionState::ECAS_Dodging) return 0.0f;
	
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	ReceiveDamage(DamageAmount);
	if (!IsAlive()) Die();
	
	return DamageAmount;
}

void APlayerCharacter::Die() {
	Super::Die();
	
	UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter is Dead"));
	ActionState = ECharacterActionState::ECAS_Dead;
}

void APlayerCharacter::AttachWeaponToHand() const {
	if (!ensureAlwaysMsgf(EquippedWeapon != nullptr, TEXT("AttachWeaponToHand called via AnimNotify, but EquippedWeapon is null."))) {
		return;
	}
	EquippedWeapon->AttachWeaponToSocket(GetMesh(), WeaponHandSocketName);
}

void APlayerCharacter::AttachWeaponToSpine() const {
	if (!ensureAlwaysMsgf(EquippedWeapon != nullptr, TEXT("AttachWeaponToSpine called via AnimNotify, but EquippedWeapon is null."))) {
		return;
	}
	EquippedWeapon->AttachWeaponToSocket(GetMesh(), WeaponSpineSocketName);
}
