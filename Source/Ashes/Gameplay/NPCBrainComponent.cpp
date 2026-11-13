// AI-assisted: Claude Code, 13.11.2026, NPC brain component: hidden relationship values, thresholds and route (Phase 7). Reviewed by <pending>.

#include "NPCBrainComponent.h"
#include "PromptBuilder.h"

UNPCBrainComponent::UNPCBrainComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNPCBrainComponent::InitFromCard(FName InCharacterRow, const FCharacterCardRow& Card, const TArray<FValueThresholdRow>& AllThresholds)
{
	if (bInitialized)
	{
		return;
	}
	bInitialized = true;
	CharacterRow = InCharacterRow;

	for (const FRelationshipValueDef& Def : Card.Values)
	{
		Defs.Add(Def.Name, Def);
		Values.Add(Def.Name, FMath::Clamp(Def.Start, 0, 100));
	}

	for (const FValueThresholdRow& Row : AllThresholds)
	{
		if (Row.Character == InCharacterRow)
		{
			Thresholds.Add(Row);
		}
	}
	Thresholds.StableSort([](const FValueThresholdRow& A, const FValueThresholdRow& B) { return A.AtLeast < B.AtLeast; });

	// Thresholds already met by the start value are unlocked silently
	for (const FValueThresholdRow& Row : Thresholds)
	{
		const int32* Current = Values.Find(Row.Value);
		if (Current && *Current >= Row.AtLeast && (!Row.bRomanceOnly || GetRoute() == EAshesRoute::Romance))
		{
			Unlocked.Add(Row.UnlockId);
		}
	}
}

int32 UNPCBrainComponent::GetValue(FName Name) const
{
	const int32* Found = Values.Find(Name);
	return Found ? *Found : 0;
}

int32 UNPCBrainComponent::ApplyDelta(FName Name, int32 Delta)
{
	int32* Current = Values.Find(Name);
	const FRelationshipValueDef* Def = Defs.Find(Name);
	if (!Current || !Def)
	{
		return 0;
	}

	const int32 Limited = FMath::Clamp(Delta, -Def->MaxDecrease, Def->MaxIncrease);
	*Current = FMath::Clamp(*Current + Limited, 0, 100);
	const int32 New = *Current;

	// Collect first, then broadcast, so listeners may safely call back into the brain
	TArray<const FValueThresholdRow*> Reached;
	for (const FValueThresholdRow& Row : Thresholds)
	{
		// Romance-only rows are looked at on every change: the route can turn romance through another value (affection)
		if ((Row.Value != Name && !Row.bRomanceOnly) || GetValue(Row.Value) < Row.AtLeast || Unlocked.Contains(Row.UnlockId))
		{
			continue;
		}
		if (Row.bRomanceOnly && GetRoute() != EAshesRoute::Romance)
		{
			continue; // stays locked; fires on a later delta once the route is romance
		}
		Unlocked.Add(Row.UnlockId);
		Reached.Add(&Row);
	}
	for (const FValueThresholdRow* Row : Reached)
	{
		OnThresholdReached.Broadcast(Row->UnlockId, Row->HintLine);
	}
	return New;
}

FString UNPCBrainComponent::GetWord(FName Name) const
{
	return UPromptBuilder::ValueToWord(GetValue(Name));
}

EAshesRoute UNPCBrainComponent::GetRoute() const
{
	if (bRouteLocked)
	{
		return LockedRoute;
	}
	return GetValue(TEXT("affection")) >= RomanceAffection ? EAshesRoute::Romance : EAshesRoute::Comrade;
}

void UNPCBrainComponent::LockRoute()
{
	LockedRoute = GetRoute();
	bRouteLocked = true;
}
