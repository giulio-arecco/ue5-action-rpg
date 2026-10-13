#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarComponent.generated.h"

class UHealthBarWidget;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONRPGCOURSE_API UHealthBarComponent : public UWidgetComponent {
	GENERATED_BODY()

public:
	UHealthBarComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void SetHealthPercent(float Percent);

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	TObjectPtr<UHealthBarWidget> HealthBarWidget;
	
	void CacheHealthBarWidget();
};
