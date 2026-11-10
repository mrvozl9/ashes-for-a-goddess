// AI-assisted: Claude Code, 10.11.2026, IMC_Exploration slot and camera pitch limits (Phase 1). Reviewed by <pending>.
// Based on the UE 5.8 Third Person template player controller (Copyright Epic Games, Inc.).


#include "AshesPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Ashes.h"
#include "Widgets/Input/SVirtualJoystick.h"

void AAshesPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Orbit camera pitch limits (technical_design.md section 3)
	if (PlayerCameraManager)
	{
		PlayerCameraManager->ViewPitchMin = CameraPitchMin;
		PlayerCameraManager->ViewPitchMax = CameraPitchMax;
	}

	// only spawn touch controls on local player controllers
	if (IsLocalPlayerController() && ShouldUseTouchControls())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogAshes, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void AAshesPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			if (ExplorationMappingContext)
			{
				Subsystem->AddMappingContext(ExplorationMappingContext, 0);
			}

			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

bool AAshesPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
