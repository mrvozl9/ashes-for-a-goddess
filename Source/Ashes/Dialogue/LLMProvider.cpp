// AI-assisted: Claude Code, 10.11.2026, abstract LLM provider: the only way game code reaches the model (Phase 3); optional streaming of partial text (Phase 7, 13.11.2026). Reviewed by <pending>.

#include "LLMProvider.h"

void ULLMProvider::SendAsync(const FLLMRequest& Request, FOnLLMReply Callback)
{
	FLLMReply Reply;
	Reply.Error = TEXT("no provider");
	Callback.ExecuteIfBound(Reply);
}

void ULLMProvider::SendAsyncStreaming(const FLLMRequest& Request, FOnLLMReply Callback, FOnLLMPartial /*Partial*/)
{
	// Default: no streaming, the whole answer arrives at once
	SendAsync(Request, Callback);
}
