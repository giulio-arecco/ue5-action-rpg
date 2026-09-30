#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

UCLASS()
class ACTIONRPGCOURSE_API ABreakableActor : public AActor, public IHitInterface {
	GENERATED_BODY()

public:
	ABreakableActor();
	virtual void GetHit(const FVector& ImpactPoint, const FVector& ImpactNormal) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent* GeometryCollection;
};
