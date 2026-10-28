#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

class UPlayerViewportWidget;

UCLASS()
class ACTIONRPGCOURSE_API APlayerHUD : public AHUD {
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerViewportWidget> PlayerViewportWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UPlayerViewportWidget> PlayerViewportWidget;
};
