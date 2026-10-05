#pragma once

#include "CoreMinimal.h"
#include "Item.h"
#include "Treasure.generated.h"

UCLASS()
class ACTIONRPGCOURSE_API ATreasure : public AItem {
	GENERATED_BODY()

public:
	ATreasure();
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Sounds")
	TObjectPtr<USoundBase> PickupSound;
};
