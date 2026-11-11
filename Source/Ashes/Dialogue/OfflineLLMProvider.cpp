// AI-assisted: Claude Code, 10.11.2026, pre-written fallback replies in the same JSON format (Phase 3). Reviewed by <pending>.

#include "OfflineLLMProvider.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

namespace
{
	// Placeholder "hesitates" lines until DT_FallbackLines exists (Tuana writes the real ones)
	const TCHAR* GFallbackLines[] =
	{
		TEXT("He studies you for a long moment, then looks away. \"Not here. Not now.\""),
		TEXT("\"Ask me again when the lanterns burn lower.\""),
		TEXT("A thin smile, nothing more. The question hangs in the smoke between you."),
	};
}

FString UOfflineLLMProvider::MakeFallbackJson(const FString& Line)
{
	TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
	Root->SetStringField(TEXT("reply"), Line);
	Root->SetStringField(TEXT("emotion"), TEXT("neutral"));
	Root->SetStringField(TEXT("gesture"), TEXT("hesitate"));
	Root->SetNumberField(TEXT("trust_delta"), 0);
	Root->SetNumberField(TEXT("affection_delta"), 0);
	Root->SetArrayField(TEXT("beats"), {});
	Root->SetArrayField(TEXT("suggestions"), {});
	Root->SetBoolField(TEXT("off_topic"), false);

	FString Out;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
	FJsonSerializer::Serialize(Root, Writer);
	return Out;
}

void UOfflineLLMProvider::SendAsync(const FLLMRequest& Request, FOnLLMReply Callback)
{
	FLLMReply Reply;
	Reply.bSuccess = true;
	Reply.bFromFallback = true;
	Reply.Text = MakeFallbackJson(GFallbackLines[NextLine % UE_ARRAY_COUNT(GFallbackLines)]);
	++NextLine;
	Callback.ExecuteIfBound(Reply);
}
