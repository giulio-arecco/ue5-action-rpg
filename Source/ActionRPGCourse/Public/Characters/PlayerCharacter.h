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
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, const FVector& ImpactNormal) override;
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetActionState(ECharacterActionState State) { ActionState = State; }
	
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE ECharacterActionState GetActionState() const { return ActionState; }
	
protected:
	virtual bool CanAttack() const override;
	virtual void Attack() override;
	virtual void HitReactMontageEnd() override;
	
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
	ECharacterActionState ActionState = ECharacterActionState::ECAS_Unoccupied;
	
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
	
	bool CanArm() const;
	bool CanDisarm() const;
	void EquipWeapon(AWeapon* OverlappingWeapon);
	
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
	void Arm();
	void Disarm();
	
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand() const;
	
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToSpine() const;
};
