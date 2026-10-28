#include "HUD/PlayerHUD.h"

#include "Blueprint/UserWidget.h"
#include "HUD/ViewportOverlay.h"

void APlayerHUD::BeginPlay() {
	if (!ensureAlwaysMsgf(ViewportOverlayClass != nullptr, TEXT("[%s] ViewportOverlayClass is null"), *GetName())) return;
	
	Super::BeginPlay();
	
	const UWorld* World = GetWorld();
	checkf(World != nullptr, TEXT("[%s] World is null"), *GetName());
	
	APlayerController* Controller = World->GetFirstPlayerController();
	checkf(Controller != nullptr, TEXT("[%s] First Player Controller is null"), *GetName());
	
	ViewportOverlay = CreateWidget<UViewportOverlay>(Controller, ViewportOverlayClass);
	ViewportOverlay->AddToViewport();
}
