#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UHitInterface : public UInterface {
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONRPGCOURSE_API IHitInterface {
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void GetHit(const FVector& ImpactPoint, const FVector& ImpactNormal) = 0;
};
