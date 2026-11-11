// AI-assisted: Claude Code, 10.11.2026, talkable character base with "E: Talk" prompt; placeholder scene start (Phase 2); opens the conversation scene (Phase 5, 11.11.2026). Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AshesInteractable.h"
#include "AshesNPC.generated.h"

class UWidgetComponent;

/**
 *  Base for characters Sahra can talk to (BP_Zafir, BP_Vashir, BP_Nura, BP_Idris, BP_CourtNPC).
 *  Shows WBP_InteractPrompt above the head while focused.
 *  Phase 2 placeholder: E logs "Start scene: <name>" and freezes Sahra; E again ends it.
 *  The Scene Director (later phase) replaces the placeholder scene.
 */
UCLASS(abstract)
class ASHES_API AAshesNPC : public ACharacter, public IAshesInteractable
{
	GENERATED_BODY()

public:

	AAshesNPC();

	/** Name used in logs and, later, the conversation screen */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="NPC")
	FText DisplayName;

	/** Prompt text while focused */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="NPC")
	FText PromptText;

	/** Row in DT_Characters (empty = Phase 2 placeholder scene) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="NPC")
	FName CharacterRowName;

	/** Row in DT_Scenes for the conversation started with E */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="NPC")
	FName SceneRowName;

	// IAshesInteractable
	virtual FText GetInteractPrompt_Implementation() const override;
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void SetInteractionFocus_Implementation(bool bFocused) override;

	UFUNCTION(BlueprintPure, Category="NPC")
	bool IsPlaceholderSceneActive() const { return bPlaceholderSceneActive; }

protected:

	/** Screen-space "E: Talk" prompt (widget class set in the Blueprint: WBP_InteractPrompt) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	TObjectPtr<UWidgetComponent> PromptWidget;

private:

	void UpdatePromptText();

	bool bPlaceholderSceneActive = false;
};
