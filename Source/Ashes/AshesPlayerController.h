// AI-assisted: Claude Code, 10.11.2026, IMC_Exploration slot and camera pitch limits (Phase 1). Reviewed by <pending>.
// Based on the UE 5.8 Third Person template player controller (Copyright Epic Games, Inc.).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AshesPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class AAshesPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Exploration controls (IMC_Exploration: move, look, run, interact, journal, pause) */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TObjectPtr<UInputMappingContext> ExplorationMappingContext;

	/** Lowest camera pitch in degrees (looking down) */
	UPROPERTY(EditAnywhere, Category="Camera")
	float CameraPitchMin = -60.f;

	/** Highest camera pitch in degrees (looking up) */
	UPROPERTY(EditAnywhere, Category="Camera")
	float CameraPitchMax = 25.f;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;

};
