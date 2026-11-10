// AI-assisted: Claude Code, 10.11.2026, AI mode and the single entry point for model requests with automatic fallback (Phase 3). Reviewed by <pending>.

#include "AshesGameInstance.h"
#include "Ashes.h"
#include "AshesAISettings.h"
#include "LocalModelSubsystem.h"
#include "OpenAICompatibleLLMProvider.h"
#include "OfflineLLMProvider.h"
#include "Engine/Engine.h"

void UAshesGameInstance::Init()
{
	Super::Init();

	const UAshesAISettings* Settings = GetDefault<UAshesAISettings>();

	ModelProvider = NewObject<UOpenAICompatibleLLMProvider>(this);
	ModelProvider->TimeoutSeconds = Settings->RequestTimeoutSeconds;
	FallbackProvider = NewObject<UOfflineLLMProvider>(this);

	if (!Settings->ExternalBaseUrl.IsEmpty())
	{
		// Optional: an external OpenAI-compatible endpoint (e.g. Ollama) instead of the built-in model
		ModelProvider->BaseUrl = Settings->ExternalBaseUrl;
		ModelProvider->Model = Settings->ExternalModel;
		ModelProvider->bDisableThinking = false;
	}
	else if (ULocalModelSubsystem* Local = GEngine ? GEngine->GetEngineSubsystem<ULocalModelSubsystem>() : nullptr)
	{
		ModelProvider->Model = Settings->ModelAlias;
		LocalModelStateHandle = Local->OnStateChanged.AddUObject(this, &UAshesGameInstance::HandleLocalModelState);
		if (Settings->bAutoStartLocalModel)
		{
			Local->StartServer();
		}
		HandleLocalModelState(Local->GetState());
	}

	UE_LOG(LogAshes, Log, TEXT("%s"), *GetAIStatusText());
}

void UAshesGameInstance::Shutdown()
{
	// The server itself belongs to the engine subsystem (kept alive between PIE sessions in the editor)
	if (ULocalModelSubsystem* Local = GEngine ? GEngine->GetEngineSubsystem<ULocalModelSubsystem>() : nullptr)
	{
		Local->OnStateChanged.Remove(LocalModelStateHandle);
	}
	Super::Shutdown();
}

void UAshesGameInstance::HandleLocalModelState(ELocalModelState NewState)
{
	if (ULocalModelSubsystem* Local = GEngine ? GEngine->GetEngineSubsystem<ULocalModelSubsystem>() : nullptr)
	{
		ModelProvider->BaseUrl = (NewState == ELocalModelState::Ready) ? Local->GetBaseUrl() : FString();
	}
}

EAIMode UAshesGameInstance::GetAIMode() const
{
	return (ModelProvider && ModelProvider->IsAvailable()) ? EAIMode::Online : EAIMode::Offline;
}

FString UAshesGameInstance::GetAIStatusText() const
{
	if (!GetDefault<UAshesAISettings>()->ExternalBaseUrl.IsEmpty())
	{
		return FString::Printf(TEXT("AI: external endpoint %s"), *GetDefault<UAshesAISettings>()->ExternalBaseUrl);
	}
	if (const ULocalModelSubsystem* Local = GEngine ? GEngine->GetEngineSubsystem<ULocalModelSubsystem>() : nullptr)
	{
		return Local->GetStatusText();
	}
	return TEXT("AI: offline, pre-written dialogue");
}

void UAshesGameInstance::RequestReply(const FLLMRequest& Request, FOnLLMReply Callback)
{
	if (GetAIMode() == EAIMode::Offline)
	{
		FallbackProvider->SendAsync(Request, Callback);
		return;
	}

	TWeakObjectPtr<UAshesGameInstance> WeakThis(this);
	ModelProvider->SendAsync(Request, FOnLLMReply::CreateLambda([WeakThis, Request, Callback](const FLLMReply& Reply)
	{
		UAshesGameInstance* Self = WeakThis.Get();
		if (Reply.bSuccess || !Self)
		{
			Callback.ExecuteIfBound(Reply);
			return;
		}

		// Timeout / connection / broken reply: answer with a pre-written line, values unchanged (GDD 8.1)
		const FString Error = Reply.Error;
		const float Latency = Reply.LatencySeconds;
		Self->FallbackProvider->SendAsync(Request, FOnLLMReply::CreateLambda([Callback, Error, Latency](const FLLMReply& Fallback)
		{
			FLLMReply Out = Fallback;
			Out.Error = Error;
			Out.LatencySeconds = Latency;
			Callback.ExecuteIfBound(Out);
		}));
	}));
}
