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
	
private:
	UPROPERTY(VisibleInstanceOnly)
	double RunningTime;

	UPROPERTY(EditAnywhere)
	float MovementAmplitude;

	UPROPERTY(EditAnywhere)
	float MovementPeriod;
};
