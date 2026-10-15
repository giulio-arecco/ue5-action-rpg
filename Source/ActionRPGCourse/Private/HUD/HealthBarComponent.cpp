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
	
	if (!ensureMsgf(HealthBarWidget != nullptr, TEXT("HealthBarWidget is null on [%s]. Did you assign the Widget Class in Blueprint?"), *GetOwner()->GetName())) {
		return;
	}

	if (UProgressBar* HealthBar = HealthBarWidget->HealthBar; HealthBar != nullptr) {
		HealthBar->SetPercent(Percent);
	}
}

