// AI-assisted: Claude Code, 10.11.2026, HTTP provider for OpenAI-compatible chat endpoints (built-in llama-server, Ollama, cloud) (Phase 3); streamed so a timed-out answer stops on the server (11.11.2026); partial text while the model writes (Phase 7, 13.11.2026). Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"
#include "LLMProvider.h"
#include "Interfaces/IHttpRequest.h"
#include "OpenAICompatibleLLMProvider.generated.h"

/**
 *  Sends FLLMRequest to "<BaseUrl>/chat/completions" (OpenAI-compatible JSON) with FHttpModule.
 *  Used for the built-in llama-server (BaseUrl = http://127.0.0.1:<port>/v1) and, optionally,
 *  for any other compatible endpoint. No API key is needed for the built-in model.
 */
UCLASS()
class ASHES_API UOpenAICompatibleLLMProvider : public ULLMProvider
{
	GENERATED_BODY()

public:

	/** e.g. "http://127.0.0.1:8731/v1" */
	UPROPERTY()
	FString BaseUrl;

	UPROPERTY()
	FString Model;

	/** Optional bearer token (never stored in config or the build) */
	FString ApiKey;

	UPROPERTY()
	float TimeoutSeconds = 6.f;

	/** Turns off "thinking" output for models that support it (Qwen3.5); keeps replies short and fast */
	UPROPERTY()
	bool bDisableThinking = true;

	virtual void SendAsync(const FLLMRequest& Request, FOnLLMReply Callback) override;

	/** Streams: Partial gets the model's text so far while it arrives (Phase 7, word-by-word reply) */
	virtual void SendAsyncStreaming(const FLLMRequest& Request, FOnLLMReply Callback, FOnLLMPartial Partial) override;

	/** The text written so far in a streamed body that may end in the middle of a line (public for tests) */
	static FString ContentSoFar(const FString& StreamBody);

	/** Closes the running requests; llama-server stops writing a streamed answer as soon as its connection closes */
	virtual void CancelActive() override;

	virtual bool IsAvailable() const override { return !BaseUrl.IsEmpty(); }
	virtual FString GetProviderName() const override { return FString::Printf(TEXT("OpenAI-compatible (%s)"), *BaseUrl); }

	/** Builds the JSON body (public for tests) */
	FString BuildRequestBody(const FLLMRequest& Request) const;

	/** Reads choices[0].message.content and timings from a response body (public for tests) */
	static bool ParseResponseBody(const FString& Body, FLLMReply& OutReply);

private:

	/** Requests not finished yet (for CancelActive) */
	TArray<TWeakPtr<IHttpRequest, ESPMode::ThreadSafe>> ActiveRequests;
};
