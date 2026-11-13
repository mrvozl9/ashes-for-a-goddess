// AI-assisted: Claude Code, 13.11.2026, pure Scene Director state machine (turns, beats, nudge, close). Reviewed by <pending>.
#pragma once

#include "CoreMinimal.h"
#include "AshesTypes.h"
#include "SceneFlow.generated.h"

/** Where a scene is in its life */
UENUM(BlueprintType)
enum class EScenePhase : uint8
{
	Idle,
	Intro,
	FreeTalk,
	Nudge,
	Resolve,
	ForcedClose,
	Caught
};

/** Pure scene state: knows nothing about the world, UI or the LLM. Owned by USceneDirectorSubsystem. */
struct ASHES_API FSceneFlow
{
	static constexpr int32 NudgeTurnsLeft = 2;
	static constexpr int32 StallLimit = 3;
	static constexpr int32 CaughtAt = 100;

	/** Value that ends the scene as Caught at CaughtAt */
	static const FName CaughtValueName;

	/** Keeps the scene and its rules, resets everything; phase = Intro */
	void Start(FName InSceneRow, const FSceneCardRow& InScene, const TArray<FSceneRuleRow>& AllRules);

	/** The player sent a line */
	void BeginTurn();

	/** The line got no real answer (hesitation): give the turn back */
	void RefundTurn();

	bool CanAcceptBeat(FName Beat, const TMap<FName, int32>& Values) const;

	/** Call once per counted character reply, after value changes. Returns beats newly reached. */
	TArray<FName> ApplyReply(const TArray<FName>& ClaimedBeats, const TMap<FName, int32>& Values, bool bTrusted = false);

	/** Marks a scene beat reached (no phase change) */
	void ForceBeat(FName Beat);

	/** Conversation ends before the limit (offline tree ran out) */
	void EndEarly();

	EScenePhase GetPhase() const { return Phase; }
	bool IsOver() const { return Phase == EScenePhase::Resolve || Phase == EScenePhase::ForcedClose || Phase == EScenePhase::Caught; }
	bool WasFull() const { return bFull; }
	int32 GetTurnsTaken() const { return TurnsTaken; }
	int32 GetTurnsLeft() const { return FMath::Max(0, Scene.TurnLimit - TurnsTaken); }
	int32 GetStalledTurns() const { return StalledTurns; }
	const TSet<FName>& GetReachedBeats() const { return Reached; }
	const TArray<FName>& GetForcedOnClose() const { return ForcedOnClose; }

	/** First required, unreached beat whose order rule is met; else first unreached required; else NAME_None */
	FName GetPendingBeat() const;

	/** Steering note for the prompt; empty unless phase is Nudge */
	FString GetNudgeNote() const;

	/** Scene-level ClosingRomance (may be empty) */
	FString GetClosingRomance() const;

	/** Scene-level StockSuggestions as a list (may be empty) */
	TArray<FString> GetStockSuggestions() const;

private:
	const FBeatDef* FindBeat(FName Beat) const;
	const FSceneRuleRow* FindRule(FName Beat) const;
	bool HasRequired() const;
	bool AllRequiredReached() const;
	bool IsOnlyTooEarly(FName Beat, const TMap<FName, int32>& Values) const;
	void CloseAtLimit();
	void RefreshOpenPhase();

	FName SceneRow;
	FSceneCardRow Scene;
	TArray<FSceneRuleRow> Rules;
	TSet<FName> Reached;
	TArray<FName> ForcedOnClose;
	/** Beats claimed before their turn or order rule was met; they count as soon as the rule is met */
	TSet<FName> Deferred;
	EScenePhase Phase = EScenePhase::Idle;
	int32 TurnsTaken = 0;
	int32 StalledTurns = 0;
	bool bFull = false;
};
