// AI-assisted: Claude Code, 11.11.2026, row structs and conversation types for cards, scenes and validated replies (Phase 4); spoiler unlock chapters, clue and fallback-line rows (12.11.2026). Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "AshesTypes.generated.h"

/** One hidden relationship value of a character (Trust, Affection, Favor, Suspicion) */
USTRUCT(BlueprintType)
struct ASHES_API FRelationshipValueDef
{
	GENERATED_BODY()

	/** Lower-case name; the model's JSON key is "<name>_delta" (e.g. trust_delta) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Relationship")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Relationship", meta=(ClampMin="0", ClampMax="100"))
	int32 Start = 0;

	/** Largest change per turn upwards (GDD: Zafir ±8) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Relationship", meta=(ClampMin="0"))
	int32 MaxIncrease = 8;

	/** Largest change per turn downwards, as a positive number */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Relationship", meta=(ClampMin="0"))
	int32 MaxDecrease = 8;
};

/** A secret the character may reveal from this chapter on (never earlier: GDD 8.1 spoiler rule) */
USTRUCT(BlueprintType)
struct ASHES_API FChapterSecret
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card")
	int32 Chapter = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card", meta=(MultiLine=true))
	FString Text;
};

/** A word the model must not say before its chapter (spoiler check) */
USTRUCT(BlueprintType)
struct ASHES_API FSpoilerKeyword
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card")
	FString Keyword;

	/** Allowed from this chapter on; 0 = never allowed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card")
	int32 UnlockChapter = 0;
};

/** DT_Characters: one hand-written character card per row (Tuana writes them; never AI-generated) */
USTRUCT(BlueprintType)
struct ASHES_API FCharacterCardRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card")
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card", meta=(MultiLine=true))
	FString Voice;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card", meta=(MultiLine=true))
	FString Wants;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card", meta=(MultiLine=true))
	FString Fears;

	/** How they talk, incl. when they turn cold or angry */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card", meta=(MultiLine=true))
	FString SpeechRules;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card", meta=(MultiLine=true))
	FString Knows;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card", meta=(MultiLine=true))
	FString MustNotSay;

	/** Added to the prompt only from their chapter on */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card")
	TArray<FChapterSecret> Secrets;

	/** Allowed emotion tags, lower case (Zafir: neutral, teasing, tender, jealous, cold, angry) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card")
	TArray<FName> Emotions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card")
	FName FallbackEmotion = TEXT("neutral");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card")
	int32 TurnLimit = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card")
	TArray<FRelationshipValueDef> Values;

	/** Words that must not appear in a reply before their unlock chapter (spoiler check) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card")
	TArray<FSpoilerKeyword> SpoilerKeywords;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card")
	bool bUsesLLM = true;
};

/** A scene goal the model can mark as reached */
USTRUCT(BlueprintType)
struct ASHES_API FBeatDef
{
	GENERATED_BODY()

	/** Id the model must use in "beats", e.g. zafir_reveals_target */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scene")
	FName Id;

	/** When it counts as reached (shown to the model) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scene", meta=(MultiLine=true))
	FString Rule;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scene")
	bool bRequired = true;
};

/** DT_Scenes: one hand-written scene card per row */
USTRUCT(BlueprintType)
struct ASHES_API FSceneCardRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scene")
	int32 Chapter = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scene")
	FString Place;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scene")
	FString Time;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scene", meta=(MultiLine=true))
	FString GoalText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scene")
	TArray<FBeatDef> Beats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scene")
	int32 TurnLimit = 10;

	/** DT_Characters row names */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scene")
	TArray<FName> Characters;

	/** Authored first line of the scene */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scene", meta=(MultiLine=true))
	FString Opener;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scene", meta=(MultiLine=true))
	FString ClosingFull;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scene", meta=(MultiLine=true))
	FString ClosingWeaker;
};

/** DA_LoreBible: the ~600-word hand-written world text; nothing outside it exists for the characters */
UCLASS(BlueprintType)
class ASHES_API UAshesLoreBible : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Lore", meta=(MultiLine=true))
	FString Text;
};

/** One line in the conversation history */
USTRUCT(BlueprintType)
struct ASHES_API FConversationTurn
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category="Conversation")
	bool bFromPlayer = true;

	UPROPERTY(BlueprintReadWrite, Category="Conversation")
	FString Text;
};

/** Current state shown to the model as words (values) and short memories */
USTRUCT(BlueprintType)
struct ASHES_API FPromptState
{
	GENERATED_BODY()

	/** Current value per relationship name (0–100) */
	UPROPERTY(BlueprintReadWrite, Category="Conversation")
	TMap<FName, int32> Values;

	/** Up to 6 memories (GDD 4.3) */
	UPROPERTY(BlueprintReadWrite, Category="Conversation")
	TArray<FString> Memories;

	/** Clues Sahra has found that this character may react to */
	UPROPERTY(BlueprintReadWrite, Category="Conversation")
	TArray<FString> KnownClues;

	/** Prompt hints of the talk clues this character holds in this chapter (DT_Clues) */
	UPROPERTY(BlueprintReadWrite, Category="Conversation")
	TArray<FString> ClueHints;
};

/** A reply after validation: safe to show and apply */
USTRUCT(BlueprintType)
struct ASHES_API FLLMResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="Conversation")
	FString Reply;

	UPROPERTY(BlueprintReadOnly, Category="Conversation")
	FName Emotion;

	UPROPERTY(BlueprintReadOnly, Category="Conversation")
	FString Gesture;

	/** Clamped change per relationship value (trust, affection, favor, suspicion) */
	UPROPERTY(BlueprintReadOnly, Category="Conversation")
	TMap<FName, int32> Deltas;

	/** Only beat ids of the current scene */
	UPROPERTY(BlueprintReadOnly, Category="Conversation")
	TArray<FName> Beats;

	/** Up to 3 suggested replies for Sahra */
	UPROPERTY(BlueprintReadOnly, Category="Conversation")
	TArray<FString> Suggestions;

	UPROPERTY(BlueprintReadOnly, Category="Conversation")
	bool bOffTopic = false;

	UPROPERTY(BlueprintReadOnly, Category="Conversation")
	TArray<FName> Clues;
};

/** DT_Clues: one clue per row (row name = clue id, e.g. ch1_nura_disappears) */
USTRUCT(BlueprintType)
struct ASHES_API FClueRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Clue")
	int32 Chapter = 1;

	/** Who holds it: a character's DisplayName, or e.g. "Guards at the lattice screen" */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Clue")
	FString Holder;

	/** talk / eavesdrop / dance reward (free text, e.g. "talk (after any other clue)") */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Clue")
	FString Source;

	/** Text in the Rumor Journal */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Clue", meta=(MultiLine=true))
	FString JournalText;

	/** Topic it unlocks as a suggested reply with other characters */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Clue")
	FString Topic;

	/** Told to the holder's model so it can reveal the clue (empty for eavesdrop / dance clues) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Clue", meta=(MultiLine=true))
	FString PromptHint;
};

/** DT_FallbackLines: pre-written line used when the model fails (values unchanged) */
USTRUCT(BlueprintType)
struct ASHES_API FFallbackLineRow : public FTableRowBase
{
	GENERATED_BODY()

	/** DT_Scenes row name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fallback")
	FName Scene;

	/** Beat being pushed, or "any" */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fallback")
	FName Beat;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fallback")
	FName Emotion;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fallback", meta=(MultiLine=true))
	FString Line;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fallback")
	bool bAIAssisted = false;
};
