#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class ACTIONRPGCOURSE_API AItem : public AActor {
	GENERATED_BODY()
	
public:	
	AItem();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Values")
	float MovementAmplitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Values")
	float MovementPeriod;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sine Values")
	FVector StartLocation;

	UFUNCTION(BlueprintPure)
	float TransformedSin() const;

	UFUNCTION(BlueprintPure)
	float TransformedCos() const;
	
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	double RunningTime;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ItemMesh;

	template<typename T>
	T Avg(T First, T Second) {
		return (First + Second) / 2;
	}
};
