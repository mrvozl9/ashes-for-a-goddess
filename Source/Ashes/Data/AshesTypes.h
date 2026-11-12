// AI-assisted: Claude Code, 11.11.2026, row structs and conversation types for cards, scenes and validated replies (Phase 4). Reviewed by <pending>.

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

	/** Words that must never appear in a reply before their secret's chapter (spoiler check) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card")
	TArray<FString> SpoilerKeywords;

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

// ---------------------------------------------------------------------------------------------
// AI-assisted: Claude Code, 12.11.2026, row structs for the CSV-imported tables (clues, fallback lines,
// offline trees, barks, dance charts, cutscene lines). Reviewed by Baha.
// CSV rule for these rows: the first column is the row name (id); lists are plain text separated by ';'
// (e.g. "sahra_confronts_blade;uneasy_truce"), value changes are "name:amount" pairs ("trust:2;affection:-3").
// AshesData (AshesDataValidator.h) parses and checks them.
// ---------------------------------------------------------------------------------------------

/** Where a clue comes from (technical_design 2.1) */
UENUM(BlueprintType)
enum class EClueSource : uint8
{
	Talk,
	Eavesdrop,
	Do,
	DanceReward
};

/** DT_Clues: one authored clue per row; the row name is the clue id (ch<N>_<who>_<what>) */
USTRUCT(BlueprintType)
struct ASHES_API FClueRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Clue")
	int32 Chapter = 1;

	/** Talk clues: DT_Characters row name. Other sources: free text (e.g. "Guards at the lattice screen") */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Clue")
	FString Holder;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Clue")
	EClueSource Source = EClueSource::Talk;

	/** Shown in the Rumor Journal */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Clue", meta=(MultiLine=true))
	FString JournalText;

	/** Extra suggested reply once the clue is found */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Clue", meta=(MultiLine=true))
	FString TopicText;

	/** Added to the holder's prompt in this chapter (talk clues only) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Clue", meta=(MultiLine=true))
	FString PromptHint;

	/** Optional DT_Clues row that must be found first */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Clue")
	FName RequiresClue;
};

/** DT_FallbackLines: a pre-written line used when the model times out, fails twice or leaks */
USTRUCT(BlueprintType)
struct ASHES_API FFallbackLineRow : public FTableRowBase
{
	GENERATED_BODY()

	/** DT_Characters row name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fallback")
	FName Character;

	/** DT_Scenes row name; empty or "any" = every scene of this character */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fallback")
	FName Scene;

	/** Beat id of the scene; empty or "any" = every turn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fallback")
	FName Beat;

	/** Emotion tag of the character; empty or "any" = every emotion */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fallback")
	FName Emotion;

	/** Optional DT_Clues row this line talks about */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fallback")
	FName Clue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fallback", meta=(MultiLine=true))
	FString Line;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fallback")
	bool bAIAssisted = true;
};

/**
 * DT_OfflineTrees: one row per reply Sahra can pick at a node (the Master Lore "Offline Trees" table, row by row).
 * A node is all rows with the same Scene + Node. NextNode 0 ends the conversation.
 */
USTRUCT(BlueprintType)
struct ASHES_API FOfflineNodeRow : public FTableRowBase
{
	GENERATED_BODY()

	/** DT_Scenes row name (key scenes) or a side-character talk id */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline")
	FName Scene;

	/** DT_Characters row name of the character who answers */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline")
	FName Character;

	/** Chapter for side-character nodes (0 = taken from the scene row) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline")
	int32 Chapter = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline", meta=(ClampMin="1"))
	int32 Node = 1;

	/** What Sahra says (the reply button) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline", meta=(MultiLine=true))
	FString PlayerLine;

	/** What the character answers */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline", meta=(MultiLine=true))
	FString CharacterLine;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline")
	FName Emotion;

	/** "trust:2;affection:3" */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline")
	FString Changes;

	/** Beat ids reached by this reply, ';' separated */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline")
	FString Beats;

	/** Clue ids given by this reply, ';' separated */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline")
	FString Clues;

	/** Node shown next in the same Scene; 0 = the scene ends */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline", meta=(ClampMin="0"))
	int32 NextNode = 0;

	/** Optional rule: this value must be at least RequiresMin, else the stand-in fields are used */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline|Rule")
	FName RequiresValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline|Rule")
	int32 RequiresMin = 0;

	/** Optional rule: this clue must be in the journal, else the stand-in fields are used */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline|Rule")
	FName RequiresClue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline|Rule", meta=(MultiLine=true))
	FString StandInLine;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline|Rule")
	FName StandInEmotion;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline|Rule")
	FString StandInChanges;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline|Rule")
	FString StandInBeats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Offline|Rule")
	int32 StandInNextNode = 0;
};

UENUM(BlueprintType)
enum class EBarkKind : uint8
{
	/** One line of an ambient NPC */
	Bark,
	/** An overheard exchange that gives a clue */
	Eavesdrop,
	/** Gossip after a good dance that gives a clue */
	DanceGossip,
	/** A fixed scripted moment (e.g. the king's servant) */
	Fixed
};

/** DT_Barks: ambient barks, eavesdrops, dance gossip and fixed moments */
USTRUCT(BlueprintType)
struct ASHES_API FBarkRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Bark")
	EBarkKind Kind = EBarkKind::Bark;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Bark")
	int32 Chapter = 1;

	/** Ambient NPC id, or the speakers of an exchange (free text) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Bark")
	FString Speaker;

	/** The line; an exchange keeps its speakers inside the text */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Bark", meta=(MultiLine=true))
	FString Line;

	/** Optional DT_Clues row given by this row */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Bark")
	FName Clue;
};

/** DT_DanceCharts: one night's song and notes */
USTRUCT(BlueprintType)
struct ASHES_API FDanceChartRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dance", meta=(ClampMin="1"))
	int32 Night = 1;

	/** MetaSound or Sound Wave; may stay empty until Tala delivers the song */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dance")
	FSoftObjectPath Song;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dance", meta=(ClampMin="1"))
	float BPM = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dance", meta=(ClampMin="0.1"))
	float TempoMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dance", meta=(ClampMin="1"))
	float LengthSeconds = 30.f;

	/** "beat:lane" pairs, ';' separated; lane 1-4 (e.g. "4:1;5:3;6.5:2") */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dance", meta=(MultiLine=true))
	FString Notes;

	/** Optional DT_Clues row given for a great dance */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dance")
	FName RewardClue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dance")
	bool bFixedOutcome = false;
};

/** DT_Cutscenes: one line of a 2D cutscene, an ember caption or a piece of the fallback confession */
USTRUCT(BlueprintType)
struct ASHES_API FCutsceneLineRow : public FTableRowBase
{
	GENERATED_BODY()

	/** Prologue, Epilogue, Ember, Confession ... */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cutscene")
	FName Part;

	/** Play order inside the part */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cutscene")
	int32 Order = 0;

	/** 2D background id (texture name without the T_ prefix); empty keeps the last one */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cutscene")
	FName Background;

	/** Empty = narration */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cutscene")
	FString Speaker;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cutscene", meta=(MultiLine=true))
	FString Line;

	/** Empty or "any" = both routes; else "romance" or "comrade" */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Cutscene")
	FName Route;
};
