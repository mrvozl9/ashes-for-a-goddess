// AI-assisted: Claude Code, 11.11.2026, builds the model request from card, lore, scene, state and turns (Phase 4). Reviewed by <pending>.

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

	/** Number of earlier turns sent with each request */
	static constexpr int32 MaxHistoryTurns = 8;

	static FLLMRequest Build(const FCharacterCardRow& Card, const FString& LoreBible, const FSceneCardRow& Scene,
		const FPromptState& State, const TArray<FConversationTurn>& History, const FString& PlayerLine, int32 Chapter);

	/** The system prompt alone (sections 1–4 + JSON contract); public for tests */
	static FString BuildSystemPrompt(const FCharacterCardRow& Card, const FString& LoreBible, const FSceneCardRow& Scene,
		const FPromptState& State, int32 Chapter);

	/** 0–100 → a word the model understands ("wary", "warm", ...) */
	static FString ValueToWord(int32 Value);
};
