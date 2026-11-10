// AI-assisted: Claude Code, 10.11.2026, walk/run, gameplay input gate, no jump, camera values (Phase 1); interaction component and E (Phase 2). Reviewed by <pending>.
// Based on the UE 5.8 Third Person template character (Copyright Epic Games, Inc.).

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AshesCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UInteractionComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  Sahra's base character: third person, walk / run, orbit camera, no jump.
 *  BP_Sahra sets the mesh, the anim Blueprint and the input actions.
 */
UCLASS(abstract)
class AAshesCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Finds the character or object Sahra faces and handles E */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInteractionComponent> InteractionComponent;

protected:

	/** Move Input Action (IA_Move) */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action (IA_Look, gamepad / mouse) */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action (template; optional, IA_Look covers the mouse) */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

	/** Run Input Action (IA_Run, hold Left Shift) */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* RunAction;

	/** Interact Input Action (IA_Interact, E) */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* InteractAction;

	/** Walk speed in cm/s (technical_design.md section 3) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0", Units="cm/s"))
	float WalkSpeed = 220.f;

	/** Run speed in cm/s while IA_Run is held */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Movement", meta=(ClampMin="0", Units="cm/s"))
	float RunSpeed = 450.f;

	/** True while the run key is held and gameplay input is on (read by guards) */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Movement")
	bool bIsRunning = false;

	/** False during conversations, cutscenes, the dance and pause */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Input")
	bool bGameplayInputEnabled = true;

public:

	/** Constructor */
	AAshesCharacter();

	/** Turns walking, running and camera look on or off (off in conversations, cutscenes, the dance and pause) */
	UFUNCTION(BlueprintCallable, Category="Input")
	void SetGameplayInputEnabled(bool bEnabled);

	UFUNCTION(BlueprintPure, Category="Input")
	bool IsGameplayInputEnabled() const { return bGameplayInputEnabled; }

	UFUNCTION(BlueprintPure, Category="Movement")
	bool IsRunning() const { return bIsRunning; }

protected:

	virtual void BeginPlay() override;

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Sahra never jumps (GDD 2 Controls) */
	virtual bool CanJumpInternal_Implementation() const override { return false; }

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Run key pressed / released */
	void StartRun();
	void StopRun();

	/** E pressed: talk to / use the focused target (not blocked by SetGameplayInputEnabled, so E can also end a scene) */
	void InteractPressed();

	/** Applies WalkSpeed or RunSpeed to CharacterMovement->MaxWalkSpeed */
	void UpdateMovementSpeed();

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Kept so the template Blueprints still compile; does nothing (no jump in this game) */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Kept so the template Blueprints still compile; does nothing (no jump in this game) */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Returns the InteractionComponent subobject **/
	FORCEINLINE UInteractionComponent* GetInteractionComponent() const { return InteractionComponent; }
};
