#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"


class APlayerCharacter;
class UCharacterMovementComponent;

UCLASS()
class ACTIONRPGCOURSE_API UPlayerAnimInstance : public UAnimInstance {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	APlayerCharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	UCharacterMovementComponent* PlayerCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool IsFalling; 
	
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
