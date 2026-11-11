// AI-assisted: Claude Code, 11.11.2026, runs one conversation scene: prompt, model, validator, turn limit (Phase 5; full state machine in Phase 7). Reviewed by <pending>.

#include "SceneDirectorSubsystem.h"
#include "Ashes.h"
#include "AshesGameInstance.h"
#include "AshesNPC.h"
#include "AshesCharacter.h"
#include "AshesPlayerController.h"
#include "ConversationWidget.h"
#include "PromptBuilder.h"
#include "ResponseValidator.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "HAL/PlatformTime.h"

namespace
{
	const TCHAR* GCharactersPath = TEXT("/Game/Ashes/Data/DT_Characters.DT_Characters");
	const TCHAR* GScenesPath = TEXT("/Game/Ashes/Data/DT_Scenes.DT_Scenes");
	const TCHAR* GLorePath = TEXT("/Game/Ashes/Data/DA_LoreBible.DA_LoreBible");

	// Bumped on every start / end so late model replies from a closed scene are ignored
	int32 GSceneSerial = 0;
}

bool USceneDirectorSubsystem::StartScene(AAshesNPC* Npc, AAshesCharacter* Sahra)
{
	if (bActive || !Npc || !Sahra)
	{
		return false;
	}

	const UDataTable* Characters = LoadObject<UDataTable>(nullptr, GCharactersPath);
	const UDataTable* Scenes = LoadObject<UDataTable>(nullptr, GScenesPath);
	const FCharacterCardRow* CardRow = Characters ? Characters->FindRow<FCharacterCardRow>(Npc->CharacterRowName, TEXT("StartScene")) : nullptr;
	const FSceneCardRow* SceneRow = Scenes ? Scenes->FindRow<FSceneCardRow>(Npc->SceneRowName, TEXT("StartScene")) : nullptr;
	if (!CardRow || !SceneRow)
	{
		UE_LOG(LogAshes, Error, TEXT("StartScene: no card '%s' or scene '%s' in /Game/Ashes/Data"), *Npc->CharacterRowName.ToString(), *Npc->SceneRowName.ToString());
		return false;
	}

	AAshesPlayerController* PC = Sahra->GetController<AAshesPlayerController>();
	TSubclassOf<UConversationWidget> WidgetClass = PC ? PC->GetConversationWidgetClass() : nullptr;
	if (!WidgetClass)
	{
		UE_LOG(LogAshes, Error, TEXT("StartScene: PC_Ashes has no Conversation Widget Class (WBP_Conversation)"));
		return false;
	}

	Card = *CardRow;
	Scene = *SceneRow;
	const UAshesLoreBible* LoreAsset = LoadObject<UAshesLoreBible>(nullptr, GLorePath);
	Lore = LoreAsset ? LoreAsset->Text : FString();

	State = FPromptState();
	for (const FRelationshipValueDef& Def : Card.Values)
	{
		State.Values.Add(Def.Name, Def.Start);
	}
	History.Reset();
	LastSuggestions.Reset();
	TurnsTaken = 0;
	bWaiting = false;
	bFinished = false;
	CurrentNpc = Npc;
	CurrentSahra = Sahra;
	++GSceneSerial;

	// Screen + input: Sahra stops, the mouse and keyboard go to the conversation screen
	Widget = CreateWidget<UConversationWidget>(PC, WidgetClass);
	Widget->InitConversation(this);
	Widget->AddToViewport(10);

	Sahra->SetGameplayInputEnabled(false);
	PC->SetShowMouseCursor(true);
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(Widget->GetFocusTarget());
	PC->SetInputMode(InputMode);

	bActive = true;
	UE_LOG(LogAshes, Log, TEXT("Scene start: %s (%s), %d turns"), *Card.DisplayName, *Npc->SceneRowName.ToString(), Scene.TurnLimit);

	FConversationTurn Opener;
	Opener.bFromPlayer = false;
	Opener.Text = Scene.Opener;
	History.Add(Opener);
	OnLinePosted.Broadcast(Card.DisplayName, Scene.Opener);
	OnTurnsChanged.Broadcast(GetTurnsLeft());

	// Let the model read the card, lore and scene while the player reads the opener
	if (UAshesGameInstance* GI = GetWorld()->GetGameInstance<UAshesGameInstance>())
	{
		GI->PrimeModel(UPromptBuilder::Build(Card, Lore, Scene, State, History, FString(), Scene.Chapter));
	}
	return true;
}

void USceneDirectorSubsystem::PickSuggestion(int32 Index)
{
	if (LastSuggestions.IsValidIndex(Index - 1))
	{
		SubmitLine(LastSuggestions[Index - 1]);
	}
}

void USceneDirectorSubsystem::SubmitLine(const FString& InLine)
{
	const FString Line = InLine.TrimStartAndEnd().Left(200); // GDD: max 200 characters
	if (!bActive || bWaiting || bFinished || Line.IsEmpty())
	{
		return;
	}

	UE_LOG(LogAshes, Log, TEXT("Scene: Sahra: %s"), *Line);
	OnLinePosted.Broadcast(TEXT("Sahra"), Line);

	const FLLMRequest Request = UPromptBuilder::Build(Card, Lore, Scene, State, History, Line, Scene.Chapter);
	FConversationTurn Turn;
	Turn.bFromPlayer = true;
	Turn.Text = Line;
	History.Add(Turn);
	++TurnsTaken;
	OnTurnsChanged.Broadcast(GetTurnsLeft());

	bWaiting = true;
	RequestStartTime = FPlatformTime::Seconds();
	OnThinkingChanged.Broadcast(true);
	RequestReply(Request, 1); // 1 silent retry on a broken / spoiling reply (GDD 8.1)
}

void USceneDirectorSubsystem::RequestReply(const FLLMRequest& Request, int32 AttemptsLeft)
{
	UAshesGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance<UAshesGameInstance>() : nullptr;
	if (!GI)
	{
		DeliverReply(FLLMResponse(), true);
		return;
	}

	const int32 Serial = GSceneSerial;
	TWeakObjectPtr<USceneDirectorSubsystem> WeakThis(this);
	GI->RequestReply(Request, FOnLLMReply::CreateLambda([WeakThis, Serial, Request, AttemptsLeft](const FLLMReply& Reply)
	{
		USceneDirectorSubsystem* Self = WeakThis.Get();
		if (!Self || Serial != GSceneSerial || !Self->bActive)
		{
			return; // scene was closed meanwhile
		}

		if (Reply.bFromFallback)
		{
			const FAshesValidationResult Parsed = UResponseValidator::Validate(Reply.Text, FAshesValidationContext());
			UE_LOG(LogAshes, Log, TEXT("Scene: fallback line (%s)"), *Reply.Error);
			Self->DeliverReply(Parsed.Response, true);
			return;
		}

		const FAshesValidationResult Result = UResponseValidator::Validate(Reply.Text, FAshesValidationContext::FromCard(Self->Card, Self->Scene));
		for (const FString& Note : Result.Notes)
		{
			UE_LOG(LogAshes, Log, TEXT("Scene: validator: %s"), *Note);
		}
		if (Result.IsOk())
		{
			UE_LOG(LogAshes, Log, TEXT("Scene: reply in %.1f s (%.0f tok/s)"), Reply.LatencySeconds, Reply.TokensPerSecond);
			Self->DeliverReply(Result.Response, false);
		}
		else if (AttemptsLeft > 0)
		{
			Self->RequestReply(Request, AttemptsLeft - 1);
		}
		else
		{
			Self->UseFallback(Request);
		}
	}));
}

void USceneDirectorSubsystem::UseFallback(const FLLMRequest& Request)
{
	UAshesGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance<UAshesGameInstance>() : nullptr;
	if (!GI)
	{
		DeliverReply(FLLMResponse(), true);
		return;
	}
	TWeakObjectPtr<USceneDirectorSubsystem> WeakThis(this);
	GI->RequestFallback(Request, FOnLLMReply::CreateLambda([WeakThis](const FLLMReply& Reply)
	{
		if (USceneDirectorSubsystem* Self = WeakThis.Get())
		{
			Self->DeliverReply(UResponseValidator::Validate(Reply.Text, FAshesValidationContext()).Response, true);
		}
	}));
}

void USceneDirectorSubsystem::DeliverReply(const FLLMResponse& InResponse, bool bFromFallback)
{
	// Keep the typing indicator up for at least 0.4 s (fallback lines arrive instantly)
	const float Elapsed = static_cast<float>(FPlatformTime::Seconds() - RequestStartTime);
	const float Delay = MinThinkingSeconds - Elapsed;
	if (Delay > 0.f && GetWorld())
	{
		const int32 Serial = GSceneSerial;
		TWeakObjectPtr<USceneDirectorSubsystem> WeakThis(this);
		FLLMResponse Copy = InResponse;
		GetWorld()->GetTimerManager().SetTimer(ThinkingTimer, FTimerDelegate::CreateLambda([WeakThis, Serial, Copy, bFromFallback]()
		{
			if (USceneDirectorSubsystem* Self = WeakThis.Get())
			{
				if (Serial == GSceneSerial && Self->bActive)
				{
					Self->RequestStartTime = 0.0; // show now
					Self->DeliverReply(Copy, bFromFallback);
				}
			}
		}), Delay, false);
		return;
	}

	FLLMResponse Response = InResponse;
	if (Response.Reply.IsEmpty())
	{
		Response.Reply = TEXT("...");
	}

	// Values change only on real model replies (fallback lines leave them unchanged, GDD 8.1)
	if (!bFromFallback)
	{
		for (const TPair<FName, int32>& Delta : Response.Deltas)
		{
			int32& Value = State.Values.FindOrAdd(Delta.Key);
			Value = FMath::Clamp(Value + Delta.Value, 0, 100);
			UE_LOG(LogAshes, Log, TEXT("Scene: %s %+d -> %d"), *Delta.Key.ToString(), Delta.Value, Value);
		}
	}

	FConversationTurn Turn;
	Turn.bFromPlayer = false;
	Turn.Text = Response.Reply;
	History.Add(Turn);
	LastSuggestions = Response.Suggestions;

	bWaiting = false;
	OnThinkingChanged.Broadcast(false);
	UE_LOG(LogAshes, Log, TEXT("Scene: %s [%s]%s: %s"), *Card.DisplayName, *Response.Emotion.ToString(), bFromFallback ? TEXT(" (fallback)") : TEXT(""), *Response.Reply);
	OnLinePosted.Broadcast(Card.DisplayName, Response.Reply);
	OnReplyReady.Broadcast(Response, bFromFallback);

	// Simple end at the turn limit (Phase 7: nudge, resolve, forced close)
	if (TurnsTaken >= Scene.TurnLimit)
	{
		bFinished = true;
		const FString Closing = Scene.ClosingFull.IsEmpty() ? TEXT("(The conversation is over.)") : Scene.ClosingFull;
		UE_LOG(LogAshes, Log, TEXT("Scene: turn limit reached"));
		OnLinePosted.Broadcast(Scene.ClosingFull.IsEmpty() ? FString() : Card.DisplayName, Closing);
		OnTurnsChanged.Broadcast(0);
	}
}

void USceneDirectorSubsystem::EndScene()
{
	if (!bActive)
	{
		return;
	}
	bActive = false;
	bWaiting = false;
	++GSceneSerial;
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(ThinkingTimer);
	}

	if (Widget)
	{
		Widget->RemoveFromParent();
		Widget = nullptr;
	}

	if (AAshesCharacter* Sahra = CurrentSahra.Get())
	{
		if (APlayerController* PC = Sahra->GetController<APlayerController>())
		{
			PC->SetShowMouseCursor(false);
			PC->SetInputMode(FInputModeGameOnly());
		}
		Sahra->SetGameplayInputEnabled(true);
	}

	UE_LOG(LogAshes, Log, TEXT("Scene end: %s after %d turns"), *Card.DisplayName, TurnsTaken);
	OnSceneEnded.Broadcast();
}

// Debug: drive the open conversation from the console (same paths as the screen's buttons)
#include "HAL/IConsoleManager.h"

static USceneDirectorSubsystem* GetDirectorFor(UWorld* World)
{
	return World ? World->GetSubsystem<USceneDirectorSubsystem>() : nullptr;
}

static FAutoConsoleCommandWithWorldAndArgs GAshesSceneSay(
	TEXT("Ashes.Scene.Say"), TEXT("Says a line in the open conversation. Usage: Ashes.Scene.Say <line>"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
	{
		if (USceneDirectorSubsystem* Director = GetDirectorFor(World)) { Director->SubmitLine(FString::Join(Args, TEXT(" "))); }
	}));

static FAutoConsoleCommandWithWorldAndArgs GAshesScenePick(
	TEXT("Ashes.Scene.Pick"), TEXT("Picks suggested reply 1-3 in the open conversation"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
	{
		if (USceneDirectorSubsystem* Director = GetDirectorFor(World)) { Director->PickSuggestion(Args.Num() ? FCString::Atoi(*Args[0]) : 1); }
	}));

static FAutoConsoleCommandWithWorldAndArgs GAshesSceneEnd(
	TEXT("Ashes.Scene.End"), TEXT("Closes the open conversation"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
	{
		if (USceneDirectorSubsystem* Director = GetDirectorFor(World)) { Director->EndScene(); }
	}));
