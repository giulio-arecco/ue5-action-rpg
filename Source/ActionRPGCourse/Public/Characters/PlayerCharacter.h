#pragma once

#include "CoreMinimal.h"
#include "CharacterTypes.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class AWeapon;
class AItem;
class UGroomComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UAnimMontage;

UCLASS()
class ACTIONRPGCOURSE_API APlayerCharacter : public ACharacter {
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:
	virtual void BeginPlay() override;

	/*****************************
	 *  Input-related Variables  *
	 ****************************/ 
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AttackAction;

	/*********************************
	 *  Animation-related Variables  *
	 ********************************/ 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	FName WeaponHandSocketName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	FName WeaponSpineSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* EquipMontage;

private:
	UPROPERTY(VisibleInstanceOnly)
	ECharacterState CharacterState;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;
	
	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Hair;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	UGroomComponent* Eyebrows;
	
	UPROPERTY(VisibleInstanceOnly, Category = "Equipment")
	AItem* OverlappingItem;
	
	UPROPERTY(VisibleInstanceOnly, Category = "Equipment")
	AWeapon* EquippedWeapon;
	
	/*************************
	 *    Input Callbacks    *
	 ************************/ 
	
	void MoveTriggered(const FInputActionValue& Value);
	void LookTriggered(const FInputActionValue& Value);
	void InteractStarted();
	void AttackStarted();
	
	/*********************************
	 *  Animation-related Functions  *
	 ********************************/

	void PlayAttackMontage() const;
	void PlayEquipMontage(FName SectionName) const;
	
	bool CanAttack() const;
	bool CanArm() const;
	bool CanDisarm() const;
	
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand() const;
	
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToSpine() const;
	
	UFUNCTION(BlueprintCallable)
	void SetEquippedWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled) const;
public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetActionState(EActionState State) { ActionState = State; }
};
