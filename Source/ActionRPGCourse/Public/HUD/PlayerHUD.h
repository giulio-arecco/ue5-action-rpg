#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class UViewportOverlay;

UCLASS()
class ACTIONRPGCOURSE_API APlayerHUD : public AHUD {
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UViewportOverlay> ViewportOverlayClass;
	
	UPROPERTY()
	TObjectPtr<UViewportOverlay> ViewportOverlay;
};
