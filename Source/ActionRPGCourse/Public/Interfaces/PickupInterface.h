#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupInterface.generated.h"

class ASoul;
class AItem;

UINTERFACE()
class UPickupInterface : public UInterface {
	GENERATED_BODY()
};

class ACTIONRPGCOURSE_API IPickupInterface {
	GENERATED_BODY()

public:
	virtual void SetOverlappingItem(AItem* Item) = 0;
};
