// AI-assisted: Claude Code, 10.11.2026, AI mode and the single entry point for model requests with automatic fallback (Phase 3); prompt warm-up (Phase 5, 11.11.2026). Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LLMTypes.h"
#include "AshesGameInstance.generated.h"

class UOpenAICompatibleLLMProvider;
class UOfflineLLMProvider;
enum class ELocalModelState : uint8;

/** Online = the AI model answers (built-in or external endpoint); Offline = pre-written lines only */
UENUM(BlueprintType)
enum class EAIMode : uint8
{
	Online,
	Offline
};

/**
 *  Lives for the whole game. Owns the LLM providers; game code calls RequestReply and always gets an answer:
 *  the model's reply, or a pre-written fallback line when the model is not ready, too slow or broken.
 */
UCLASS()
class ASHES_API UAshesGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;
	virtual void Shutdown() override;

	/** Sends the request to the model; falls back to a pre-written line on any failure. Callback runs once on the game thread. */
	void RequestReply(const FLLMRequest& Request, FOnLLMReply Callback);

	/** Sends the prompt with a 1-token limit and ignores the answer, so the model caches it before the player's first line */
	void PrimeModel(const FLLMRequest& Request);

	/** A pre-written fallback line right away (after a reply failed validation twice) */
	void RequestFallback(const FLLMRequest& Request, FOnLLMReply Callback);

	UFUNCTION(BlueprintPure, Category="AI")
	EAIMode GetAIMode() const;

	/** Status line for Settings ("AI: built-in model ready", ...) */
	UFUNCTION(BlueprintPure, Category="AI")
	FString GetAIStatusText() const;

private:

	void HandleLocalModelState(ELocalModelState NewState);

	UPROPERTY()
	TObjectPtr<UOpenAICompatibleLLMProvider> ModelProvider;

	UPROPERTY()
	TObjectPtr<UOfflineLLMProvider> FallbackProvider;

	FDelegateHandle LocalModelStateHandle;
};
