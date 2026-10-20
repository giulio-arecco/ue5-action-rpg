#include "Breakables/BreakableActor.h"

#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Items/Treasure.h"
#include "Kismet/GameplayStatics.h"

ABreakableActor::ABreakableActor() {
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("BreakableActorGeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GeometryCollection->SetNotifyBreaks(true);
}

void ABreakableActor::BeginPlay() {
	Super::BeginPlay();
	GeometryCollection->OnChaosBreakEvent.AddUniqueDynamic(this, &ABreakableActor::OnBreak);
}

#if WITH_EDITOR
void ABreakableActor::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) {
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	const FName PropertyName = PropertyChangedEvent.GetPropertyName();
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ABreakableActor, TreasureClassArray)) {
		TreasureClassArray.Sort([] (const TSubclassOf<ATreasure>& A, const TSubclassOf<ATreasure>& B) {
			const UClass* ClassA = A.Get();
			const UClass* ClassB = B.Get();
			
			// If either A or B is null, push nulls to the back
			if (ClassA == nullptr || ClassB == nullptr) {
				return ClassA != nullptr;
			}
			
			return ClassA->GetDefaultObject<ATreasure>()->GetGoldAmount() < ClassB->GetDefaultObject<ATreasure>()->GetGoldAmount();
		});
	}
}
#endif

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, const FVector& ImpactNormal) {
	UGameplayStatics::PlaySoundAtLocation(this, BreakSound, ImpactPoint);
}

void ABreakableActor::OnBreak(const FChaosBreakEvent& BreakEvent) {
	// Disable break notifies to avoid responding to subsequent breaks into sub pieces
	GeometryCollection->SetNotifyBreaks(false);
	GeometryCollection->OnChaosBreakEvent.RemoveDynamic(this, &ABreakableActor::OnBreak);
	GeometryCollection->SetCanEverAffectNavigation(false);
	
	SetLifeSpan(LifeSpanAfterBreak);
	
	UWorld* World = GetWorld();
	if (World != nullptr && !TreasureClassArray.IsEmpty()) {
		FVector Location = GetActorLocation();
		Location.Z += 50.0;
		
		const int32 Roll = FMath::RandRange(0, TreasureClassArray.Num() - 1);
		World->SpawnActor<ATreasure>(TreasureClassArray[Roll], Location, FRotator::ZeroRotator);
	}
}

