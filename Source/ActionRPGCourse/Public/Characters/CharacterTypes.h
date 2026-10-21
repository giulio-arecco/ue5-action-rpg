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

UENUM(BlueprintType)
enum class EEnemyState : uint8 {
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
};
