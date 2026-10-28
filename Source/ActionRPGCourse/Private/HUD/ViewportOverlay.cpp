#include "HUD/ViewportOverlay.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UViewportOverlay::SetHealthBarPercent(float Percent) const {
	checkf(HealthProgressBar != nullptr, TEXT("[%s] HealthProgressBar is null"), *GetName());
	HealthProgressBar->SetPercent(Percent);
}

void UViewportOverlay::SetStaminaBarPercent(float Percent) const {
	checkf(StaminaProgressBar != nullptr, TEXT("[%s] StaminaProgressBar is null"), *GetName());
	StaminaProgressBar->SetPercent(Percent);
}

void UViewportOverlay::SetGoldText(int32 GoldAmount) const {
	checkf(GoldText != nullptr, TEXT("[%s] GoldText is null"), *GetName());
	GoldText->SetText(FText::FromString(FString::FromInt(GoldAmount)));
}

void UViewportOverlay::SetSoulsText(int32 SoulsAmount) const {
	checkf(SoulsText != nullptr, TEXT("[%s] SoulsText is null"), *GetName());
	SoulsText->SetText(FText::FromString(FString::FromInt(SoulsAmount)));
}
