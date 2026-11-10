// AI-assisted: Claude Code, 10.11.2026, walk/run, gameplay input gate, no jump, camera values (Phase 1). Reviewed by <pending>.
// Based on the UE 5.8 Third Person template character (Copyright Epic Games, Inc.).

#include "AshesCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Ashes.h"

AAshesCharacter::AAshesCharacter()
{
	// Nothing here needs to run every frame
	PrimaryActorTick.bCanEverTick = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement (walk 220 cm/s, run 450 cm/s, no jump)
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->bOrientRotationToMovement = true;
	Movement->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	Movement->MaxWalkSpeed = WalkSpeed;
	Movement->MinAnalogWalkSpeed = 20.f;
	Movement->BrakingDecelerationWalking = 2000.f;
	Movement->BrakingDecelerationFalling = 1500.0f;
	Movement->JumpZVelocity = 0.f;
	Movement->GetNavAgentPropertiesRef().bCanJump = false;

	// Orbit camera over Sahra's right shoulder (values recorded in technical_design.md section 3)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 350.0f;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 60.f);
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 12.f;
	CameraBoom->ProbeSize = 12.f;

	// Follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->SetFieldOfView(80.f);

	// Note: the skeletal mesh and anim Blueprint are set in BP_Sahra (no content references in C++)
}

void AAshesCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Blueprint values for WalkSpeed / RunSpeed are known now
	UpdateMovementSpeed();
}

void AAshesCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAshesCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAshesCharacter::Look);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AAshesCharacter::Look);

		// Running (hold)
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &AAshesCharacter::StartRun);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AAshesCharacter::StopRun);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Canceled, this, &AAshesCharacter::StopRun);
	}
	else
	{
		UE_LOG(LogAshes, Error, TEXT("'%s' Failed to find an Enhanced Input component! This character needs Enhanced Input."), *GetNameSafe(this));
	}
}

void AAshesCharacter::SetGameplayInputEnabled(bool bEnabled)
{
	if (bGameplayInputEnabled == bEnabled)
	{
		return;
	}

	bGameplayInputEnabled = bEnabled;

	if (!bEnabled)
	{
		// Stop right away: no sliding into a conversation or the dance
		bIsRunning = false;
		UpdateMovementSpeed();
		ConsumeMovementInputVector();
		GetCharacterMovement()->StopMovementImmediately();
	}
}

void AAshesCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AAshesCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AAshesCharacter::StartRun()
{
	if (!bGameplayInputEnabled)
	{
		return;
	}

	bIsRunning = true;
	UpdateMovementSpeed();
}

void AAshesCharacter::StopRun()
{
	bIsRunning = false;
	UpdateMovementSpeed();
}

void AAshesCharacter::UpdateMovementSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = bIsRunning ? RunSpeed : WalkSpeed;
}

void AAshesCharacter::DoMove(float Right, float Forward)
{
	if (!bGameplayInputEnabled || GetController() == nullptr)
	{
		return;
	}

	// find out which way is forward
	const FRotator Rotation = GetController()->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, Forward);
	AddMovementInput(RightDirection, Right);
}

void AAshesCharacter::DoLook(float Yaw, float Pitch)
{
	if (!bGameplayInputEnabled || GetController() == nullptr)
	{
		return;
	}

	AddControllerYawInput(Yaw);
	AddControllerPitchInput(Pitch);
}

void AAshesCharacter::DoJumpStart()
{
	// No jump in Ashes for a Goddess
}

void AAshesCharacter::DoJumpEnd()
{
	// No jump in Ashes for a Goddess
}
