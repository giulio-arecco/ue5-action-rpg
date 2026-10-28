#include "HUD/PlayerViewportWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UPlayerViewportWidget::SetHealthBarPercent(float Percent) {
	if (!ensureAlwaysMsgf(HealthProgressBar != nullptr, TEXT("[%s] HealthProgressBar is null"), *GetName())) return;
	HealthProgressBar->SetPercent(Percent);
}

void UPlayerViewportWidget::SetStaminaBarPercent(float Percent) {
	if (!ensureAlwaysMsgf(StaminaProgressBar != nullptr, TEXT("[%s] StaminaProgressBar is null"), *GetName())) return;
	StaminaProgressBar->SetPercent(Percent);
}

void UPlayerViewportWidget::SetGold(int32 GoldAmount) {
	if (!ensureAlwaysMsgf(GoldText != nullptr, TEXT("[%s] GoldText is null"), *GetName())) return;
	GoldText->SetText(FText::FromString(FString::FromInt(GoldAmount)));
}

void UPlayerViewportWidget::SetSouls(int32 SoulsAmount) {
	if (!ensureAlwaysMsgf(SoulsText != nullptr, TEXT("[%s] SoulsText is null"), *GetName())) return;
	SoulsText->SetText(FText::FromString(FString::FromInt(SoulsAmount)));
}
