#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8 {
	ECS_Disarmed UMETA(DisplayName = "Disarmed"),
	ECS_Armed UMETA(DisplayName = "Armed"),
};

UENUM(BlueprintType)
enum class ECharacterActionState : uint8 {
	ECAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECAS_ReactingToHit UMETA(DisplayName = "Reacting To Hit"),
	ECAS_Attacking UMETA(DisplayName = "Attacking"),
	ECAS_ArmingOrDisarming UMETA(DisplayNAme = "Arming or Disarming")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8 {
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_InCombat UMETA(DisplayName = "InCombat"),
};

UENUM(BlueprintType)
enum class EEnemyActionState : uint8 {
	EEAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EEAS_StateLocked UMETA(DisplayName = "State Locked"),
};
