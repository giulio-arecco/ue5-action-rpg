#include "HUD/HealthBarComponent.h"

#include "Components/ProgressBar.h"
#include "HUD/HealthBarWidget.h"

UHealthBarComponent::UHealthBarComponent() {
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthBarComponent::BeginPlay() {
	Super::BeginPlay();
}

void UHealthBarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UHealthBarComponent::CacheHealthBarWidget() {
	if (HealthBarWidget == nullptr) {
		HealthBarWidget = Cast<UHealthBarWidget>(GetUserWidgetObject());
	}
}

void UHealthBarComponent::SetHealthPercent(float Percent) {
	CacheHealthBarWidget();

	if (UProgressBar* HealthBar = HealthBarWidget->HealthBar; HealthBarWidget != nullptr && HealthBar != nullptr) {
		HealthBar->SetPercent(Percent);
	}
}

