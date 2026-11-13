// AI-assisted: Claude Code, 13.11.2026, NPC brain component: hidden relationship values, thresholds and route (Phase 7). Reviewed by <pending>.
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AshesTypes.h"
#include "NPCBrainComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAshesThresholdReached, FName, UnlockId, const FString&, HintLine);

/**
 * Holds the hidden relationship values (0-100) of one character across scenes, plus the thresholds they unlock
 * and the romance/comrade route. The player never sees the numbers (GDD); the UI shows words and hint lines.
 */
UCLASS(ClassGroup=(Ashes), meta=(BlueprintSpawnableComponent))
class ASHES_API UNPCBrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNPCBrainComponent();

	/** Affection at or above this value puts the route on Romance */
	static constexpr int32 RomanceAffection = 60;

	/** Runs only the first time; later calls are ignored so values persist between scenes */
	void InitFromCard(FName InCharacterRow, const FCharacterCardRow& Card, const TArray<FValueThresholdRow>& AllThresholds);

	UFUNCTION(BlueprintPure, Category="NPC|Brain")
	bool IsInitialized() const { return bInitialized; }

	/** 0 if the value is unknown */
	UFUNCTION(BlueprintPure, Category="NPC|Brain")
	int32 GetValue(FName Name) const;

	const TMap<FName, int32>& GetValues() const { return Values; }

	UFUNCTION(BlueprintPure, Category="NPC|Brain")
	bool HasValue(FName Name) const { return Values.Contains(Name); }

	/** Clamps Delta to the value's per-turn limits, applies it, fires thresholds. Unknown name: ignored, returns 0. Returns the new value. */
	UFUNCTION(BlueprintCallable, Category="NPC|Brain")
	int32 ApplyDelta(FName Name, int32 Delta);

	UFUNCTION(BlueprintPure, Category="NPC|Brain")
	bool IsUnlocked(FName UnlockId) const { return Unlocked.Contains(UnlockId); }

	UFUNCTION(BlueprintPure, Category="NPC|Brain")
	TArray<FName> GetUnlocked() const { return Unlocked.Array(); }

	/** The value as a word for the prompt ("wary", "warming", ...) */
	UFUNCTION(BlueprintPure, Category="NPC|Brain")
	FString GetWord(FName Name) const;

	/** The locked route if locked, else Romance when affection >= RomanceAffection, else Comrade */
	UFUNCTION(BlueprintPure, Category="NPC|Brain")
	EAshesRoute GetRoute() const;

	/** Stores the current route (end of Chapter 5) */
	UFUNCTION(BlueprintCallable, Category="NPC|Brain")
	void LockRoute();

	UFUNCTION(BlueprintPure, Category="NPC|Brain")
	bool IsRouteLocked() const { return bRouteLocked; }

	/** Fires when a value reaches a threshold (once per UnlockId) */
	UPROPERTY(BlueprintAssignable, Category="NPC|Brain")
	FAshesThresholdReached OnThresholdReached;

private:
	bool bInitialized = false;
	bool bRouteLocked = false;
	EAshesRoute LockedRoute = EAshesRoute::Comrade;
	FName CharacterRow;
	TMap<FName, FRelationshipValueDef> Defs;
	TMap<FName, int32> Values;
	/** This character's thresholds, ascending by AtLeast */
	TArray<FValueThresholdRow> Thresholds;
	TSet<FName> Unlocked;
};
