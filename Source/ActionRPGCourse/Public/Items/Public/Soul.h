#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"

UCLASS()
class ACTIONRPGCOURSE_API ASoul : public AItem {
	GENERATED_BODY()

public:
	ASoul();
	virtual void Tick(float DeltaTime) override;
	

protected:
	virtual void BeginPlay() override;
};
