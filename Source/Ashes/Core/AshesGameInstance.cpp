// AI-assisted: Claude Code, 10.11.2026, AI mode and the single entry point for model requests with automatic fallback (Phase 3); prompt warm-up (Phase 5, 11.11.2026); warm-up kept until the model is ready, AI mode change event (Phase 6 fix 6, 13.11.2026). Reviewed by <pending>.

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
	ModelProvider->TimeoutSeconds = Settings->GetEffectiveRequestTimeout();
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

	LastAIMode = GetAIMode();
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

	const EAIMode Mode = GetAIMode();
	if (Mode == LastAIMode)
	{
		return;
	}
	LastAIMode = Mode;

	// A scene opened while the model was loading: warm it up now, so the first real reply is not a cold one (41 s in the playtest)
	if (Mode == EAIMode::Online && PendingPrime.IsSet())
	{
		UE_LOG(LogAshes, Log, TEXT("AI ready: sending the warm-up that waited for it"));
		const FLLMRequest Request = PendingPrime.GetValue();
		PendingPrime.Reset();
		SendPrime(Request);
	}
	OnAIModeChanged.Broadcast();
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

void UAshesGameInstance::RequestFallback(const FLLMRequest& Request, FOnLLMReply Callback)
{
	FallbackProvider->SendAsync(Request, Callback);
}

void UAshesGameInstance::PrimeModel(const FLLMRequest& Request)
{
	if (GetAIMode() != EAIMode::Online)
	{
		PendingPrime = Request; // the newest scene's prompt; sent when the model is ready
		return;
	}
	PendingPrime.Reset();
	SendPrime(Request);
}

void UAshesGameInstance::SendPrime(const FLLMRequest& Request)
{
	// llama-server keeps the processed prompt in its cache; the real first request then only adds the new line
	FLLMRequest Prime = Request;
	Prime.MaxTokens = 1;
	Prime.bJsonOutput = false;
	ModelProvider->SendAsync(Prime, FOnLLMReply::CreateLambda([](const FLLMReply& Reply)
	{
		UE_LOG(LogAshes, Log, TEXT("AI prompt cached in %.1f s%s"), Reply.LatencySeconds, Reply.bSuccess ? TEXT("") : *(TEXT(" (") + Reply.Error + TEXT(")")));
	}));
}
