// AI-assisted: Claude Code, 11.11.2026, builds the model request from card, lore, scene, state and turns (Phase 4); state in the newest message and history in blocks for the prompt cache (Phase 6 fix 1, 13.11.2026). Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AshesTypes.h"
#include "LLMTypes.h"
#include "PromptBuilder.generated.h"

/**
 *  Prompt order (GDD 8.1): (1) character card, (2) lore bible, (3) scene card, (4) state as words + memories,
 *  (5) conversation: last 8 turns and the player's new line. Then the JSON reply contract.
 *  Secrets are only included from the chapter where they may be revealed.
 */
UCLASS()
class ASHES_API UPromptBuilder : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** Most earlier turns sent with each request */
	static constexpr int32 MaxHistoryTurns = 8;

	/** The history window moves by this many turns at once (keeps the start of the prompt cached) */
	static constexpr int32 HistoryBlock = 4;

	static FLLMRequest Build(const FCharacterCardRow& Card, const FString& LoreBible, const FSceneCardRow& Scene,
		const FPromptState& State, const TArray<FConversationTurn>& History, const FString& PlayerLine, int32 Chapter);

	/** The system prompt alone (sections 1–3 + JSON contract); it does not change during a scene. State is unused here
	 *  since 13.11.2026 (it goes into the newest message, see BuildStateText); kept in the signature for callers. Public for tests. */
	static FString BuildSystemPrompt(const FCharacterCardRow& Card, const FString& LoreBible, const FSceneCardRow& Scene,
		const FPromptState& State, int32 Chapter);

	/** Values as words, memories, clues and clue hints in one line (the note put before the player's newest line) */
	static FString BuildStateText(const FCharacterCardRow& Card, const FPromptState& State);

	/** Index of the first history turn sent, for a history of HistoryNum turns (public for tests) */
	static int32 GetHistoryStart(int32 HistoryNum);

	/** 0–100 → a word the model understands ("wary", "warm", ...) */
	static FString ValueToWord(int32 Value);
};
