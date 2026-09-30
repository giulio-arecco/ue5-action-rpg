#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "Physics/Experimental/ChaosEventType.h"
#include "BreakableActor.generated.h"


UCLASS()
class ACTIONRPGCOURSE_API ABreakableActor : public AActor, public IHitInterface {
	GENERATED_BODY()

public:
	ABreakableActor();
	virtual void GetHit_Implementation(const FVector& ImpactPoint, const FVector& ImpactNormal) override;

protected:
	UPROPERTY(VisibleAnywhere, BLueprintReadOnly)
	TObjectPtr<UGeometryCollectionComponent> GeometryCollection;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float LifeSpanAfterBreak;
	
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> BreakSound;
	
	UFUNCTION()
	void OnBreak(const FChaosBreakEvent& BreakEvent);
};
