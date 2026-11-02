#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "InputActionValue.h"
#include "Interfaces/PickupInterface.h"
#include "PlayerCharacter.generated.h"

class UPlayerViewportWidget;
class APlayerHUD;
class AWeapon;
class AItem;
class UGroomComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class UAnimMontage;

UCLASS()
class ACTIONRPGCOURSE_API APlayerCharacter : public ABaseCharacter, public IPickupInterface {
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const AActor* Hitter, const FVector& ImpactPoint, const FVector& ImpactNormal) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Jump() override;
	
	FORCEINLINE virtual void SetOverlappingItem(AItem* Item) override { OverlappingItem = Item; }
	FORCEINLINE ECharacterActionState GetActionState() const { return ActionState; }
	FORCEINLINE void SetActionState(ECharacterActionState State) { ActionState = State; }
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE UAttributeComponent* GetAttributeComponent() const { return AttributeComponent; };
	
protected:
	virtual void Die() override;
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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DodgeAction;

	/*********************************
	 *  Animation-related Variables  *
	 ********************************/ 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	FName WeaponHandSocketName = "hand_r_socket";
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	FName WeaponSpineSocketName = "spine_socket";
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation|Montages")
	TObjectPtr<UAnimMontage> EquipMontage;
	
	UPROPERTY(EditAnywhere, Category = "Animation|Montages")
	TObjectPtr<UAnimMontage> DodgeMontage;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
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
	void DodgeStarted();
	
	/*********************************
	 *  Animation-related Functions  *
	 ********************************/
	
	void PlayArmMontage(FName SectionName) const;
	void PlayDodgeMontage() const;
	void Arm();
	void Disarm();
	
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand() const;
	
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToSpine() const;
};
