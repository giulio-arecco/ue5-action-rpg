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
	
	FORCEINLINE float GetGoldAmount() const { return GoldAmount; }
	
protected:
	virtual void BeginPlay() override;
	virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	
private:
	UPROPERTY(EditAnywhere)
	float RotationRate = 45.0f;
	
	UPROPERTY(EditAnywhere, Category = "Treasure Properties")
	int32 GoldAmount;
};
