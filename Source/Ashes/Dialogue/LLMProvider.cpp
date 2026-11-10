// AI-assisted: Claude Code, 10.11.2026, abstract LLM provider: the only way game code reaches the model (Phase 3). Reviewed by <pending>.

#include "LLMProvider.h"

void ULLMProvider::SendAsync(const FLLMRequest& Request, FOnLLMReply Callback)
{
	FLLMReply Reply;
	Reply.Error = TEXT("no provider");
	Callback.ExecuteIfBound(Reply);
}
