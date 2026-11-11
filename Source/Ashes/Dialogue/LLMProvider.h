// AI-assisted: Claude Code, 10.11.2026, abstract LLM provider: the only way game code reaches the model (Phase 3). Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LLMTypes.h"
#include "LLMProvider.generated.h"

/**
 *  The only way game code reaches a language model (technical_design.md section 4).
 *  Implementations: UOpenAICompatibleLLMProvider (built-in llama.cpp server, Ollama or a cloud endpoint)
 *  and UOfflineLLMProvider (pre-written fallback). SendAsync never blocks the game thread.
 */
UCLASS(Abstract)
class ASHES_API ULLMProvider : public UObject
{
	GENERATED_BODY()

public:

	/** Sends the request; Callback runs on the game thread exactly once */
	virtual void SendAsync(const FLLMRequest& Request, FOnLLMReply Callback);

	/** True when the provider can answer right now */
	virtual bool IsAvailable() const { return false; }

	/** Short name for logs and the settings status line */
	virtual FString GetProviderName() const { return TEXT("None"); }
};
