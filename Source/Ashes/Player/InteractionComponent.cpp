// AI-assisted: Claude Code, 10.11.2026, finds the interactable Sahra faces on a 0.1 s timer and handles E (Phase 2). Reviewed by <pending>.

#include "InteractionComponent.h"
#include "AshesInteractable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/World.h"
#include "TimerManager.h"

UInteractionComponent::UInteractionComponent()
{
	// Scans run on a timer, never on Tick
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(ScanTimer, this, &UInteractionComponent::Scan, ScanInterval, true);
	}
}

void UInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(ScanTimer);
	}
	SetCurrentTarget(nullptr);

	Super::EndPlay(EndPlayReason);
}

void UInteractionComponent::Scan()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	const FVector Origin = Owner->GetActorLocation();
	const FVector Forward = Owner->GetActorForwardVector().GetSafeNormal2D();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TArray<AActor*> Overlaps;
	UKismetSystemLibrary::SphereOverlapActors(this, Origin, DetectionRadius, ObjectTypes, nullptr, { Owner }, Overlaps);

	AActor* Best = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	for (AActor* Candidate : Overlaps)
	{
		if (!Candidate || !Candidate->Implements<UAshesInteractable>())
		{
			continue;
		}
		if (!IAshesInteractable::Execute_CanInteract(Candidate, Owner))
		{
			continue;
		}

		const FVector ToCandidate = Candidate->GetActorLocation() - Origin;
		if (FVector::DotProduct(Forward, ToCandidate.GetSafeNormal2D()) < MinFacingDot)
		{
			continue;
		}

		const float DistSq = ToCandidate.SizeSquared2D();
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			Best = Candidate;
		}
	}

	SetCurrentTarget(Best);
}

void UInteractionComponent::SetCurrentTarget(AActor* NewTarget)
{
	AActor* OldTarget = CurrentTarget.Get();
	if (OldTarget == NewTarget)
	{
		return;
	}

	if (OldTarget && OldTarget->Implements<UAshesInteractable>())
	{
		IAshesInteractable::Execute_SetInteractionFocus(OldTarget, false);
	}

	CurrentTarget = NewTarget;

	if (NewTarget)
	{
		IAshesInteractable::Execute_SetInteractionFocus(NewTarget, true);
	}

	OnFocusChanged.Broadcast(NewTarget);
}

bool UInteractionComponent::TryInteract()
{
	AActor* Target = CurrentTarget.Get();
	AActor* Owner = GetOwner();
	if (!Target || !Owner || !Target->Implements<UAshesInteractable>())
	{
		return false;
	}
	if (!IAshesInteractable::Execute_CanInteract(Target, Owner))
	{
		return false;
	}

	IAshesInteractable::Execute_Interact(Target, Owner);
	OnInteract.Broadcast(Target);
	return true;
}
