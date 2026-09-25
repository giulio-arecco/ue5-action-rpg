#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8 {
	ECS_Disarmed UMETA(DisplayName = "Disarmed"),
	ECS_Armed UMETA(DisplayName = "Armed"),
};

UENUM(BlueprintType)
enum class EActionState : uint8 {
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_ArmingOrDisarming UMETA(DisplayNAme = "Arming or Disarming")
};
