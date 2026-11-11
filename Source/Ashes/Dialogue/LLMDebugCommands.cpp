// AI-assisted: Claude Code, 10.11.2026, console commands to test the built-in AI (Ashes.TestLLM, Ashes.LLM.*) (Phase 3). Reviewed by <pending>.

#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "UObject/StrongObjectPtr.h"
#include "Ashes.h"
#include "AshesAISettings.h"
#include "AshesGameInstance.h"
#include "LocalModelSubsystem.h"
#include "OpenAICompatibleLLMProvider.h"
#include "OfflineLLMProvider.h"

namespace AshesLLMDebug
{
	// Short Zafir test prompt in the GDD 8.1 style (character card + JSON contract). The real one comes from UPromptBuilder later.
	static FLLMRequest MakeZafirTestRequest(const FString& PlayerLine)
	{
		FLLMRequest Request;
		Request.Messages.Emplace(TEXT("system"), TEXT(
			"You are Zafir, a rival assassin in the fire-worshipping palace of King Vashir. "
			"You call Sahra \"Little Ember\", tease her, use fire metaphors and joke when hurt. "
			"You hide why you are in the palace. Stay in character; nothing outside this world exists. "
			"Answer in 1-4 sentences (max 80 words). Reply ONLY with a JSON object with the keys: "
			"reply (string), emotion (one of neutral, teasing, tender, jealous, cold, angry), gesture (string), "
			"trust_delta (integer -8..8), affection_delta (integer -8..8), beats (array of strings), "
			"suggestions (array of exactly 3 short replies Sahra could say), off_topic (boolean)."));
		Request.Messages.Emplace(TEXT("user"), PlayerLine.IsEmpty()
			? TEXT("I saw you near the king's chamber last night. What are you really doing here?")
			: PlayerLine);
		return Request;
	}

	static void LogReply(const FLLMReply& Reply)
	{
		UE_LOG(LogAshes, Display, TEXT("TestLLM %s in %.2f s (%d tokens, %.1f tok/s)%s%s"),
			Reply.bFromFallback ? TEXT("FALLBACK") : (Reply.bSuccess ? TEXT("OK") : TEXT("FAILED")),
			Reply.LatencySeconds, Reply.CompletionTokens, Reply.TokensPerSecond,
			Reply.Error.IsEmpty() ? TEXT("") : TEXT(", error: "), *Reply.Error);
		UE_LOG(LogAshes, Display, TEXT("TestLLM reply: %s"), *Reply.Text);
	}

	static void SendDirect(const FString& PlayerLine)
	{
		// Editor without PIE: talk to the built-in server directly, fall back on failure
		ULocalModelSubsystem* Local = GEngine->GetEngineSubsystem<ULocalModelSubsystem>();
		const UAshesAISettings* Settings = GetDefault<UAshesAISettings>();

		static TStrongObjectPtr<UOpenAICompatibleLLMProvider> Provider(NewObject<UOpenAICompatibleLLMProvider>());
		static TStrongObjectPtr<UOfflineLLMProvider> Fallback(NewObject<UOfflineLLMProvider>());
		Provider->BaseUrl = Local->GetBaseUrl();
		Provider->Model = Settings->ModelAlias;
		Provider->TimeoutSeconds = Settings->GetEffectiveRequestTimeout();

		const FLLMRequest Request = MakeZafirTestRequest(PlayerLine);
		Provider->SendAsync(Request, FOnLLMReply::CreateLambda([Request](const FLLMReply& Reply)
		{
			if (Reply.bSuccess)
			{
				LogReply(Reply);
				return;
			}
			const FString Error = Reply.Error;
			const float Latency = Reply.LatencySeconds;
			Fallback->SendAsync(Request, FOnLLMReply::CreateLambda([Error, Latency](const FLLMReply& FallbackReply)
			{
				FLLMReply Out = FallbackReply;
				Out.Error = Error;
				Out.LatencySeconds = Latency;
				LogReply(Out);
			}));
		}));
	}
}

static FAutoConsoleCommandWithWorldAndArgs GAshesTestLLM(
	TEXT("Ashes.TestLLM"),
	TEXT("Sends a Zafir test line to the AI and prints the JSON reply. Usage: Ashes.TestLLM [player line]"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
	{
		const FString Line = FString::Join(Args, TEXT(" "));

		// In PIE / the game: the real path through the game instance (with fallback)
		if (UAshesGameInstance* GI = World ? World->GetGameInstance<UAshesGameInstance>() : nullptr)
		{
			UE_LOG(LogAshes, Display, TEXT("TestLLM via game instance (%s)"), *GI->GetAIStatusText());
			GI->RequestReply(AshesLLMDebug::MakeZafirTestRequest(Line), FOnLLMReply::CreateStatic(&AshesLLMDebug::LogReply));
			return;
		}

		ULocalModelSubsystem* Local = GEngine ? GEngine->GetEngineSubsystem<ULocalModelSubsystem>() : nullptr;
		if (!Local)
		{
			return;
		}
		if (Local->IsReady() || Local->GetState() == ELocalModelState::Failed)
		{
			AshesLLMDebug::SendDirect(Line);
			return;
		}

		// Not running yet: start it and send when ready
		UE_LOG(LogAshes, Display, TEXT("TestLLM: starting the built-in model first..."));
		Local->StartServer();
		TSharedRef<FDelegateHandle> Handle = MakeShared<FDelegateHandle>();
		*Handle = Local->OnStateChanged.AddLambda([Local, Line, Handle](ELocalModelState NewState)
		{
			if (NewState == ELocalModelState::Ready || NewState == ELocalModelState::Failed)
			{
				// Copy first: Remove() destroys this lambda and its captures while it is still running
				const FString LineCopy = Line;
				Local->OnStateChanged.Remove(*Handle);
				AshesLLMDebug::SendDirect(LineCopy);
			}
		});
	}));

static FAutoConsoleCommand GAshesLLMStart(
	TEXT("Ashes.LLM.Start"), TEXT("Starts the built-in AI server"),
	FConsoleCommandDelegate::CreateLambda([]() { if (GEngine) { GEngine->GetEngineSubsystem<ULocalModelSubsystem>()->StartServer(); } }));

static FAutoConsoleCommand GAshesLLMStop(
	TEXT("Ashes.LLM.Stop"), TEXT("Stops the built-in AI server and frees its memory"),
	FConsoleCommandDelegate::CreateLambda([]() { if (GEngine) { GEngine->GetEngineSubsystem<ULocalModelSubsystem>()->StopServer(); } }));

static FAutoConsoleCommand GAshesLLMStatus(
	TEXT("Ashes.LLM.Status"), TEXT("Prints the built-in AI status"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		if (GEngine)
		{
			const ULocalModelSubsystem* Local = GEngine->GetEngineSubsystem<ULocalModelSubsystem>();
			UE_LOG(LogAshes, Display, TEXT("%s %s"), *Local->GetStatusText(), *Local->GetBaseUrl());
		}
	}));
