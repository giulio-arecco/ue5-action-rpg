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
	FVector BoxTraceHalfSize = FVector(5.0f, 5.0f, 5.0f);
	
	virtual void BeginPlay() override;
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
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
	
	UPROPERTY(EditAnywhere)
	float Damage = 20.0f;
};
