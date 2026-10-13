#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UAttributeComponent;

UCLASS()
class ACTIONRPGCOURSE_API AEnemy : public ACharacter, public IHitInterface {
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, const FVector& ImpactNormal) override;
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Animation|Montages")
	TObjectPtr<UAnimMontage> HitReactMontage;
	
	UPROPERTY(EditAnywhere, Category = "Sound")
	TObjectPtr<USoundBase> HitSound;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> HitParticles;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> Attributes;
		
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarComponent;
	
	void PlayHitReactMontage(const FVector& ImpactPoint) const;
	double ComputeAngleToImpact(const FVector& ImpactPoint) const;
};
