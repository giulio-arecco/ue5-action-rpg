#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"

#include "Bird.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UCapsuleComponent;
class USkeletalMeshComponent;
class UInputMappingContext;

UCLASS()
class ACTIONRPGCOURSE_API ABird : public APawn {
	GENERATED_BODY()

public:
	ABird();

	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* BirdMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;
	
	virtual void BeginPlay() override;

	// The callback related to the MoveAction input action triggered event
	void MoveTriggered(const FInputActionValue& Value);

	// The callback related to the LookAction input action triggered event
	void LookTriggered(const FInputActionValue& Value);

private:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* Capsule;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* BirdMesh;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;
};
