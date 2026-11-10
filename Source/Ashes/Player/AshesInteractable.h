// AI-assisted: Claude Code, 10.11.2026, interface for things Sahra can interact with (Phase 2). Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AshesInteractable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UAshesInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 *  Anything Sahra can use with E: characters (talk), later hiding spots (hide / leave).
 *  Implement in C++ (override the _Implementation functions) or in a Blueprint (Class Settings > Interfaces).
 */
class ASHES_API IAshesInteractable
{
	GENERATED_BODY()

public:

	/** Prompt text shown while focused, e.g. "E: Talk" */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	FText GetInteractPrompt() const;

	/** False hides the prompt and blocks E (e.g. NPC busy, not available this chapter) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	bool CanInteract(AActor* Interactor) const;

	/** E pressed while this actor is focused */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	void Interact(AActor* Interactor);

	/** Focus gained / lost: show or hide the prompt */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Interaction")
	void SetInteractionFocus(bool bFocused);
};
