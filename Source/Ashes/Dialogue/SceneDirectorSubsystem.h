// AI-assisted: Claude Code, 11.11.2026, runs one conversation scene: prompt, model, validator, turn limit (Phase 5; full state machine in Phase 7); beats, clue hints, fallback context (Phase 4 data, 12.11.2026); soft / hard limit with hesitation; Phase 7: state machine (FSceneFlow), NPC brain, offline tree, word-by-word reply (13.11.2026); Phase 6 fixes 4, 7. Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AshesTypes.h"
#include "LLMTypes.h"
#include "SceneFlow.h"
#include "OfflineTreeWalker.h"
#include "SceneDirectorSubsystem.generated.h"

class AAshesNPC;
class AAshesCharacter;
class UConversationWidget;
class UNPCBrainComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAshesLinePosted, const FString&, Speaker, const FString&, Text);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAshesReplyReady, const FLLMResponse&, Response, bool, bFromFallback);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAshesThinkingChanged, bool, bThinking);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAshesTurnsChanged, int32, TurnsLeft);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAshesSceneEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAshesHesitating);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAshesSceneResolved, bool, bFull);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAshesReplyPartial, const FString&, TextSoFar);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAshesCaught);

/**
 *  One conversation at a time (technical_design.md section 4.2).
 *  Phase 7: the rules live in FSceneFlow (Intro, FreeTalk, Nudge, Resolve, ForcedClose, Caught), the values in the NPC's
 *  UNPCBrainComponent, the offline answers in FOfflineTreeWalker; this class connects them to the prompt, the model and the screen.
 */
UCLASS()
class ASHES_API USceneDirectorSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	/** Opens the conversation screen with this NPC; false if data is missing or a scene is running */
	bool StartScene(AAshesNPC* Npc, AAshesCharacter* Sahra);

	/** The player's line (typed or a picked suggestion) */
	UFUNCTION(BlueprintCallable, Category="Conversation")
	void SubmitLine(const FString& Line);

	/** Picks suggestion 1–3 of the last reply */
	UFUNCTION(BlueprintCallable, Category="Conversation")
	void PickSuggestion(int32 Index);

	/** Closes the screen and gives control back to the player */
	UFUNCTION(BlueprintCallable, Category="Conversation")
	void EndScene();

	UFUNCTION(BlueprintPure, Category="Conversation")
	bool IsSceneActive() const { return bActive; }

	/** True after the turn limit: only Leave is possible */
	UFUNCTION(BlueprintPure, Category="Conversation")
	bool IsFinished() const { return bFinished; }

	/** Where the scene is: Intro, FreeTalk, Nudge, Resolve, ForcedClose or Caught */
	UFUNCTION(BlueprintPure, Category="Conversation")
	EScenePhase GetPhase() const { return Flow.GetPhase(); }

	/** The values of the character being talked to (null outside a scene) */
	UFUNCTION(BlueprintPure, Category="Conversation")
	UNPCBrainComponent* GetBrain() const;

	/** One log line: phase, turns, beats, values (debug: Ashes.Scene.Status) */
	FString GetStatusText() const;

	/** False while the AI is offline: then the player only picks a reply (typed text cannot fit pre-written lines) */
	UFUNCTION(BlueprintPure, Category="Conversation")
	bool IsTypingAllowed() const;

	/** Number of reply buttons the screen has */
	static constexpr int32 SuggestionCount = 3;

	/**
	 *  Fills Suggestions up to SuggestionCount from Stock (in order), skipping lines already there and AvoidLine
	 *  (what the player just said); any case. Public for tests.
	 */
	static void TopUpSuggestions(TArray<FString>& Suggestions, const TArray<FString>& Stock, const FString& AvoidLine = FString());

	UFUNCTION(BlueprintPure, Category="Conversation")
	int32 GetTurnsLeft() const { return FMath::Max(0, Scene.TurnLimit - TurnsTaken); }

	UFUNCTION(BlueprintPure, Category="Conversation")
	FString GetSpeakerName() const { return Card.DisplayName; }

	UPROPERTY(BlueprintAssignable, Category="Conversation")
	FAshesLinePosted OnLinePosted;

	UPROPERTY(BlueprintAssignable, Category="Conversation")
	FAshesReplyReady OnReplyReady;

	UPROPERTY(BlueprintAssignable, Category="Conversation")
	FAshesThinkingChanged OnThinkingChanged;

	UPROPERTY(BlueprintAssignable, Category="Conversation")
	FAshesTurnsChanged OnTurnsChanged;

	UPROPERTY(BlueprintAssignable, Category="Conversation")
	FAshesSceneEnded OnSceneEnded;

	/** Soft limit reached while still waiting: the status shows "<Name> hesitates..." (no dialogue line) */
	UPROPERTY(BlueprintAssignable, Category="Conversation")
	FAshesHesitating OnHesitating;

	/** The scene closed by its goal (bFull) or by the turn limit (weaker outcome); only Leave is possible after it */
	UPROPERTY(BlueprintAssignable, Category="Conversation")
	FAshesSceneResolved OnSceneResolved;

	/** Suspicion reached 100 */
	UPROPERTY(BlueprintAssignable, Category="Conversation")
	FAshesCaught OnCaught;

	/**
	 *  The character's reply while the model is still writing it (word by word, not yet validated).
	 *  Empty text = take the unfinished line off the screen (it failed the check, or the final line replaces it).
	 */
	UPROPERTY(BlueprintAssignable, Category="Conversation")
	FAshesReplyPartial OnReplyPartial;

	/** Minimum time the typing indicator shows (design.md Motion_TypingIndicator) */
	static constexpr float MinThinkingSeconds = 0.4f;

	/** Undo the turn of an unanswered line: one turn back and the player's line leaves the history (public for tests) */
	static void RefundUnansweredTurn(int32& InOutTurnsTaken, TArray<FConversationTurn>& InOutHistory);

private:

	void RequestReply(const FLLMRequest& Request, int32 AttemptsLeft);
	/** bAuthored: an offline tree answer (pre-written, but its value changes and beats count) */
	void DeliverReply(const FLLMResponse& Response, bool bFromFallback, bool bAuthored = false);

	/** Offline: answers from DT_OfflineTrees if the line matches a reply of the current node; false = use a fallback line */
	bool TryAnswerFromTree(const FString& Line);

	/** Resolve / ForcedClose / Caught: closing line, delegates, only Leave stays */
	void CloseScene();

	UFUNCTION()
	void HandleThresholdReached(FName UnlockId, const FString& HintLine);
	void UseFallback(const FLLMRequest& Request);

	/** Soft limit reached: status only, keep waiting for the real answer */
	void ShowHesitation();

	/** Bumped per player line: replies, retries and timeouts of an older line are dropped */
	int32 RequestSerial = 0;

	/** Online failure (hard limit, broken / spoiling replies): a Hesitate line instead of an answer; the turn does not count */
	void HesitateInsteadOfReply(const FLLMRequest& Request, const FString& Reason);
	void FinishHesitation(const FLLMResponse& Hesitation);

	/** Shows a fallback line: speech with the speaker, a [gesture] as "(...)"; gesture-only lines show just the gesture */
	void PostFallbackLine(const FLLMResponse& Line);

	bool bActive = false;
	bool bWaiting = false;
	bool bFinished = false;
	int32 TurnsTaken = 0;
	double RequestStartTime = 0.0;

	FCharacterCardRow Card;
	FSceneCardRow Scene;
	FString Lore;
	FPromptState State;
	TArray<FConversationTurn> History;
	TArray<FString> LastSuggestions;
	/** Beats, turns, stall and the current phase (the rules) */
	FSceneFlow Flow;

	/** Offline answers for this scene (DT_OfflineTrees); inactive when the scene has no tree */
	FOfflineTreeWalker Tree;
	bool bTreeEnded = false;

	/** Word by word (Phase 7): the model's text so far arrived for the current line */
	void HandlePartialText(const FString& TextSoFar);

	/** Takes the unfinished line off the screen (before the final line, a retry or a dodge) */
	void ClearLiveReply();

	/** The reply text shown so far for the current request (empty = nothing shown) */
	FString LiveReply;

	/** The shown text hit a locked spoiler or unsafe word: stop showing this request's words */
	bool bLiveBlocked = false;

	/** The time to the first words was logged for the current line */
	bool bFirstWordsLogged = false;

	/** Thinking limit (FirstWordsTimeoutSeconds): no words on screen by then -> stop the model, the character dodges */
	FTimerHandle FirstWordsTimer;

	/** The request of the line being answered (the dodge line after the thinking limit needs its context) */
	FLLMRequest PendingRequest;

	void HandleFirstWordsTimeout();

	/** The AI went online or offline during the scene: show or hide typing, and offline give pick-only buttons */
	void HandleAIModeChanged();
	FDelegateHandle AIModeHandle;

	/** The scene's stock lines (DT_SceneRules StockSuggestions), then a few generic ones */
	TArray<FString> GetStockSuggestions() const;

	/** Reply buttons when nothing else gives them: the offline tree's replies, else the stock lines */
	TArray<FString> GetChoicesWithoutModel() const;

	/** Hint lines of thresholds reached by the current reply; shown after it */
	TArray<FString> PendingHints;

	TWeakObjectPtr<UNPCBrainComponent> Brain;
	FName LastEmotion;
	FName SceneRowName;
	FName CharacterRowName;

	/** Scene, beat being pushed and current emotion, so a fallback line fits the moment (DT_FallbackLines) */
	void FillFallbackContext(FLLMRequest& Request) const;

	TWeakObjectPtr<AAshesNPC> CurrentNpc;
	TWeakObjectPtr<AAshesCharacter> CurrentSahra;

	UPROPERTY()
	TObjectPtr<UConversationWidget> Widget;

	FTimerHandle ThinkingTimer;
	FTimerHandle HesitateTimer;
};
