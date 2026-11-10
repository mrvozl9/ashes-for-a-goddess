// AI-assisted: Claude Code, 10.11.2026, HTTP provider for OpenAI-compatible chat endpoints (built-in llama-server, Ollama, cloud) (Phase 3). Reviewed by <pending>.

#include "OpenAICompatibleLLMProvider.h"
#include "Ashes.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "HAL/PlatformTime.h"

FString UOpenAICompatibleLLMProvider::BuildRequestBody(const FLLMRequest& Request) const
{
	TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
	Root->SetStringField(TEXT("model"), Model);
	Root->SetNumberField(TEXT("temperature"), Request.Temperature);
	Root->SetNumberField(TEXT("max_tokens"), Request.MaxTokens);
	Root->SetBoolField(TEXT("stream"), false);

	TArray<TSharedPtr<FJsonValue>> Messages;
	for (const FLLMMessage& Message : Request.Messages)
	{
		TSharedRef<FJsonObject> Obj = MakeShared<FJsonObject>();
		Obj->SetStringField(TEXT("role"), Message.Role);
		Obj->SetStringField(TEXT("content"), Message.Content);
		Messages.Add(MakeShared<FJsonValueObject>(Obj));
	}
	Root->SetArrayField(TEXT("messages"), Messages);

	if (Request.bJsonOutput)
	{
		TSharedRef<FJsonObject> Format = MakeShared<FJsonObject>();
		Format->SetStringField(TEXT("type"), TEXT("json_object"));
		Root->SetObjectField(TEXT("response_format"), Format);
	}

	if (bDisableThinking)
	{
		TSharedRef<FJsonObject> Kwargs = MakeShared<FJsonObject>();
		Kwargs->SetBoolField(TEXT("enable_thinking"), false);
		Root->SetObjectField(TEXT("chat_template_kwargs"), Kwargs);
	}

	FString Body;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
	FJsonSerializer::Serialize(Root, Writer);
	return Body;
}

bool UOpenAICompatibleLLMProvider::ParseResponseBody(const FString& Body, FLLMReply& OutReply)
{
	TSharedPtr<FJsonObject> Root;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Body);
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		OutReply.Error = TEXT("response is not JSON");
		return false;
	}

	const TArray<TSharedPtr<FJsonValue>>* Choices = nullptr;
	if (!Root->TryGetArrayField(TEXT("choices"), Choices) || Choices->Num() == 0)
	{
		OutReply.Error = TEXT("no choices in response");
		return false;
	}

	const TSharedPtr<FJsonObject> Choice = (*Choices)[0]->AsObject();
	const TSharedPtr<FJsonObject>* MessageObj = nullptr;
	FString Content;
	if (!Choice.IsValid() || !Choice->TryGetObjectField(TEXT("message"), MessageObj) || !(*MessageObj)->TryGetStringField(TEXT("content"), Content))
	{
		OutReply.Error = TEXT("no message content");
		return false;
	}

	Content.TrimStartAndEndInline();
	if (Content.IsEmpty())
	{
		OutReply.Error = TEXT("empty reply");
		return false;
	}
	OutReply.Text = Content;

	// llama-server reports generation speed; other endpoints may not
	const TSharedPtr<FJsonObject>* Timings = nullptr;
	if (Root->TryGetObjectField(TEXT("timings"), Timings))
	{
		double PerSecond = 0.0;
		int32 Predicted = 0;
		(*Timings)->TryGetNumberField(TEXT("predicted_per_second"), PerSecond);
		(*Timings)->TryGetNumberField(TEXT("predicted_n"), Predicted);
		OutReply.TokensPerSecond = static_cast<float>(PerSecond);
		OutReply.CompletionTokens = Predicted;
	}
	const TSharedPtr<FJsonObject>* Usage = nullptr;
	if (OutReply.CompletionTokens == 0 && Root->TryGetObjectField(TEXT("usage"), Usage))
	{
		(*Usage)->TryGetNumberField(TEXT("completion_tokens"), OutReply.CompletionTokens);
	}

	OutReply.bSuccess = true;
	return true;
}

void UOpenAICompatibleLLMProvider::SendAsync(const FLLMRequest& Request, FOnLLMReply Callback)
{
	if (BaseUrl.IsEmpty())
	{
		FLLMReply Reply;
		Reply.Error = TEXT("no endpoint");
		Callback.ExecuteIfBound(Reply);
		return;
	}

	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Http = FHttpModule::Get().CreateRequest();
	Http->SetURL(BaseUrl / TEXT("chat/completions"));
	Http->SetVerb(TEXT("POST"));
	Http->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	if (!ApiKey.IsEmpty())
	{
		Http->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *ApiKey));
	}
	Http->SetContentAsString(BuildRequestBody(Request));
	Http->SetTimeout(TimeoutSeconds);

	const double StartTime = FPlatformTime::Seconds();

	// Completion runs on the game thread; the game never waits for the model
	Http->OnProcessRequestComplete().BindLambda(
		[Callback, StartTime](FHttpRequestPtr /*Req*/, FHttpResponsePtr Response, bool bConnected)
		{
			FLLMReply Reply;
			Reply.LatencySeconds = static_cast<float>(FPlatformTime::Seconds() - StartTime);

			if (!bConnected || !Response.IsValid())
			{
				Reply.Error = TEXT("timeout or connection failed");
			}
			else if (Response->GetResponseCode() != 200)
			{
				Reply.Error = FString::Printf(TEXT("http %d"), Response->GetResponseCode());
			}
			else
			{
				ParseResponseBody(Response->GetContentAsString(), Reply);
			}

			if (!Reply.bSuccess)
			{
				UE_LOG(LogAshes, Warning, TEXT("LLM request failed after %.2f s: %s"), Reply.LatencySeconds, *Reply.Error);
			}
			Callback.ExecuteIfBound(Reply);
		});

	Http->ProcessRequest();
}
