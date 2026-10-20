#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "Physics/Experimental/ChaosEventType.h"
#include "BreakableActor.generated.h"


class ATreasure;

UCLASS()
class ACTIONRPGCOURSE_API ABreakableActor : public AActor, public IHitInterface {
	GENERATED_BODY()

public:
	ABreakableActor();
	virtual void GetHit_Implementation(const FVector& ImpactPoint, const FVector& ImpactNormal) override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	UPROPERTY(VisibleAnywhere, BLueprintReadOnly)
	TObjectPtr<UGeometryCollectionComponent> GeometryCollection;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Break Properties")
	float LifeSpanAfterBreak = 20.0f;
	
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Break Properties")
	TObjectPtr<USoundBase> BreakSound;
	
	UFUNCTION(Category = "Break Properties")
	void OnBreak(const FChaosBreakEvent& BreakEvent);
	
	UPROPERTY(EditAnywhere, Category = "Break Properties")
	TArray<TSubclassOf<ATreasure>> TreasureClassArray;
};
