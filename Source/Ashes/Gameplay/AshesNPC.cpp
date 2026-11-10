// AI-assisted: Claude Code, 10.11.2026, talkable character base with "E: Talk" prompt; placeholder scene start (Phase 2). Reviewed by <pending>.

#include "AshesNPC.h"
#include "AshesCharacter.h"
#include "Ashes.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

#define LOCTEXT_NAMESPACE "AshesNPC"

AAshesNPC::AAshesNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	DisplayName = LOCTEXT("DefaultName", "NPC");
	PromptText = LOCTEXT("TalkPrompt", "E: Talk");

	// Prompt floats above the head, always faces the screen, hidden until focused
	PromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PromptWidget"));
	PromptWidget->SetupAttachment(RootComponent);
	PromptWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f));
	PromptWidget->SetWidgetSpace(EWidgetSpace::Screen);
	PromptWidget->SetDrawAtDesiredSize(true);
	PromptWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PromptWidget->SetHiddenInGame(true);
}

FText AAshesNPC::GetInteractPrompt_Implementation() const
{
	return PromptText;
}

bool AAshesNPC::CanInteract_Implementation(AActor* Interactor) const
{
	return true;
}

void AAshesNPC::Interact_Implementation(AActor* Interactor)
{
	AAshesCharacter* Sahra = Cast<AAshesCharacter>(Interactor);

	// Placeholder until the Scene Director exists: E starts, E again ends
	bPlaceholderSceneActive = !bPlaceholderSceneActive;

	if (bPlaceholderSceneActive)
	{
		UE_LOG(LogAshes, Log, TEXT("Start scene: %s"), *DisplayName.ToString());
		if (Sahra)
		{
			Sahra->SetGameplayInputEnabled(false);
		}
	}
	else
	{
		UE_LOG(LogAshes, Log, TEXT("End scene: %s"), *DisplayName.ToString());
		if (Sahra)
		{
			Sahra->SetGameplayInputEnabled(true);
		}
	}

	// Hide the prompt while the scene runs
	PromptWidget->SetHiddenInGame(bPlaceholderSceneActive);
}

void AAshesNPC::SetInteractionFocus_Implementation(bool bFocused)
{
	if (bFocused)
	{
		UpdatePromptText();
	}
	PromptWidget->SetHiddenInGame(!bFocused || bPlaceholderSceneActive);
}

void AAshesNPC::UpdatePromptText()
{
	// WBP_InteractPrompt has a TextBlock named "PromptText"
	if (UUserWidget* Widget = PromptWidget->GetUserWidgetObject())
	{
		if (UTextBlock* Text = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("PromptText"))))
		{
			Text->SetText(IAshesInteractable::Execute_GetInteractPrompt(this));
		}
	}
}

#undef LOCTEXT_NAMESPACE
