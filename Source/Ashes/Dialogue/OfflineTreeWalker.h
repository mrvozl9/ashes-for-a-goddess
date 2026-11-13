// AI-assisted: Claude Code, 13.11.2026, offline dialogue tree walker (pure logic, no world/UI/LLM). Reviewed by <pending>.
#pragma once

#include "CoreMinimal.h"
#include "AshesTypes.h"

/** What the character answers after Sahra picked a reply */
struct ASHES_API FOfflineStep
{
	bool bValid = false;          // false: no such reply at the current node
	FString Line;                 // character line, may contain [gestures]
	FName Emotion;
	TMap<FName, int32> Changes;   // value changes, e.g. trust +2
	TArray<FName> Beats;
	TArray<FName> Clues;
	bool bStandIn = false;        // the row's rule failed: stand-in fields were used
	bool bEnded = false;          // next node 0 or a node without rows: the conversation is over
	TArray<FString> NextReplies;  // Sahra's replies at the next node (empty when ended)
};

/**
 * Walks one scene of DT_OfflineTrees. Copyable, so tests can branch a walk.
 * A node = all rows of the scene with the same Node id (at most MaxReplies are used).
 */
struct ASHES_API FOfflineTreeWalker
{
	static constexpr int32 MaxReplies = 3;

	/** Keeps the rows of Scene (table order). False when the scene has no rows. */
	bool Start(const TArray<FOfflineNodeRow>& AllRows, FName Scene);

	void Reset();

	/** Started and not ended */
	bool IsActive() const { return bActive; }

	int32 GetNode() const { return Node; }

	/** PlayerLine of each row of the current node */
	TArray<FString> GetReplies() const;

	/** Index into GetReplies for free text, or -1 (exact match first, then a clear keyword winner) */
	int32 MatchReply(const FString& PlayerText) const;

	/** Applies the reply at Index and moves on. bValid false (state unchanged) when Index or state is wrong. */
	FOfflineStep Pick(int32 Index, const TMap<FName, int32>& Values, const TSet<FName>& KnownClues);

private:
	TArray<const FOfflineNodeRow*> RowsAt(int32 NodeId) const;

	TArray<FOfflineNodeRow> Rows;
	int32 Node = 0;
	bool bActive = false;
};
