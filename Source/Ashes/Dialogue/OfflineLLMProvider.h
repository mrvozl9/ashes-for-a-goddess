// AI-assisted: Claude Code, 10.11.2026, pre-written fallback replies in the same JSON format (Phase 3). Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"
#include "LLMProvider.h"
#include "OfflineLLMProvider.generated.h"

/**
 *  Answers instantly with a pre-written line in the GDD 8.1 JSON format (values unchanged, no beats).
 *  Used when the built-in model is not ready, too slow (timeout) or returns something broken.
 *  Phase 3: a few generic lines in code. Later: DT_FallbackLines by scene, beat and emotion (~120 lines by Tuana).
 */
UCLASS()
class ASHES_API UOfflineLLMProvider : public ULLMProvider
{
	GENERATED_BODY()

public:

	virtual void SendAsync(const FLLMRequest& Request, FOnLLMReply Callback) override;
	virtual bool IsAvailable() const override { return true; }
	virtual FString GetProviderName() const override { return TEXT("Offline (pre-written)"); }

	/** Builds the fallback JSON for one line (public for tests) */
	static FString MakeFallbackJson(const FString& Line);

private:

	int32 NextLine = 0;
};
