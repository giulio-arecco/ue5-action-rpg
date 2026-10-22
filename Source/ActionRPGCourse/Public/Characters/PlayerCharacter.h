#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
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
class ACTIONRPGCOURSE_API APlayerCharacter : public ABaseCharacter {
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:
	virtual void BeginPlay() override;
	
	virtual bool CanAttack() const override;
	virtual void Attack() override;
	
	/*****************************
	 *  Input-related Variables  *
	 ****************************/ 
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	/*********************************
	 *  Animation-related Variables  *
	 ********************************/ 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	FName WeaponHandSocketName = "hand_r_socket";
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	FName WeaponSpineSocketName = "spine_socket";
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Montages")
	TObjectPtr<UAnimMontage> EquipMontage;

private:
	UPROPERTY(VisibleInstanceOnly)
	ECharacterState CharacterState = ECharacterState::ECS_Disarmed;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> ViewCamera;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleAnywhere, Category = "Hair")
	TObjectPtr<UGroomComponent> Hair;

	UPROPERTY(VisibleAnywhere, Category = "Hair")
	TObjectPtr<UGroomComponent> Eyebrows;
	
	UPROPERTY(VisibleInstanceOnly, Category = "Equipment")
	TObjectPtr<AItem> OverlappingItem;
	
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

	void PlayEquipMontage(FName SectionName) const;
	
	bool CanArm() const;
	bool CanDisarm() const;
	
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand() const;
	
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToSpine() const;
public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetActionState(EActionState State) { ActionState = State; }
};
