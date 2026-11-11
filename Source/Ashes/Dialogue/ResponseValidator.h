// AI-assisted: Claude Code, 11.11.2026, checks and clamps every model reply before the game uses it (Phase 4). Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AshesTypes.h"
#include "ResponseValidator.generated.h"

/** What the reply is checked against (built from the card and scene) */
USTRUCT()
struct ASHES_API FAshesValidationContext
{
	GENERATED_BODY()

	TArray<FName> AllowedEmotions;
	FName FallbackEmotion = TEXT("neutral");
	TArray<FRelationshipValueDef> Values;
	TArray<FName> AllowedBeats;
	TArray<FName> AllowedClues;
	TArray<FString> SpoilerKeywords;
	TArray<FString> SafetyWords;
	int32 MaxWords = 80;
	int32 MaxSuggestions = 3;

	static FAshesValidationContext FromCard(const FCharacterCardRow& Card, const FSceneCardRow& Scene);
};

UENUM()
enum class EValidationOutcome : uint8
{
	Ok,              // use Response
	RetryBrokenJson, // ask again once silently, then fallback line
	Regenerate,      // spoiler or unsafe words: ask again once, then fallback line
};

USTRUCT()
struct ASHES_API FAshesValidationResult
{
	GENERATED_BODY()

	EValidationOutcome Outcome = EValidationOutcome::RetryBrokenJson;

	/** Cleaned, clamped reply (valid only when Outcome == Ok) */
	FLLMResponse Response;

	/** What was fixed or rejected, for logs and tests ("emotion 'smug' -> neutral", ...) */
	TArray<FString> Notes;

	bool IsOk() const { return Outcome == EValidationOutcome::Ok; }
};

/**
 *  The model only suggests; this decides (GDD 8.1). Parses the JSON (also inside ``` fences or extra text),
 *  whitelists the emotion (fallback), cuts the reply to 80 words, clamps every delta to the card's per-turn limits,
 *  keeps only beat / clue ids of the current scene, keeps 3 suggestions, flags spoilers and unsafe words.
 */
UCLASS()
class ASHES_API UResponseValidator : public UObject
{
	GENERATED_BODY()

public:

	static FAshesValidationResult Validate(const FString& RawJson, const FAshesValidationContext& Context);

	/** True if the player's line contains a word from the safety list (3 times closes the scene, later phase) */
	static bool IsUnsafeText(const FString& Text, const TArray<FString>& SafetyWords);

	/** Default safety word list (placeholder until the team writes the real list) */
	static TArray<FString> DefaultSafetyWords();
};
