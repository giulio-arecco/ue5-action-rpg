
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ViewportOverlay.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class ACTIONRPGCOURSE_API UViewportOverlay : public UUserWidget {
	GENERATED_BODY()
	
public:
	void SetHealthBarPercent(float Percent) const;
	void SetStaminaBarPercent(float Percent) const;
	void SetGoldText(int32 GoldAmount) const;
	void SetSoulsText(int32 SoulsAmount) const;
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaProgressBar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GoldText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SoulsText;
};
