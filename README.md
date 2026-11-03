# Unreal Engine 5 Action RPG

![License](https://img.shields.io/badge/License-MIT-blue.svg)

## Table of Contents
- [Introduction and Architectural Context](#introduction-and-architectural-context)
- [Core Character Architecture and Combat Execution](#core-character-architecture-and-combat-execution)
- [Player Character Systems and Locomotion](#player-character-systems-and-locomotion)
- [Artificial Intelligence and Enemy Combat](#artificial-intelligence-and-enemy-combat)
- [Attribute Component and User Interface](#attribute-component-and-user-interface)
- [Items, Weapons, and Pickups](#items-weapons-and-pickups)
- [Chaos Physics and Destructibles](#chaos-physics-and-destructibles)
- [Engineering Patterns and Defensive Invariants](#engineering-patterns-and-defensive-invariants)
- [License](#license)

## Introduction
This repository contains a third-person Action RPG software architecture engineered in Unreal Engine 5.6 using C++ and Blueprint visual scripting. The system incorporates real-time melee combat, directional hit reactions, FSM-driven enemy AI, procedural foot Inverse Kinematics, Chaos physics destruction, and an event-driven attribute and UI infrastructure.

The project implementation originated from the curricula presented in the [Unreal Engine 5 - The Ultimate Game Developer Course](https://www.udemy.com/course/unreal-engine-5-the-ultimate-game-developer-course/) course by Stephen Ulibarri. However, this codebase is not a direct replication of the instructional repository. Multiple architectural decisions and technical solutions were altered, expanded, and refined beyond the course material to evaluate alternative engineering designs and implement specialized mechanics.

## Core Character Architecture and Combat Execution

### Base Character Implementation
The `ABaseCharacter` class provides foundational logic shared across player avatars and hostile entities. It owns a `UAttributeComponent` subobject, manages weapon lifecycle references, and establishes initial collision filter states.

#### Collision Channel Configuration
In `ABaseCharacter`, collision filtering is explicitly separated between the root capsule and the skeletal mesh:
- Capsule Component: Configured to `ECC_Pawn` with `ECollisionEnabled::QueryAndPhysics`. The capsule ignores `ECC_Visibility` and `ECC_Camera` channels, preventing camera occlusion artifacts and trace interference during raycasts.
- Skeletal Mesh Component: Configured to `ECC_WorldDynamic` with `ECollisionEnabled::QueryOnly`. The mesh ignores all standard channels, overlaps `ECC_WorldDynamic`, and explicitly blocks `ECC_Visibility`. This setup ensures that precision weapon sweeps verify hits against the physical body geometry rather than the bounding capsule.

### Directional Hit Reactions
When an actor implementing `IHitInterface` is struck, `ABaseCharacter` executes `ComputeAngleTo(const FVector& InPoint)` to compute the impact quadrant relative to the actor orientation:

1. Vector Plane Projection: The displacement vector between the impact source and the actor origin is projected onto the horizontal plane defined by the actor up vector:
   `ToPoint = FVector::VectorPlaneProject(InPoint - Location, Up).GetSafeNormal()`
2. Coordinate Derivation: The algorithm calculates both sine and cosine components of the relative angle:
   `Cross = FVector::CrossProduct(Forward, ToPoint)`
   `SinTheta = FVector::DotProduct(Cross, Up)`
   `CosTheta = FVector::DotProduct(Forward, ToPoint)`
3. Four-Quadrant Angle Resolution: The signed planar angle in degrees is derived via:
   `Theta = FMath::RadiansToDegrees(FMath::Atan2(SinTheta, CosTheta))`

#### Numerical Rationale for `Atan2` over `Acos`
The implementation explicitly rejects standard inverse cosine or tangent operations in favor of `Atan2` for three critical numerical reasons:
- Elimination of Division-by-Zero: Standard tangent formulations require `y / x`, triggering floating point exceptions when `CosTheta` reaches zero. `Atan2` inspects both coordinate signs natively without division.
- Immunity to Floating Point Drift: In vector dot products, precision drift can yield values marginally greater than `1.0` (such as `1.0000001`). Passing this value into `Acos` generates `NaN` conditions that invalidate downstream transforms. `Atan2` remains numerically defined across all real coordinate pairs where `(x, y)` is non-zero.
- Polar Numerical Stability: The derivative of `Acos` approaches infinity near 0 degrees (`x = 1.0`) and 180 degrees (`x = -1.0`), magnifying minuscule floating point errors into large angular deviations. `Atan2` exhibits uniform derivative continuity across the entire circle.

#### Quadrant Section Mapping
The computed angle `Theta` maps to four 90-degree animation sections within `HitReactMontage`:
- `FromFront`: `Theta` between `-45.0` degrees and `45.0` degrees.
- `FromRight`: `Theta` between `45.0` degrees and `135.0` degrees.
- `FromLeft`: `Theta` between `-135.0` degrees and `-45.0` degrees.
- `FromBack`: `Theta` greater than or equal to `135.0` degrees, or less than `-135.0` degrees.

### Damage Pipeline and Death Sequence
The combat pipeline operates through standard engine damage functions and custom lifecycle routines:
1. Hit Detection: `AWeapon` executes `DoBoxTrace` and invokes `UGameplayStatics::ApplyDamage`.
2. Damage Ingestion: `TakeDamage` is executed and passes `DamageAmount` to `ReceiveDamage`, updating `UAttributeComponent`.
3. Death Verification: If `IsAlive()` evaluates to `false`, `Die()` initiates cleanup.
4. Resource Teardown:
   - Disables movement on `UCharacterMovementComponent` and clears remaining velocity via `StopMovementImmediately`.
   - Sets collision on `UCapsuleComponent` and `USkeletalMeshComponent` to `ECollisionEnabled::NoCollision`.
   - Clears overlap event generation and disables actor tick.
   - Executes `PlayDeathMontage` with a randomly selected section index.
   - Pauses skeletal mesh animation ticking via `PauseAnimations` to freeze the final frame after montage completion.
   - Detaches from controller and registers a timer with `GetWorldTimerManager()` bound to `AActor::K2_DestroyActor` using `LifeSpanAfterDeath` (10 seconds by default).

## Player Character Systems and Locomotion

### Player Avatar Engineering
The `APlayerCharacter` class inherits from `ABaseCharacter` and implements `IPickupInterface`. It coordinates user inputs, manages grooming assets, enforces combat rules, and binds to viewport user interfaces.

#### Input System Configuration
The character utilizes the Enhanced Input subsystem initialized in `BeginPlay`:
- `DefaultMappingContext` is registered via `UEnhancedInputLocalPlayerSubsystem`.
- Action bindings map to member functions:
  - `IA_Movement` maps to `MoveTriggered`, computing forward and right vectors relative to the controller yaw rotation matrix.
  - `IA_Looking` maps to `LookTriggered`, feeding yaw and pitch inputs to the controller.
  - `IA_Jump` maps to `APlayerCharacter::Jump` and `ACharacter::StopJumping`.
  - `IA_Interact` maps to `InteractStarted`.
  - `IA_Attack` maps to `AttackStarted`.
  - `IA_Dodge` maps to `DodgeStarted`.
- Movement orientation decouples character mesh facing from camera rotation by setting `bOrientRotationToMovement` to `true` with a yaw `RotationRate` of 480.0 degrees per second, while disabling `bUseControllerRotationYaw`, `bUseControllerRotationPitch`, and `bUseControllerRotationRoll`.

#### Groom Subsystem Integration
`APlayerCharacter` incorporates hair simulation using two `UGroomComponent` subobjects:
- Hair: Attached to the skeletal mesh and bound to the `head` socket.
- Eyebrows: Attached to the skeletal mesh and bound to the `head` socket.

### Finite State Machine Specifications
`APlayerCharacter` executes under two state machines:

```
ECharacterState:
  [ECS_Disarmed] <====== (Arm / Disarm Input) ======> [ECS_Armed]

ECharacterActionState:
                 +---> [ECAS_Attacking] --------+
                 |                              |
  [ECAS_Unoccupied] -> [ECAS_Dodging] ----------+-> [ECAS_Unoccupied]
        |        |                              |
        |        +---> [ECAS_ArmingOrDisarming] +
        |        |                              |
        |        +---> [ECAS_ReactingToHit] ----+
        |
        +------------> [ECAS_Dead]
```

#### State Definitions and Invariants
- `ECharacterState`:
  - `ECS_Disarmed`: Weapon attached to spine socket. Attacks are prohibited.
  - `ECS_Armed`: Weapon attached to right hand socket. Attacks and weapon box traces are permitted.
- `ECharacterActionState`:
  - `ECAS_Unoccupied`: Default state. Locomotion, jumping, attacking, dodging, and interacting are valid.
  - `ECAS_Attacking`: Character executing an attack montage section. Locomotion and new action inputs are ignored.
  - `ECAS_Dodging`: Character executing a directional roll montage. Grants complete invulnerability.
  - `ECAS_ArmingOrDisarming`: Character transitioning weapons between hand and spine sockets.
  - `ECAS_ReactingToHit`: Character playing directional impact montage. State lock cleared via `HitReactMontageEnd`.
  - `ECAS_Dead`: Terminal state. All inputs and damage events are ignored.

### Dodge Mechanics and Invulnerability Frames
Executing a dodge requires meeting these preconditions:
- `ActionState` must equal `ECAS_Unoccupied`.
- `UCharacterMovementComponent::IsFalling` must evaluate to `false`.
- `UAttributeComponent` must possess at least `DodgeStaminaCost` (25 by default).

Upon validation:
1. `UAttributeComponent::ConsumeStamina` deducts 25 stamina units, broadcasting the updated ratio and triggering the regeneration pause timer.
2. `ActionState` transitions to `ECAS_Dodging`.
3. `PlayDodgeMontage` executes `AM_Echo_Dodge`.
4. I-Frames: Inside `TakeDamage` and `GetHit_Implementation`, an immediate guard clause checks if `ActionState == ECharacterActionState::ECAS_Dodging`. If `true`, the functions return `0.0f` and exit immediately, completely bypassing damage deductions, impact sounds, and hit reactions.

### Weapon Attachment and Sheathing
Transitions between armed and disarmed states operate through animation notifies:
- Equipping: When interacting with an unequipped weapon, `EquipWeapon` attaches the weapon to `hand_r_socket`, sets `CharacterState` to `ECS_Armed`, and registers ownership.
- Arming: When armed weapon is sheathed, `Arm` transitions `ActionState` to `ECAS_ArmingOrDisarming` and plays the `Equip` section of `EquipMontage`. An `AnimNotify` calls `AttachWeaponToHand`, transferring the weapon from `spine_socket` to `hand_r_socket`.
- Disarming: `Disarm` transitions `ActionState` to `ECAS_ArmingOrDisarming` and plays the `Unequip` section of `EquipMontage`. An `AnimNotify` calls `AttachWeaponToSpine`, transferring the weapon back to `spine_socket`.

### Animation Runtime and Control Rig Integration
The animation system is orchestrated through `UPlayerAnimInstance` and derived Blueprint assets:
- Property Extraction: `NativeUpdateAnimation` extracts horizontal ground velocity via `UKismetMathLibrary::VSizeXY(Velocity)`, falling status from `CharacterMovement`, and copies `CharacterState` and `ActionState` from the character.
- Linked Animation Layers: `ABP_Echo` utilizes linked animation layers (`ABP_Echo_MainStates`) to isolate state machine blending between armed and disarmed locomotion sets.
- Procedural Foot IK: `ABP_Echo_IK` evaluates two-bone IK and floor traces via Control Rig assets `CR_EchoFootIK` and `IK_Echo`. This offsets leg bones to maintain foot contact on uneven slopes and stairs.

## Artificial Intelligence and Enemy Combat

### Enemy Architecture and Behavioral Pipeline
The `AEnemy` class establishes autonomous enemy behaviors, supporting multiple concrete archetypes including the humanoid Paladin (`BP_Paladin`) and the creature Raptor (`BP_Raptor`).

```
EEnemyState Behavior Loop:

    +-------------------------------------------------------+
    |                                                       |
    v                                                       |
[Patrolling] --- (Target in Chase Radius) ---> [Chasing]    | (Target Lost / Dead)
    ^                                             |         |
    |                                             v         |
    +----- (Target Lost) <----------------- [In Combat] ----+
    |                                             |
    |                                             v
    +---------------------------------------- [Dead]
```

### State Specifications and State Locking
- `EEnemyState`:
  - `EES_Patrolling`: Enemy navigates between predefined world markers at `WalkSpeed`.
  - `EES_Chasing`: Enemy tracks the player character at `RunSpeed`.
  - `EES_InCombat`: Enemy performs melee attacks within `CombatRadius`.
  - `EES_Dead`: Terminal state. AI logic, sensing, and navigation tasks are terminated.
- `EEnemyActionState`:
  - `EEAS_Unoccupied`: Normal state where behavioral transitions can execute.
  - `EEAS_StateLocked`: Applied during attack montages and hit reaction montages. When locked, the state machine update in `Tick` is bypassed, preventing mid-swing or mid-hit state overrides.

### Sensory Perception and Filtering
Enemy perception utilizes `UPawnSensingComponent`:
- `SightRadius` is set to 1250.0 units by default.
- `PeripheralVisionAngle` is set to 60.0 degrees by default (providing a 120-degree forward detection cone).
- The `OnSeePawn` delegate binds to `AEnemy::OnSeePawn`.
- Verification Logic: Rejects pawns that lack the `PlayerCharacter` actor tag, targets that fail `IsValid` checks, or targets whose `UAttributeComponent` reports dead status. Transitions from `EES_Patrolling` to `EES_Chasing` upon successful visual contact.

### Non-Repeating Random Patrol Distribution Algorithm
Patrol waypoints are managed via an instance-editable array of actor pointers (`PatrolTargets`). To prevent unnatural back-and-forth ping-pong movement between adjacent markers, `AEnemy` implements a modulo-based selection algorithm:

1. Initial Selection: If `MoveTarget` is unassigned, a random index from `0` to `N - 1` is selected.
2. Single-Target Handling: If `N == 1`, the single target is returned initially, after which navigation remains at the point.
3. Multi-Target Offset Logic:
   `CurrentIndex = PatrolTargets.IndexOfByKey(CurrentMoveTarget)`
   `RandomOffset = FMath::RandRange(1, NumPatrolTargets - 1)`
   `NextIndex = (CurrentIndex + RandomOffset) % NumPatrolTargets`
   Because `RandomOffset` is constrained between `1` and `N - 1`, `NextIndex` is mathematically guaranteed never to equal `CurrentIndex`, ensuring non-repeating waypoint selection.
4. Asynchronous Arrival Handling: Subscribes a weak lambda to `UPathFollowingComponent::OnRequestFinished`. Upon successful arrival during `EES_Patrolling`, a randomized timer (`TargetReachedMinWait` to `TargetReachedMaxWait`) is scheduled before dispatching the next move command via `AIController->MoveTo`.

### Distance-Squared Optimization
Spatial evaluations in `Tick` and `CanAttack` bypass expensive square root operations:
```cpp
bool AEnemy::IsInTargetRange(const AActor* Target, float Range) const {
    const double DistSq = FVector::DistSquared(GetActorLocation(), Target->GetActorLocation());
    return DistSq < FMath::Square(Range);
}
```

### Motion Warping and Combat Engagement
During melee attacks, enemies synchronize their movement with the player using Unreal Engine Motion Warping:
- Offset Computation: `ComputeMotionWarpingTarget` projects a target location offset from the player along the vector connecting the target to the enemy:
  `TargetLocation = CombatTarget->GetActorLocation()`
  `DirFromTargetToThis = (Location - TargetLocation).GetSafeNormal()`
  `WarpTarget = TargetLocation + (DirFromTargetToThis * WarpTargetOffset)`
  `WarpTargetOffset` (50.0 units by default) prevents the enemy from penetrating the player capsule.
- Root Motion Alignment: `AnimNotifyState` classe `ANS_TrackWarpTarget` updats the motion warping target throughout the attack animation window.
- Attack Cooldown Cycling: `AttackMontageEnd` resets `ActionState` to `EEAS_Unoccupied` and initiates `AttackTimerHandle` with a randomized duration between `AttackTimerMin` and `AttackTimerMax`, after which `AttackTimerEnd` initiates the next strike if `CanAttack` remains valid.

### Deferred Soul Spawning on Elimination
When an enemy is eliminated, `AEnemy::SpawnSoul` executes a two-phase deferred actor instantiation:
1. Spawning is initiated via `GetWorld()->SpawnActorDeferred<ASoul>(SoulClass, SpawnTransform)`.
2. The spawned soul is initialized with the enemy soul balance: `SpawnedSoul->SetNumberOfSouls(AttributeComponent->GetSouls())`.
3. Spawning completes via `SpawnedSoul->FinishSpawning(SpawnTransform)`.
This deferred pattern guarantees that `ASoul` has a valid number of souls before its `BeginPlay` initializes particle effects and collisions.

## Attribute Component and User Interface

### Attribute Component Architecture
The `UAttributeComponent` class provides an isolated container for actor vital statistics. It manages:
- `Health` and `MaxHealth`: Health pool clamp between `0.0` and `MaxHealth` (`100.0f`).
- `Stamina` and `MaxStamina`: Resource pool for combat maneuvers (`100.0f`).
- `StaminaRegenRate`: Linear regeneration rate per second.
- `StaminaRegenPause`: Delay duration following stamina consumption before regeneration begins.
- `Gold` and `Souls`: Integer progression currencies.

#### Multicast Delegate System
`UAttributeComponent` declares four C++ multicast delegates:
- `FHealthChanged`: Broadcasts updated health percentage (`float`).
- `FStaminaChanged`: Broadcasts updated stamina percentage (`float`).
- `FGoldChanged`: Broadcasts updated gold total (`int32`).
- `FSoulsChanged`: Broadcasts updated souls total (`int32`).

#### Stamina Regeneration
Stamina consumption invokes `ConsumeStamina`, which deducts the requested amount, broadcasts the new percentage, and starts a non-looping timer:
`World->GetTimerManager().SetTimer(StaminaRegenPauseTimerHandle, StaminaRegenPause, false)`
Inside `TickComponent`, regeneration proceeds strictly when preconditions are satisfied:
```cpp
if (StaminaRegenRate > 0.0f && Stamina < MaxStamina) {
    const UWorld* World = GetWorld();
    if (!ensureMsgf(World != nullptr, TEXT("[%s] World is null in UAttributeComponent::TickComponent"), *GetName())) return;
    if (World->GetTimerManager().IsTimerActive(StaminaRegenPauseTimerHandle)) return;

    AddStamina(StaminaRegenRate * DeltaTime);
}
```

### User Interface Subsystems

#### Viewport Overlay HUD
The player HUD is managed by `APlayerHUD` and instantiated as `WBP_ViewportOverlay` (derived from `UPlayerViewportWidget`):
1. In `APlayerHUD::BeginPlay`, the widget is created and attached to the viewport via `AddToViewport`.
2. The HUD accesses `APlayerCharacter` and retrieves `UAttributeComponent`.
3. Initial progress bar percentages and currency text blocks are populated.
4. Delegate subscriptions are registered using `AddUObject`:
   - `OnHealthChanged` binds to `UPlayerViewportWidget::SetHealthBarPercent`.
   - `OnStaminaChanged` binds to `UPlayerViewportWidget::SetStaminaBarPercent`.
   - `OnGoldChanged` binds to `UPlayerViewportWidget::SetGold`.
   - `OnSoulsChanged` binds to `UPlayerViewportWidget::SetSouls`.

#### World-Space Floating Enemy Health Bars
Enemy health bars are managed via `UHealthBarComponent`, a specialized `UWidgetComponent` subclass:
- Caching: `CacheHealthBarWidget` casts `GetUserWidgetObject` to `UHealthBarWidget` on demand.
- Event Binding: In `AEnemy::BeginPlay`, `SetHealthPercent` is bound to `UAttributeComponent::OnHealthChanged`.
- Visibility Management: The widget is set to hidden during patrol states, made visible upon taking damage or entering chase/combat, and hidden permanently upon death.

## Items, Weapons, and Pickups

### Item Base Architecture
The `AItem` class serves as the abstract root for world interactables. It consists of:
- `USceneComponent` (`ItemRoot`): Root transform component.
- `UStaticMeshComponent` (`ItemMesh`): Visual geometry configured to `ECollisionEnabled::NoCollision`.
- `USphereComponent` (`Sphere`): Proximity trigger configured to `QueryOnly`, ignoring all channels except `ECC_Pawn`, which is set to `ECR_Overlap`.
- `UNiagaraComponent` (`ItemParticles`): Ambient particle emitter.

### Weapon Blade Tracing
The `AWeapon` class extends `AItem` to implement melee combat.

#### Socket Attachment and State Transition
`Equip` snaps the weapon root to the character skeletal mesh socket:
```cpp
const FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
ItemRoot->AttachToComponent(InParent, TransformRules, InSocket);
```
Upon attachment, the proximity sphere collision is disabled, `ItemState` transitions to `EIS_Equipped`, and ownership is assigned to the character.

#### Sweep Detection (`DoBoxTrace`)
Weapon hits are detected using a box sweep:
1. Box Collision Window: `AnimNotifies` in attack montages toggle `UBoxComponent` collision via `SetWeaponBoxCollisionEnabled`.
2. Blade Extents Sweep: when an overlap begins, `DoBoxTrace` executes `UKismetSystemLibrary::BoxTraceSingle` between `BoxTraceStart` and `BoxTraceEnd` scene components positioned along the weapon blade.
3. Hit Filtering and Invariants:
   - Self-Hit Prevention: the weapon owner is added to `ActorsToIgnore`.
   - Multi-Hit Prevention: hit actors are stored in `ActorsToIgnore` to prevent multiple impacts during a single animation swing.
   - Friendly Fire Suppression: hits are rejected if both the weapon owner and the hit actor possess the `Enemy` tag.
4. Impact Application: if valid, `ApplyDamage` applies damage, and `IHitInterface::Execute_GetHit` dispatches directional impact reactions.
5. Physics Disruption: `CreateWeaponFields` is invoked with the impact point to trigger Chaos destruction fields.

### Currency and Soul Pickups
- `ATreasure`: Rotates around its Z-axis via `AddActorWorldRotation(FRotator(0.0, RotationRate * DeltaTime, 0.0))`. Overlapping `APlayerCharacter` directly adds `GoldAmount` to `UAttributeComponent`, triggers audio, and self-destructs.
- `ASoul`: Initialized with souls currency via deferred spawning. Overlapping `APlayerCharacter` transfers souls, spawns `NS_SoulPickup` Niagara particles, plays audio, and self-destructs.

## Chaos Physics and Destructibles

### Breakable Actor Engineering
The `ABreakableActor` class integrates Unreal Engine Chaos destruction physics:
- Root Component: `UGeometryCollectionComponent` configured with `SetNotifyBreaks(true)` and `SetGenerateOverlapEvents(true)`. It ignores the `ECC_Camera` channel to prevent camera collisions with fractured shards.
- Sound Generation: Implements `IHitInterface` to play `BreakSound` at the impact location upon weapon contact.

### Chaos Break Handling and Optimization
When fracture occurs, Chaos dispatches an `FChaosBreakEvent` triggering `ABreakableActor::OnBreak`:
1. Immediate Event Teardown:
   `GeometryCollection->SetNotifyBreaks(false);`
   `GeometryCollection->OnChaosBreakEvent.RemoveDynamic(this, &ABreakableActor::OnBreak);`
   Disabling break notifications and removing the delegate immediately prevents secondary fracture fragments from triggering redundant gameplay logic.
2. Navigation Mesh Optimization:
   `GeometryCollection->SetCanEverAffectNavigation(false);`
   Excluding the fractured shards from the navigation mesh avoids expensive dynamic navmesh re-tessellation.
3. Shard Lifecycle Management: `SetLifeSpan(LifeSpanAfterBreak)` schedules actor deletion, clearing debris from memory.
4. Dynamic Drop Spawning: Picks a random subclass from `TreasureClassArray` and spawns it at an elevated location (`Z + 50.0` units).

### Editor Property Sorting
In `ABreakableActor::PostEditChangeProperty`, when the `TreasureClassArray` is edited in the editor, the array is automatically sorted in ascending order based on the Class Default Object gold amount:
```cpp
TreasureClassArray.Sort([] (const TSubclassOf<ATreasure>& A, const TSubclassOf<ATreasure>& B) {
    const UClass* ClassA = A.Get();
    const UClass* ClassB = B.Get();
    if (ClassA == nullptr || ClassB == nullptr) return ClassA != nullptr;
    return ClassA->GetDefaultObject<ATreasure>()->GetGoldAmount() < ClassB->GetDefaultObject<ATreasure>()->GetGoldAmount();
});
```

### Weapon Strain Fields
When `AWeapon` hits an actor, it calls the `BlueprintImplementableEvent` `CreateWeaponFields(ImpactPoint)`. In derived Blueprint assets, this spawns radial strain fields through `BP_FieldSystem`, applying physical fracture forces directly to Geometry Collections.

## Engineering Patterns and Defensive Invariants

### Design Pattern Catalog
The architecture incorporates the following software engineering patterns:
- Observer Pattern: Implemented via non-dynamic multicast delegates in `UAttributeComponent`. Decouples vital gameplay statistics from UI rendering and world-space widgets.
- State Pattern: Implemented through enums (`ECharacterState`/`ECharacterActionState` and `EEnemyState`/`EEnemyActionState`) combined with action-locking guards to govern state transitions.
- Interface Segregation Pattern: Implemented via `IHitInterface` and `IPickupInterface`, enabling combat and interaction dispatching across heterogeneous actor classes without type casting or hard coupling.
- Deferred Instantiation: Utilized in `AEnemy::SpawnSoul` via `SpawnActorDeferred`, allowing configuration of runtime state before actor lifecycle methods execute.

### Defensive Programming and Invariant Enforcement
To prevent runtime crashes in development and production builds, the codebase enforces strict defensive invariants:
- Non-Fatal Assertions: Critical failure points employ `ensureAlwaysMsgf` and `ensureMsgf` containing descriptive diagnostic text and actor names, followed by immediate early returns.
- Weak Pointer Referencing: AI move and combat targets utilize `TWeakObjectPtr<AActor>`. This prevents referencing dangling pointers if targets are destroyed externally during navigation or combat and allow the GC to delete them when they're no longer referenced.

## License

All original C++ code (`Source/`) and Blueprint logic (`Content/_Game/Blueprints/`) are licensed under the [MIT License](LICENSE).
