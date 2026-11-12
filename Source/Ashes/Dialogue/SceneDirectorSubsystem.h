// AI-assisted: Claude Code, 11.11.2026, runs one conversation scene: prompt, model, validator, turn limit (Phase 5; full state machine in Phase 7); beats, clue hints, fallback context (Phase 4 data, 12.11.2026). Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "AshesTypes.h"
#include "LLMTypes.h"
#include "SceneDirectorSubsystem.generated.h"

class AAshesNPC;
class AAshesCharacter;
class UConversationWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAshesLinePosted, const FString&, Speaker, const FString&, Text);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAshesReplyReady, const FLLMResponse&, Response, bool, bFromFallback);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAshesThinkingChanged, bool, bThinking);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAshesTurnsChanged, int32, TurnsLeft);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAshesSceneEnded);

/**
 *  One conversation at a time (technical_design.md section 4.2).
 *  Phase 5: start / turns / end. Phase 7 adds the full FSM (Intro, FreeTalk, Nudge, Resolve, ForcedClose, Caught).
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

	/** Minimum time the typing indicator shows (design.md Motion_TypingIndicator) */
	static constexpr float MinThinkingSeconds = 0.4f;

private:

	void RequestReply(const FLLMRequest& Request, int32 AttemptsLeft);
	void DeliverReply(const FLLMResponse& Response, bool bFromFallback);
	void UseFallback(const FLLMRequest& Request);

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
	TSet<FName> ReachedBeats;
	FName LastEmotion;
	FName SceneRowName;

	/** Scene, beat being pushed and current emotion, so a fallback line fits the moment (DT_FallbackLines) */
	void FillFallbackContext(FLLMRequest& Request) const;

	TWeakObjectPtr<AAshesNPC> CurrentNpc;
	TWeakObjectPtr<AAshesCharacter> CurrentSahra;

	UPROPERTY()
	TObjectPtr<UConversationWidget> Widget;

	FTimerHandle ThinkingTimer;
};
