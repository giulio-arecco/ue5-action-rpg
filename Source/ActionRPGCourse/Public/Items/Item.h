#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class UNiagaraComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EItemState : uint8 {
	EIS_Hovering UMETA(DisplayName = "Hovering"),
	EIS_Equipped UMETA(DisplayName = "Equipped")
};

UCLASS()
class ACTIONRPGCOURSE_API AItem : public AActor {
	GENERATED_BODY()
	
public:	
	AItem();
	virtual void Tick(float DeltaTime) override;
	
	FORCEINLINE USphereComponent* GetSphere() const { return Sphere; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Values")
	float MovementAmplitude = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Values")
	float MovementPeriod = 2.5f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sine Values")
	FVector StartLocation = FVector::ZeroVector;
	
	UPROPERTY(VisibleInstanceOnly)
	EItemState ItemState = EItemState::EIS_Hovering;
	
	virtual void BeginPlay() override;
	void DeactivatePickupVFX() const;

	UFUNCTION(BlueprintPure)
	float TransformedSin() const;

	UFUNCTION(BlueprintPure)
	float TransformedCos() const;

	UFUNCTION()
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	double RunningTime = 0.0f;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UNiagaraComponent> PickupParticles;

	template<typename T>
	T Avg(T First, T Second) {
		return (First + Second) / 2;
	}
};
