// AI-assisted: Claude Code, 10.11.2026, request / reply types for the LLM providers (Phase 3); fallback context fields (Phase 4, 12.11.2026). Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"
#include "LLMTypes.generated.h"

/** One chat message: Role is "system", "user" or "assistant" */
USTRUCT(BlueprintType)
struct ASHES_API FLLMMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LLM")
	FString Role;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LLM")
	FString Content;

	FLLMMessage() = default;
	FLLMMessage(const FString& InRole, const FString& InContent) : Role(InRole), Content(InContent) {}
};

/** What the game asks the model (built by UPromptBuilder in a later phase) */
USTRUCT(BlueprintType)
struct ASHES_API FLLMRequest
{
	GENERATED_BODY()

	/** Ordered messages: system prompt first, then the conversation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LLM")
	TArray<FLLMMessage> Messages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LLM")
	float Temperature = 0.8f;

	/** Upper bound for the reply length in tokens (1–4 sentences + JSON fields fit in ~300) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LLM")
	int32 MaxTokens = 300;

	/** Ask the model for a single JSON object (GDD 8.1 response JSON) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LLM")
	bool bJsonOutput = true;

	/** For the fallback pick (DT_FallbackLines): scene row, beat being pushed (None = any), current emotion */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LLM")
	FName FallbackScene;

	/** DT_Characters row of the speaker (fallback lines are per character) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LLM")
	FName FallbackCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LLM")
	FName FallbackBeat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LLM")
	FName FallbackEmotion;

	/** True: pick a Hesitate line (online timeout / broken reply); false: a Content line (offline) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="LLM")
	bool bHesitate = false;
};

/** What comes back. Text is the raw model output (the JSON string); UResponseValidator parses it later. */
USTRUCT(BlueprintType)
struct ASHES_API FLLMReply
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category="LLM")
	bool bSuccess = false;

	UPROPERTY(BlueprintReadOnly, Category="LLM")
	FString Text;

	/** Empty on success; "timeout", "connection", "http 500", "empty reply", ... on failure */
	UPROPERTY(BlueprintReadOnly, Category="LLM")
	FString Error;

	/** True when the reply is a pre-written fallback, not the model */
	UPROPERTY(BlueprintReadOnly, Category="LLM")
	bool bFromFallback = false;

	UPROPERTY(BlueprintReadOnly, Category="LLM")
	float LatencySeconds = 0.f;

	/** Generated tokens and speed, when the server reports them */
	UPROPERTY(BlueprintReadOnly, Category="LLM")
	int32 CompletionTokens = 0;

	UPROPERTY(BlueprintReadOnly, Category="LLM")
	float TokensPerSecond = 0.f;

	/** Prompt tokens read for this request, of which taken from the server cache, and how long reading took */
	UPROPERTY(BlueprintReadOnly, Category="LLM")
	int32 PromptTokens = 0;

	UPROPERTY(BlueprintReadOnly, Category="LLM")
	int32 CachedTokens = 0;

	UPROPERTY(BlueprintReadOnly, Category="LLM")
	float PromptMs = 0.f;
};

/** Called on the game thread when a request finishes (success, failure or timeout) */
DECLARE_DELEGATE_OneParam(FOnLLMReply, const FLLMReply& /*Reply*/);
