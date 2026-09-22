#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;

enum class EItemState : uint8 {
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class ACTIONRPGCOURSE_API AItem : public AActor {
	GENERATED_BODY()
	
public:	
	AItem();
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Values")
	float MovementAmplitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Values")
	float MovementPeriod;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sine Values")
	FVector StartLocation;
	
	EItemState ItemState;
	
	virtual void BeginPlay() override;

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
	double RunningTime;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

	template<typename T>
	T Avg(T First, T Second) {
		return (First + Second) / 2;
	}
};
