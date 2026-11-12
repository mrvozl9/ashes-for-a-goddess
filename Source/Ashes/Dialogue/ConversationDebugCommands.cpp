// AI-assisted: Claude Code, 11.11.2026, Ashes.TalkZafir: test conversation through the real prompt builder and validator (Phase 4); fallback context (12.11.2026). Reviewed by <pending>.
// Temporary harness until the Scene Director and WBP_Conversation exist (Phases 5–6).

#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "UObject/StrongObjectPtr.h"
#include "Ashes.h"
#include "AshesTypes.h"
#include "AshesAISettings.h"
#include "LocalModelSubsystem.h"
#include "OpenAICompatibleLLMProvider.h"
#include "OfflineLLMProvider.h"
#include "PromptBuilder.h"
#include "ResponseValidator.h"

namespace AshesTalk
{
	const TCHAR* CharactersPath = TEXT("/Game/Ashes/Data/DT_Characters.DT_Characters");
	const TCHAR* ScenesPath = TEXT("/Game/Ashes/Data/DT_Scenes.DT_Scenes");
	const TCHAR* LorePath = TEXT("/Game/Ashes/Data/DA_LoreBible.DA_LoreBible");
	const FName CharacterRow = TEXT("Zafir");
	const FName SceneRow = TEXT("Ch1_ZafirKeyScene");

	// Conversation state kept between console calls
	TArray<FConversationTurn> History;
	FPromptState State;
	bool bBusy = false;
	TSet<FName> ReachedBeats;
	FName LastEmotion;

	TStrongObjectPtr<UOpenAICompatibleLLMProvider>& Provider()
	{
		static TStrongObjectPtr<UOpenAICompatibleLLMProvider> P(NewObject<UOpenAICompatibleLLMProvider>());
		return P;
	}

	TStrongObjectPtr<UOfflineLLMProvider>& Fallback()
	{
		static TStrongObjectPtr<UOfflineLLMProvider> P(NewObject<UOfflineLLMProvider>());
		return P;
	}

	bool LoadData(FCharacterCardRow& OutCard, FSceneCardRow& OutScene, FString& OutLore)
	{
		const UDataTable* Characters = LoadObject<UDataTable>(nullptr, CharactersPath);
		const UDataTable* Scenes = LoadObject<UDataTable>(nullptr, ScenesPath);
		const UAshesLoreBible* Lore = LoadObject<UAshesLoreBible>(nullptr, LorePath);
		const FCharacterCardRow* Card = Characters ? Characters->FindRow<FCharacterCardRow>(CharacterRow, TEXT("TalkZafir")) : nullptr;
		const FSceneCardRow* Scene = Scenes ? Scenes->FindRow<FSceneCardRow>(SceneRow, TEXT("TalkZafir")) : nullptr;
		if (!Card || !Scene)
		{
			UE_LOG(LogAshes, Error, TEXT("TalkZafir: missing DT_Characters row 'Zafir' or DT_Scenes row 'Ch1_ZafirKeyScene' in /Game/Ashes/Data"));
			return false;
		}
		OutCard = *Card;
		OutScene = *Scene;
		OutLore = Lore ? Lore->Text : FString();
		return true;
	}

	void Finish(const FCharacterCardRow& Card, const FAshesValidationResult* Valid, const FLLMReply& Raw, bool bFallback)
	{
		bBusy = false;

		FString ReplyText;
		if (bFallback || !Valid)
		{
			// Fallback line: values unchanged (GDD 8.1)
			const FAshesValidationResult FallbackParsed = UResponseValidator::Validate(Raw.Text, FAshesValidationContext::FromCard(Card, FSceneCardRow()));
			ReplyText = FallbackParsed.Response.Reply;
			UE_LOG(LogAshes, Display, TEXT("Zafir (FALLBACK%s%s): %s"), Raw.Error.IsEmpty() ? TEXT("") : TEXT(", "), *Raw.Error, *ReplyText);
			LastEmotion = FallbackParsed.Response.Emotion;
		}
		else
		{
			const FLLMResponse& R = Valid->Response;
			ReplyText = R.Reply;
			FString DeltaText;
			for (const TPair<FName, int32>& Pair : R.Deltas)
			{
				int32& Value = State.Values.FindOrAdd(Pair.Key);
				Value = FMath::Clamp(Value + Pair.Value, 0, 100);
				DeltaText += FString::Printf(TEXT("%s %+d -> %d  "), *Pair.Key.ToString(), Pair.Value, Value);
			}
			TArray<FString> BeatStrings;
			for (const FName& Beat : R.Beats) { BeatStrings.Add(Beat.ToString()); ReachedBeats.Add(Beat); }
			LastEmotion = R.Emotion;

			UE_LOG(LogAshes, Display, TEXT("Zafir [%s, %s] (%.1f s, %.0f tok/s): %s"), *R.Emotion.ToString(), *R.Gesture, Raw.LatencySeconds, Raw.TokensPerSecond, *R.Reply);
			UE_LOG(LogAshes, Display, TEXT("  values: %s| beats: %s | off_topic: %s"), *DeltaText, BeatStrings.Num() ? *FString::Join(BeatStrings, TEXT(", ")) : TEXT("-"), R.bOffTopic ? TEXT("yes") : TEXT("no"));
			for (int32 i = 0; i < R.Suggestions.Num(); ++i)
			{
				UE_LOG(LogAshes, Display, TEXT("  %d: %s"), i + 1, *R.Suggestions[i]);
			}
			for (const FString& Note : Valid->Notes)
			{
				UE_LOG(LogAshes, Display, TEXT("  validator: %s"), *Note);
			}
		}

		FConversationTurn Turn;
		Turn.bFromPlayer = false;
		Turn.Text = ReplyText;
		History.Add(Turn);
	}

	/** Sends, validates; broken JSON -> one silent retry, spoiler/unsafe -> one regenerate, then the fallback line */
	void SendAndValidate(const FLLMRequest& Request, const FCharacterCardRow& Card, const FAshesValidationContext& Context, int32 AttemptsLeft)
	{
		Provider()->SendAsync(Request, FOnLLMReply::CreateLambda([Request, Card, Context, AttemptsLeft](const FLLMReply& Reply)
		{
			if (!Reply.bSuccess)
			{
				Fallback()->SendAsync(Request, FOnLLMReply::CreateLambda([Card, Reply](const FLLMReply& FallbackReply)
				{
					FLLMReply Out = FallbackReply;
					Out.Error = Reply.Error;
					Finish(Card, nullptr, Out, true);
				}));
				return;
			}

			const FAshesValidationResult Result = UResponseValidator::Validate(Reply.Text, Context);
			if (Result.IsOk())
			{
				Finish(Card, &Result, Reply, false);
				return;
			}

			UE_LOG(LogAshes, Display, TEXT("  validator: %s -> %s"), *FString::Join(Result.Notes, TEXT("; ")), AttemptsLeft > 0 ? TEXT("asking again") : TEXT("fallback line"));
			if (AttemptsLeft > 0)
			{
				SendAndValidate(Request, Card, Context, AttemptsLeft - 1);
				return;
			}
			Fallback()->SendAsync(Request, FOnLLMReply::CreateLambda([Card](const FLLMReply& FallbackReply)
			{
				FLLMReply Out = FallbackReply;
				Out.Error = TEXT("invalid reply");
				Finish(Card, nullptr, Out, true);
			}));
		}));
	}

	void Talk(const FString& Line)
	{
		if (bBusy)
		{
			UE_LOG(LogAshes, Warning, TEXT("TalkZafir: still waiting for the last reply"));
			return;
		}

		FCharacterCardRow Card;
		FSceneCardRow Scene;
		FString Lore;
		if (!LoadData(Card, Scene, Lore))
		{
			return;
		}

		if (History.Num() == 0)
		{
			// New conversation: start values and the authored opener
			State = FPromptState();
			for (const FRelationshipValueDef& Def : Card.Values)
			{
				State.Values.Add(Def.Name, Def.Start);
			}
			FConversationTurn Opener;
			Opener.bFromPlayer = false;
			Opener.Text = Scene.Opener;
			History.Add(Opener);
			UE_LOG(LogAshes, Display, TEXT("Zafir (opener): %s"), *Scene.Opener);
		}

		const FString PlayerLine = Line.Left(200);
		if (UResponseValidator::IsUnsafeText(PlayerLine, UResponseValidator::DefaultSafetyWords()))
		{
			UE_LOG(LogAshes, Display, TEXT("TalkZafir: unsafe player line (the character refuses in character; 3 times closes the scene, later phase)"));
		}
		UE_LOG(LogAshes, Display, TEXT("Sahra: %s"), *PlayerLine);

		FLLMRequest Request = UPromptBuilder::Build(Card, Lore, Scene, State, History, PlayerLine, Scene.Chapter);
		Request.FallbackScene = SceneRow;
		Request.FallbackEmotion = LastEmotion;
		for (const FBeatDef& Beat : Scene.Beats)
		{
			if (Beat.bRequired && !ReachedBeats.Contains(Beat.Id)) { Request.FallbackBeat = Beat.Id; break; }
		}
		FConversationTurn Turn;
		Turn.bFromPlayer = true;
		Turn.Text = PlayerLine;
		History.Add(Turn);

		const UAshesAISettings* Settings = GetDefault<UAshesAISettings>();
		ULocalModelSubsystem* Local = GEngine->GetEngineSubsystem<ULocalModelSubsystem>();
		Provider()->Model = Settings->ModelAlias;
		Provider()->TimeoutSeconds = Settings->GetEffectiveRequestTimeout();

		FAshesValidationContext Context = FAshesValidationContext::FromCard(Card, Scene);
		bBusy = true;

		auto Send = [Request, Card, Context, Local]()
		{
			Provider()->BaseUrl = Local->GetBaseUrl();
			SendAndValidate(Request, Card, Context, 1);
		};

		if (Local->IsReady() || Local->GetState() == ELocalModelState::Failed)
		{
			Send();
			return;
		}

		UE_LOG(LogAshes, Display, TEXT("TalkZafir: starting the built-in model first..."));
		Local->StartServer();
		TSharedRef<FDelegateHandle> Handle = MakeShared<FDelegateHandle>();
		*Handle = Local->OnStateChanged.AddLambda([Local, Send, Handle](ELocalModelState NewState)
		{
			if (NewState == ELocalModelState::Ready || NewState == ELocalModelState::Failed)
			{
				// Copy first: Remove() destroys this lambda and its captures while it is still running
				const auto SendCopy = Send;
				Local->OnStateChanged.Remove(*Handle);
				SendCopy();
			}
		});
	}
}

static FAutoConsoleCommand GAshesTalkZafir(
	TEXT("Ashes.TalkZafir"),
	TEXT("Talk to Zafir through the real prompt builder + validator. Usage: Ashes.TalkZafir <line>  (Ashes.TalkZafir.Reset starts over)"),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		AshesTalk::Talk(FString::Join(Args, TEXT(" ")));
	}));

static FAutoConsoleCommand GAshesTalkZafirReset(
	TEXT("Ashes.TalkZafir.Reset"),
	TEXT("Forget the test conversation with Zafir"),
	FConsoleCommandDelegate::CreateLambda([]()
	{
		AshesTalk::History.Reset();
		AshesTalk::State = FPromptState();
		AshesTalk::ReachedBeats.Reset();
		AshesTalk::LastEmotion = NAME_None;
		AshesTalk::bBusy = false;
		UE_LOG(LogAshes, Display, TEXT("TalkZafir: conversation reset"));
	}));
