// AI-assisted: Claude Code, 10.11.2026, finds the interactable Sahra faces on a 0.1 s timer and handles E (Phase 2). Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAshesInteractionFocusChanged, AActor*, NewTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAshesInteractionEvent, AActor*, Target);

/**
 *  Finds the nearest IAshesInteractable in front of the owner (sphere overlap every ScanInterval, no Tick),
 *  shows its prompt, and calls Interact on it when TryInteract() runs (E / IA_Interact).
 */
UCLASS(ClassGroup=(Ashes), meta=(BlueprintSpawnableComponent))
class ASHES_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UInteractionComponent();

	/** Detection radius in cm around the owner (technical_design.md section 3) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction", meta=(ClampMin="0", Units="cm"))
	float DetectionRadius = 200.f;

	/** Seconds between scans */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction", meta=(ClampMin="0.02", Units="s"))
	float ScanInterval = 0.1f;

	/** Minimum dot product between the owner's forward vector and the direction to the target (0 = anything in front) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interaction", meta=(ClampMin="-1", ClampMax="1"))
	float MinFacingDot = 0.f;

	/** Fired when the focused interactable changes (nullptr = nothing focused) */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FAshesInteractionFocusChanged OnFocusChanged;

	/** Fired after Interact was called on the focused target */
	UPROPERTY(BlueprintAssignable, Category="Interaction")
	FAshesInteractionEvent OnInteract;

	/** Interacts with the focused target; returns true if there was one */
	UFUNCTION(BlueprintCallable, Category="Interaction")
	bool TryInteract();

	UFUNCTION(BlueprintPure, Category="Interaction")
	AActor* GetCurrentTarget() const { return CurrentTarget.Get(); }

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	void Scan();
	void SetCurrentTarget(AActor* NewTarget);

	TWeakObjectPtr<AActor> CurrentTarget;
	FTimerHandle ScanTimer;
};
