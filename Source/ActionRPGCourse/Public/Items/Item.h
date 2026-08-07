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

	UFUNCTION(BlueprintPure)
	float TransformedSin() const;

	UFUNCTION(BlueprintPure)
	float TransformedCos() const;
	
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	double RunningTime;

	FVector StartLocation;
};
