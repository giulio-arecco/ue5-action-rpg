#include "HUD/PlayerHUD.h"

#include "Blueprint/UserWidget.h"
#include "Characters/PlayerCharacter.h"
#include "Components/AttributeComponent.h"
#include "HUD/PlayerViewportWidget.h"

void APlayerHUD::BeginPlay() {
	if (!ensureAlwaysMsgf(PlayerViewportWidgetClass != nullptr, TEXT("[%s] PlayerViewportWidgetClass is null"), *GetName())) return;
	
	Super::BeginPlay();
	
	APlayerController* PlayerController = GetOwningPlayerController();
	if (!ensureAlwaysMsgf(PlayerController != nullptr, TEXT("[%s] PlayerController is null"), *GetName())) return;
	
	PlayerViewportWidget = CreateWidget<UPlayerViewportWidget>(PlayerController, PlayerViewportWidgetClass);
	PlayerViewportWidget->AddToViewport();
	
	const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwningPawn());
	if (!ensureAlwaysMsgf(PlayerCharacter != nullptr, TEXT("[%s] PlayerCharacter is null"), *GetName())) return;
	
	UAttributeComponent* PlayerAttributes = PlayerCharacter->GetAttributeComponent();
	if (!ensureAlwaysMsgf(PlayerAttributes != nullptr, TEXT("[%s] The PlayerCharacter AttributeComponent is null"), *GetName())) return;
	
	// Set initial widget values
	PlayerViewportWidget->SetHealthBarPercent(PlayerAttributes->GetHealthPercent());
	PlayerViewportWidget->SetStaminaBarPercent(PlayerAttributes->GetStaminaPercent());
	PlayerViewportWidget->SetGold(PlayerAttributes->GetGold());
	PlayerViewportWidget->SetSouls(PlayerAttributes->GetSouls());
	
	// Bind callbacks to attribute change events
	PlayerAttributes->OnHealthChanged.AddUObject(PlayerViewportWidget.Get(), &UPlayerViewportWidget::SetHealthBarPercent);
	PlayerAttributes->OnStaminaChanged.AddUObject(PlayerViewportWidget.Get(), &UPlayerViewportWidget::SetStaminaBarPercent);
	PlayerAttributes->OnGoldChanged.AddUObject(PlayerViewportWidget.Get(), &UPlayerViewportWidget::SetGold);
	PlayerAttributes->OnSoulsChanged.AddUObject(PlayerViewportWidget.Get(), &UPlayerViewportWidget::SetSouls);
}
