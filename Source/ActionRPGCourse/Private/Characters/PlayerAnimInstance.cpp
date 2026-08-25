#include "Characters/PlayerAnimInstance.h"

#include "Characters/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayerAnimInstance::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	if (PlayerCharacter != nullptr) {
		PlayerCharacterMovement = PlayerCharacter->GetCharacterMovement();
	}
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds) {
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (PlayerCharacterMovement != nullptr) {
		const FVector CurrentVelocity = PlayerCharacterMovement->Velocity;
		GroundSpeed = UKismetMathLibrary::VSizeXY(CurrentVelocity);
		IsFalling = PlayerCharacterMovement->IsFalling();
	}
}
