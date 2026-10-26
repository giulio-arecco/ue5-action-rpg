#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class UBoxComponent;
class USoundBase;

UCLASS()
class ACTIONRPGCOURSE_API AWeapon : public AItem {
	GENERATED_BODY()

public:
	AWeapon();
	
	virtual void Tick(float DeltaTime) override;
	
	void Equip(USceneComponent* InParent, const FName& InSocket, AActor* NewOwner, APawn* NewInstigator);
	void AttachWeaponToSocket(USceneComponent* InParent, const FName& InSocket) const;
	
	FORCEINLINE void SetWeaponBoxCollisionEnabled(ECollisionEnabled::Type CollisionEnabled) const;
	FORCEINLINE void EmptyActorsToIgnore() { ActorsToIgnore.Empty(); }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Box Trace")
	FVector BoxTraceHalfSize = FVector(5.0f);
	
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void CreateWeaponFields(const FVector& FieldLocation);
	
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> EquipSound;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon Box Trace")
	TObjectPtr<UBoxComponent> WeaponBox;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon Box Trace")
	TObjectPtr<USceneComponent> BoxTraceStart;
	
	UPROPERTY(VisibleAnywhere, Category = "Weapon Box Trace")
	TObjectPtr<USceneComponent> BoxTraceEnd;
	
	UPROPERTY(VisibleInstanceOnly, Category = "Weapon Box Trace")
	TArray<AActor*> ActorsToIgnore;
	
	UPROPERTY(EditAnywhere, Category = "Weapon Box Trace")
	bool bShowBoxDebug = false;
	
	UPROPERTY(EditAnywhere)
	float Damage = 20.0f;
	
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	bool DoBoxTrace(FHitResult& OutHitResult);
	void HitActor(AActor* Actor, const FVector& ImpactPoint, const FVector& ImpactNormal);
};
