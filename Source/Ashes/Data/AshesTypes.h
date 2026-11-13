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

	/** e.g. "he/him" - the model writes gestures about the character with these pronouns */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Card")
	FString Pronouns;

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

/** Content = an answer for offline mode; Hesitate = a dodge or silent gesture that fits any question (online timeout / broken reply) */
UENUM(BlueprintType)
enum class EFallbackLineType : uint8
{
	Content,
	Hesitate
};

/** DT_FallbackLines: pre-written line used when the model fails (values unchanged) */
USTRUCT(BlueprintType)
struct ASHES_API FFallbackLineRow : public FTableRowBase
{
	GENERATED_BODY()

	/** DT_Characters row name; empty = any character */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fallback")
	FName Character;

	/** Hesitate lines are used when the model is online but too slow or broken; Content lines only offline */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Fallback")
	EFallbackLineType Type = EFallbackLineType::Content;

	/** DT_Scenes row name, or "any" (evasive lines fit every scene) */
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

// ---------------------------------------------------------------------------------------------
// AI-assisted: Claude Code, 12.11.2026, row structs for the offline trees, barks, dance charts and cutscene lines.
// Reviewed by Baha.
// CSV rule for these rows: the first column is the row name (id); lists are plain text separated by ';'
// (e.g. "sahra_confronts_blade;uneasy_truce"), value changes are "name:amount" pairs ("trust:2;affection:-3").
// AshesData (AshesDataValidator.h) parses and checks them.
// ---------------------------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------------------------
// AI-assisted: Claude Code, 13.11.2026, Phase 7 rows: machine-readable beat rules and nudge notes per scene,
// relationship thresholds, and the route. Reviewed by <pending>.
// These tables sit beside DT_Scenes / DT_Characters (which keep the authored texts) so those assets stay as they are.
// ---------------------------------------------------------------------------------------------

/** Which ending route the player is on (GDD: romance when Affection is high enough, else comrade) */
UENUM(BlueprintType)
enum class EAshesRoute : uint8
{
	Comrade,
	Romance
};

/**
 * DT_SceneRules: what the Scene Director checks in C++ for one beat of a scene (the beat's Rule text in DT_Scenes
 * is only read by the model). A row with an empty Beat is the scene-level row (NudgeNote for any beat, ClosingRomance).
 */
USTRUCT(BlueprintType)
struct ASHES_API FSceneRuleRow : public FTableRowBase
{
	GENERATED_BODY()

	/** DT_Scenes row name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SceneRule")
	FName Scene;

	/** Beat id of that scene; empty = the scene-level row */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SceneRule")
	FName Beat;

	/** The beat counts only from this turn on (1 = the first player line); 0 = any turn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SceneRule", meta=(ClampMin="0"))
	int32 MinTurn = 0;

	/** The beat counts only after this beat was reached; empty = no order */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SceneRule")
	FName RequiresBeat;

	/** The beat counts only while this value is at least RequiresMin (e.g. trust 30); empty = no value rule */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SceneRule")
	FName RequiresValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SceneRule")
	int32 RequiresMin = 0;

	/** The Director sets the beat itself at this turn (fixed outcome, e.g. Ch6_VashirTrap); 0 = never */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SceneRule", meta=(ClampMin="0"))
	int32 ForceAtTurn = 0;

	/** Steering note added to the prompt while the Director nudges toward this beat */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SceneRule", meta=(MultiLine=true))
	FString NudgeNote;

	/** Scene-level row only: closing line on the romance route (empty = ClosingFull) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SceneRule", meta=(MultiLine=true))
	FString ClosingRomance;

	/**
	 * Scene-level row only: safe things Sahra can say in this scene, ';' separated. They fill the reply buttons up to 3
	 * when the model sends fewer, and are the choices offline when the scene has no offline tree (Phase 6 fix 4, 13.11.2026).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="SceneRule", meta=(MultiLine=true))
	FString StockSuggestions;
};

/** DT_Thresholds: something unlocks when a relationship value first reaches AtLeast */
USTRUCT(BlueprintType)
struct ASHES_API FValueThresholdRow : public FTableRowBase
{
	GENERATED_BODY()

	/** DT_Characters row name */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Threshold")
	FName Character;

	/** trust / affection / favor / suspicion */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Threshold")
	FName Value;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Threshold", meta=(ClampMin="0", ClampMax="100"))
	int32 AtLeast = 0;

	/** Id other systems listen for (e.g. one_true_thing, caught) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Threshold")
	FName UnlockId;

	/** Shown to the player instead of a number (e.g. "Zafir looks at you a moment too long."); may be empty */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Threshold", meta=(MultiLine=true))
	FString HintLine;

	/** Counts only on the romance route */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Threshold")
	bool bRomanceOnly = false;
};
