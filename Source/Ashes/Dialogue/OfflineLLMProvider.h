// AI-assisted: Claude Code, 10.11.2026, pre-written fallback replies in the same JSON format (Phase 3); lines from DT_FallbackLines (Phase 4, 12.11.2026); no repeat in a row; Hesitate / Content types. Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"
#include "LLMProvider.h"
#include "AshesTypes.h"
#include "OfflineLLMProvider.generated.h"

class UDataTable;

/**
 *  Answers instantly with a pre-written line in the GDD 8.1 JSON format (values unchanged, no beats).
 *  Used when the built-in model is not ready, too slow (timeout) or returns something broken.
 *  Lines come from DT_FallbackLines (same scene; the beat being pushed, else "any"; the current emotion preferred);
 *  without a matching row a few generic lines are used.
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
	static FString MakeFallbackJson(const FString& Line, FName Emotion = TEXT("neutral"));

	/**
	 *  Picks a line: same scene; rows of the pushed beat if any, else beat "any", else any row of the scene;
	 *  within that set the current emotion is preferred. Rotation spreads repeats. AvoidLine (the previous fallback)
	 *  is skipped while anything else fits, so the same line does not come twice in a row.
	 *  Only rows of the given Type are used (Hesitate for online failures, Content for offline mode). Returns false if nothing fits.
	 */
	static bool PickLine(const TArray<FFallbackLineRow>& Rows, FName Scene, FName Beat, FName Emotion, int32 Rotation, FFallbackLineRow& OutRow, const FString& AvoidLine = FString(), EFallbackLineType Type = EFallbackLineType::Content);

private:

	UPROPERTY()
	TObjectPtr<UDataTable> FallbackTable;

	bool bTriedLoad = false;
	int32 NextLine = 0;
	FString LastLine;
};
