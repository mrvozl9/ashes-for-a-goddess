// AI-assisted: Claude Code, 11.11.2026, runs one conversation scene: prompt, model, validator, turn limit (Phase 5; full state machine in Phase 7); beats, clue hints, fallback context (Phase 4 data, 12.11.2026); soft / hard limit with hesitation; Phase 7: state machine (FSceneFlow), NPC brain, offline tree, word-by-word reply (13.11.2026); Phase 6 fixes 4, 7. Reviewed by <pending>.

#include "SceneDirectorSubsystem.h"
#include "Ashes.h"
#include "AshesGameInstance.h"
#include "AshesAISettings.h"
#include "AshesNPC.h"
#include "AshesCharacter.h"
#include "AshesPlayerController.h"
#include "ConversationWidget.h"
#include "PromptBuilder.h"
#include "ResponseValidator.h"
#include "NPCBrainComponent.h"
#include "OfflineLLMProvider.h"
#include "ReplyStream.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "HAL/PlatformTime.h"

namespace
{
	const TCHAR* GCharactersPath = TEXT("/Game/Ashes/Data/DT_Characters.DT_Characters");
	const TCHAR* GScenesPath = TEXT("/Game/Ashes/Data/DT_Scenes.DT_Scenes");
	const TCHAR* GLorePath = TEXT("/Game/Ashes/Data/DA_LoreBible.DA_LoreBible");
	const TCHAR* GCluesPath = TEXT("/Game/Ashes/Data/DT_Clues.DT_Clues");
	const TCHAR* GSceneRulesPath = TEXT("/Game/Ashes/Data/DT_SceneRules.DT_SceneRules");
	const TCHAR* GThresholdsPath = TEXT("/Game/Ashes/Data/DT_Thresholds.DT_Thresholds");
	const TCHAR* GOfflineTreesPath = TEXT("/Game/Ashes/Data/DT_OfflineTrees.DT_OfflineTrees");

	// All rows of an optional table; empty when the asset does not exist yet or has another row type
	template <typename RowType>
	TArray<RowType> LoadRows(const TCHAR* Path)
	{
		TArray<RowType> Out;
		const UDataTable* Table = LoadObject<UDataTable>(nullptr, Path, nullptr, LOAD_Quiet | LOAD_NoWarn);
		if (Table && Table->GetRowStruct() && Table->GetRowStruct()->IsChildOf(RowType::StaticStruct()))
		{
			TArray<RowType*> Rows;
			Table->GetAllRows<RowType>(TEXT("SceneDirector"), Rows);
			for (const RowType* Row : Rows)
			{
				Out.Add(*Row);
			}
		}
		return Out;
	}

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
	SceneRowName = Npc->SceneRowName;
	CharacterRowName = Npc->CharacterRowName;
	LastEmotion = NAME_None;
	PendingHints.Reset();
	bTreeEnded = false;

	// The rules of this scene (DT_SceneRules is optional: without it every beat counts at any turn)
	Flow.Start(SceneRowName, Scene, LoadRows<FSceneRuleRow>(GSceneRulesPath));

	// The character's values live on the NPC, so they carry over to the next scene (the card only gives the start)
	UNPCBrainComponent* NpcBrain = Npc->FindComponentByClass<UNPCBrainComponent>();
	if (!NpcBrain)
	{
		NpcBrain = NewObject<UNPCBrainComponent>(Npc, TEXT("NPCBrain"));
		NpcBrain->RegisterComponent();
	}
	NpcBrain->InitFromCard(CharacterRowName, Card, LoadRows<FValueThresholdRow>(GThresholdsPath));
	NpcBrain->OnThresholdReached.AddUniqueDynamic(this, &USceneDirectorSubsystem::HandleThresholdReached);
	Brain = NpcBrain;

	State = FPromptState();
	State.Values = NpcBrain->GetValues();

	// Offline answers for this scene, if written (used only while the model is not available)
	if (!Tree.Start(LoadRows<FOfflineNodeRow>(GOfflineTreesPath), SceneRowName))
	{
		Tree.Reset();
	}

	// Talk clues this character holds in this chapter: their hints go into the prompt (DT_Clues)
	if (const UDataTable* Clues = LoadObject<UDataTable>(nullptr, GCluesPath, nullptr, LOAD_Quiet | LOAD_NoWarn))
	{
		TArray<FClueRow*> ClueRows;
		Clues->GetAllRows<FClueRow>(TEXT("StartScene"), ClueRows);
		for (const FClueRow* Clue : ClueRows)
		{
			if (Clue->Chapter == Scene.Chapter && Clue->Holder == Card.DisplayName && Clue->Source.StartsWith(TEXT("talk")) && !Clue->PromptHint.IsEmpty())
			{
				State.ClueHints.Add(Clue->PromptHint);
			}
		}
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

	UAshesGameInstance* GI = GetWorld()->GetGameInstance<UAshesGameInstance>();
	if (GI)
	{
		AIModeHandle = GI->OnAIModeChanged.AddUObject(this, &USceneDirectorSubsystem::HandleAIModeChanged);
	}

	// Reply buttons for the first line: offline the tree's replies (or the stock lines), online the stock lines
	{
		FLLMResponse First;
		First.Emotion = Card.FallbackEmotion;
		if (IsTypingAllowed())
		{
			TopUpSuggestions(First.Suggestions, GetStockSuggestions());
		}
		else
		{
			First.Suggestions = GetChoicesWithoutModel(); // exactly the tree's replies, or the stock lines
		}
		LastSuggestions = First.Suggestions;
		OnReplyReady.Broadcast(First, true);
	}

	// Let the model read the card, lore and scene while the player reads the opener
	if (GI)
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

	if (const UNPCBrainComponent* NpcBrain = Brain.Get())
	{
		State.Values = NpcBrain->GetValues(); // the prompt shows them as words
	}
	FLLMRequest Request = UPromptBuilder::Build(Card, Lore, Scene, State, History, Line, Scene.Chapter);

	// Nudge (2 turns left or 3 turns without a new beat): a steering note rides on the newest line only,
	// so the cached part of the prompt stays the same and the note never enters the history
	const FString NudgeNote = Flow.GetNudgeNote();
	if (!NudgeNote.IsEmpty() && Request.Messages.Num() > 0)
	{
		Request.Messages.Last().Content += FString::Printf(TEXT("\n\n[Director's note, not spoken by Sahra: %s]"), *NudgeNote);
		UE_LOG(LogAshes, Log, TEXT("Scene: nudge toward '%s'"), *Flow.GetPendingBeat().ToString());
	}

	FillFallbackContext(Request);
	FConversationTurn Turn;
	Turn.bFromPlayer = true;
	Turn.Text = Line;
	History.Add(Turn);
	++TurnsTaken;
	Flow.BeginTurn();
	OnTurnsChanged.Broadcast(GetTurnsLeft());

	bWaiting = true;
	RequestStartTime = FPlatformTime::Seconds();
	bFirstWordsLogged = false;
	OnThinkingChanged.Broadcast(true);
	++RequestSerial;

	// Offline: the tree answers if the line is (or clearly means) one of its replies; else a fallback line below
	if (TryAnswerFromTree(Line))
	{
		return;
	}
	UE_LOG(LogAshes, Log, TEXT("Scene: waiting (soft limit %.0f s, first words by %.0f s, hard limit %.0f s%s)"), GetDefault<UAshesAISettings>()->GetHesitateAfterSeconds(),
		GetDefault<UAshesAISettings>()->FirstWordsTimeoutSeconds, GetDefault<UAshesAISettings>()->GetEffectiveRequestTimeout(), GIsEditor ? TEXT(", editor") : TEXT(""));
	PendingRequest = Request;
	RequestReply(Request, 1); // 1 silent retry on a broken / spoiling reply (GDD 8.1)

	// Soft limit: after RequestTimeoutSeconds (6 s) Zafir visibly hesitates, but we keep waiting up to the hard limit
	UAshesGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance<UAshesGameInstance>() : nullptr;
	if (GI && GI->GetAIMode() == EAIMode::Online)
	{
		TWeakObjectPtr<USceneDirectorSubsystem> WeakThis(this);
		const int32 ReqSerial = RequestSerial;
		GetWorld()->GetTimerManager().SetTimer(HesitateTimer, FTimerDelegate::CreateLambda([WeakThis, ReqSerial]()
		{
			USceneDirectorSubsystem* Self = WeakThis.Get();
			if (Self && ReqSerial == Self->RequestSerial && Self->bWaiting)
			{
				Self->ShowHesitation();
			}
		}), GetDefault<UAshesAISettings>()->GetHesitateAfterSeconds(), false);

		// Thinking limit: no words on screen by FirstWordsTimeoutSeconds (10 s) -> stop the model, the character dodges
		GetWorld()->GetTimerManager().SetTimer(FirstWordsTimer, FTimerDelegate::CreateLambda([WeakThis, ReqSerial]()
		{
			USceneDirectorSubsystem* Self = WeakThis.Get();
			if (Self && ReqSerial == Self->RequestSerial && Self->bWaiting)
			{
				Self->HandleFirstWordsTimeout();
			}
		}), GetDefault<UAshesAISettings>()->FirstWordsTimeoutSeconds, false);
	}
}

void USceneDirectorSubsystem::HandleFirstWordsTimeout()
{
	if (bFirstWordsLogged || !LiveReply.IsEmpty())
	{
		return; // the words are coming: the reply may finish up to the hard limit
	}
	const float Limit = GetDefault<UAshesAISettings>()->FirstWordsTimeoutSeconds;
	UE_LOG(LogAshes, Log, TEXT("Scene: no words after %.0f s: the model is stopped"), Limit);
	// The dodge first (it ends the wait), then stop the model: its callback then finds the line answered and is dropped
	HesitateInsteadOfReply(PendingRequest, FString::Printf(TEXT("no words within %.0f s"), Limit));
	if (UAshesGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance<UAshesGameInstance>() : nullptr)
	{
		GI->CancelModelRequests();
	}
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
	const int32 ReqSerial = RequestSerial;
	TWeakObjectPtr<USceneDirectorSubsystem> WeakThis(this);

	// Word by word (Phase 7): a new attempt (also the silent retry) starts with an empty live line
	ClearLiveReply();
	bLiveBlocked = false;
	FOnLLMPartial Partial = FOnLLMPartial::CreateLambda([WeakThis, Serial, ReqSerial](const FString& TextSoFar)
	{
		USceneDirectorSubsystem* Self = WeakThis.Get();
		if (Self && Serial == GSceneSerial && Self->bActive && ReqSerial == Self->RequestSerial && Self->bWaiting)
		{
			Self->HandlePartialText(TextSoFar);
		}
	});

	GI->RequestReply(Request, FOnLLMReply::CreateLambda([WeakThis, Serial, ReqSerial, Request, AttemptsLeft](const FLLMReply& Reply)
	{
		USceneDirectorSubsystem* Self = WeakThis.Get();
		if (!Self || Serial != GSceneSerial || !Self->bActive)
		{
			return; // scene was closed meanwhile
		}
		if (ReqSerial != Self->RequestSerial || !Self->bWaiting)
		{
			UE_LOG(LogAshes, Log, TEXT("Scene: late reply dropped (%.1f s)"), Reply.LatencySeconds);
			return; // this line was already answered (e.g. by a dodge): exactly one Zafir line per player line
		}

		if (Reply.bFromFallback)
		{
			if (!Reply.Error.IsEmpty())
			{
				// Online but too slow / broken: a dodge that fits any question, not an unrelated answer
				Self->HesitateInsteadOfReply(Request, Reply.Error);
				return;
			}
			// Offline mode: a pre-written Content line is the answer
			const FAshesValidationResult Parsed = UResponseValidator::Validate(Reply.Text, FAshesValidationContext::FromCard(Self->Card, Self->Scene));
			UE_LOG(LogAshes, Log, TEXT("Scene: offline line"));
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
			UE_LOG(LogAshes, Log, TEXT("Scene: reply in %.1f s (prompt %d tokens, %d from cache, read in %.1f s; %d tokens at %.0f tok/s)"),
				Reply.LatencySeconds, Reply.PromptTokens + Reply.CachedTokens, Reply.CachedTokens, Reply.PromptMs / 1000.f, Reply.CompletionTokens, Reply.TokensPerSecond);
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
	}), Partial);
}

void USceneDirectorSubsystem::HandlePartialText(const FString& TextSoFar)
{
	if (bLiveBlocked)
	{
		return;
	}
	const FString Reply = AshesReplyStream::ExtractReply(TextSoFar);
	if (Reply.TrimStartAndEnd().IsEmpty())
	{
		return;
	}
	const FAshesValidationContext Context = FAshesValidationContext::FromCard(Card, Scene);
	if (AshesReplyStream::IsBlocked(Reply, Context))
	{
		// A locked spoiler or unsafe word is being written: take it off the screen at once; the validator will
		// reject the finished reply and ask again (or the character dodges)
		UE_LOG(LogAshes, Log, TEXT("Scene: live reply hidden (blocked word while writing)"));
		bLiveBlocked = true;
		ClearLiveReply();
		return;
	}
	// Whole words only, and nothing that could be the start of a locked phrase
	const FString Shown = AshesReplyStream::SafeToShow(Reply, Context);
	if (Shown.IsEmpty() || Shown == LiveReply)
	{
		return;
	}
	if (!bFirstWordsLogged)
	{
		bFirstWordsLogged = true;
		// GDD 13.5 early warning: the first words should appear within 4 s on average (measured in the game window)
		UE_LOG(LogAshes, Log, TEXT("Scene: first words after %.1f s"), FPlatformTime::Seconds() - RequestStartTime);
	}
	LiveReply = Shown;
	OnReplyPartial.Broadcast(LiveReply);
}

void USceneDirectorSubsystem::ClearLiveReply()
{
	if (!LiveReply.IsEmpty())
	{
		LiveReply.Reset();
		OnReplyPartial.Broadcast(FString());
	}
}

void USceneDirectorSubsystem::UseFallback(const FLLMRequest& Request)
{
	// Broken JSON or a spoiler twice: the model is online, so Zafir dodges instead of giving an unrelated line
	HesitateInsteadOfReply(Request, TEXT("invalid reply twice"));
}

void USceneDirectorSubsystem::ShowHesitation()
{
	// Status only ("Zafir hesitates..."): no dialogue line, the real answer may still come
	if (!LiveReply.IsEmpty())
	{
		return; // the words are already appearing: no need to say he hesitates
	}
	UE_LOG(LogAshes, Log, TEXT("Scene: soft limit reached after %.1f s, still waiting"), FPlatformTime::Seconds() - RequestStartTime);
	OnHesitating.Broadcast();
}

void USceneDirectorSubsystem::HesitateInsteadOfReply(const FLLMRequest& Request, const FString& Reason)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(HesitateTimer);
		GetWorld()->GetTimerManager().ClearTimer(FirstWordsTimer);
	}
	ClearLiveReply(); // the words shown so far will not be his answer
	UE_LOG(LogAshes, Log, TEXT("Scene: no answer (%s): hesitate line, turn not counted"), *Reason);

	UAshesGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance<UAshesGameInstance>() : nullptr;
	if (!GI)
	{
		FinishHesitation(FLLMResponse());
		return;
	}
	FLLMRequest Hesitate = Request;
	Hesitate.bHesitate = true;
	const int32 Serial = GSceneSerial;
	const int32 ReqSerial = RequestSerial;
	TWeakObjectPtr<USceneDirectorSubsystem> WeakThis(this);
	GI->RequestFallback(Hesitate, FOnLLMReply::CreateLambda([WeakThis, Serial, ReqSerial](const FLLMReply& Reply)
	{
		USceneDirectorSubsystem* Self = WeakThis.Get();
		if (Self && Serial == GSceneSerial && Self->bActive && ReqSerial == Self->RequestSerial && Self->bWaiting)
		{
			Self->FinishHesitation(UResponseValidator::Validate(Reply.Text, FAshesValidationContext()).Response);
		}
	}));
}

void USceneDirectorSubsystem::RefundUnansweredTurn(int32& InOutTurnsTaken, TArray<FConversationTurn>& InOutHistory)
{
	InOutTurnsTaken = FMath::Max(0, InOutTurnsTaken - 1);
	if (InOutHistory.Num() > 0 && InOutHistory.Last().bFromPlayer)
	{
		InOutHistory.Pop(); // the model never answered it; it must not dangle in the next prompt
	}
}

void USceneDirectorSubsystem::FinishHesitation(const FLLMResponse& Hesitation)
{
	RefundUnansweredTurn(TurnsTaken, History);
	Flow.RefundTurn();
	bWaiting = false;
	OnThinkingChanged.Broadcast(false);
	// Values unchanged; the previous suggestions stay; the player can ask again
	UE_LOG(LogAshes, Log, TEXT("Scene: %s dodges: %s [%s]"), *Card.DisplayName, *Hesitation.Reply, *Hesitation.Gesture);
	PostFallbackLine(Hesitation);
	if (!IsTypingAllowed())
	{
		// The AI went offline while this line waited: the model's old suggestions would get unrelated pre-written answers
		FLLMResponse Choices;
		Choices.Emotion = LastEmotion.IsNone() ? Card.FallbackEmotion : LastEmotion;
		Choices.Suggestions = GetChoicesWithoutModel();
		LastSuggestions = Choices.Suggestions;
		OnReplyReady.Broadcast(Choices, true);
	}
	OnTurnsChanged.Broadcast(GetTurnsLeft());
}

void USceneDirectorSubsystem::PostFallbackLine(const FLLMResponse& Line)
{
	const bool bSilent = Line.Reply.IsEmpty() || Line.Reply == TEXT("...");
	if (!bSilent || Line.Gesture.IsEmpty())
	{
		OnLinePosted.Broadcast(Card.DisplayName, bSilent ? FString(TEXT("...")) : Line.Reply);
	}
	if (!Line.Gesture.IsEmpty())
	{
		OnLinePosted.Broadcast(FString(), FString::Printf(TEXT("(%s)"), *Line.Gesture));
	}
}

bool USceneDirectorSubsystem::TryAnswerFromTree(const FString& Line)
{
	const UAshesGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance<UAshesGameInstance>() : nullptr;
	if (!Tree.IsActive() || (GI && GI->GetAIMode() == EAIMode::Online))
	{
		return false;
	}
	const int32 Index = Tree.MatchReply(Line);
	if (Index < 0)
	{
		return false; // free text the tree does not know: a fallback line answers, the node stays
	}

	// The clue journal comes in Phase 9; until then a "requires clue" rule uses its stand-in
	const FOfflineStep Step = Tree.Pick(Index, State.Values, TSet<FName>());
	if (!Step.bValid)
	{
		return false;
	}

	FLLMResponse Response;
	UOfflineLLMProvider::SplitGesture(Step.Line, Response.Reply, Response.Gesture);
	Response.Emotion = Step.Emotion.IsNone() ? Card.FallbackEmotion : Step.Emotion;
	Response.Deltas = Step.Changes;
	Response.Beats = Step.Beats;
	Response.Clues = Step.Clues;
	Response.Suggestions = Step.NextReplies;
	bTreeEnded = Step.bEnded;
	UE_LOG(LogAshes, Log, TEXT("Scene: offline tree reply %d%s -> node %d%s"), Index + 1, Step.bStandIn ? TEXT(" (stand-in)") : TEXT(""),
		Tree.GetNode(), Step.bEnded ? TEXT(" (end)") : TEXT(""));
	DeliverReply(Response, true, true);
	return true;
}

void USceneDirectorSubsystem::HandleThresholdReached(FName UnlockId, const FString& HintLine)
{
	UE_LOG(LogAshes, Log, TEXT("Scene: unlocked '%s'"), *UnlockId.ToString());
	if (!HintLine.IsEmpty())
	{
		PendingHints.Add(HintLine); // shown after the reply that caused it; never a number
	}
}

void USceneDirectorSubsystem::HandleAIModeChanged()
{
	if (!bActive || bFinished)
	{
		return;
	}
	UE_LOG(LogAshes, Log, TEXT("Scene: AI is now %s"), IsTypingAllowed() ? TEXT("online (typing allowed)") : TEXT("offline (pick a reply)"));
	if (!IsTypingAllowed() && !bWaiting)
	{
		// The model's last suggestions would get unrelated pre-written answers: offer the tree's replies or the stock lines
		FLLMResponse Choices;
		Choices.Emotion = LastEmotion.IsNone() ? Card.FallbackEmotion : LastEmotion;
		Choices.Suggestions = GetChoicesWithoutModel();
		LastSuggestions = Choices.Suggestions;
		OnReplyReady.Broadcast(Choices, true);
	}
	OnTurnsChanged.Broadcast(GetTurnsLeft()); // the screen shows or hides the text box
}

bool USceneDirectorSubsystem::IsTypingAllowed() const
{
	const UAshesGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance<UAshesGameInstance>() : nullptr;
	return GI && GI->GetAIMode() == EAIMode::Online;
}

void USceneDirectorSubsystem::TopUpSuggestions(TArray<FString>& Suggestions, const TArray<FString>& Stock, const FString& AvoidLine)
{
	auto Has = [&Suggestions](const FString& Line)
	{
		return Suggestions.ContainsByPredicate([&Line](const FString& S) { return S.TrimStartAndEnd().Equals(Line, ESearchCase::IgnoreCase); });
	};
	const FString Avoid = AvoidLine.TrimStartAndEnd();
	for (const FString& Line : Stock)
	{
		if (Suggestions.Num() >= SuggestionCount)
		{
			break;
		}
		const FString Trimmed = Line.TrimStartAndEnd();
		if (!Trimmed.IsEmpty() && !Has(Trimmed) && !Trimmed.Equals(Avoid, ESearchCase::IgnoreCase))
		{
			Suggestions.Add(Trimmed);
		}
	}
}

TArray<FString> USceneDirectorSubsystem::GetStockSuggestions() const
{
	TArray<FString> Stock = Flow.GetStockSuggestions();
	// Last resort when the scene has no stock lines yet: neutral lines that fit any conversation
	Stock.Append({ TEXT("Tell me more."), TEXT("Why do you say that?"), TEXT("Let's talk about something else.") });
	return Stock;
}

TArray<FString> USceneDirectorSubsystem::GetChoicesWithoutModel() const
{
	if (Tree.IsActive())
	{
		return Tree.GetReplies();
	}
	TArray<FString> Choices;
	const FString PlayerLine = (History.Num() > 0 && History.Last().bFromPlayer) ? History.Last().Text : FString();
	TopUpSuggestions(Choices, GetStockSuggestions(), PlayerLine);
	return Choices;
}

UNPCBrainComponent* USceneDirectorSubsystem::GetBrain() const
{
	return bActive ? Brain.Get() : nullptr;
}

FString USceneDirectorSubsystem::GetStatusText() const
{
	if (!bActive)
	{
		return TEXT("no scene");
	}
	TArray<FString> Beats;
	for (const FName& Beat : Flow.GetReachedBeats())
	{
		Beats.Add(Beat.ToString());
	}
	TArray<FString> Values;
	for (const TPair<FName, int32>& Value : State.Values)
	{
		Values.Add(FString::Printf(TEXT("%s %d (%s)"), *Value.Key.ToString(), Value.Value, *UPromptBuilder::ValueToWord(Value.Value)));
	}
	return FString::Printf(TEXT("%s: phase %s, turn %d of %d, stalled %d, beats [%s], next beat %s, values [%s], offline tree %s"),
		*SceneRowName.ToString(), *UEnum::GetDisplayValueAsText(Flow.GetPhase()).ToString(), Flow.GetTurnsTaken(), Scene.TurnLimit, Flow.GetStalledTurns(),
		*FString::Join(Beats, TEXT(", ")), *Flow.GetPendingBeat().ToString(), *FString::Join(Values, TEXT(", ")),
		Tree.IsActive() ? *FString::Printf(TEXT("node %d"), Tree.GetNode()) : TEXT("none"));
}

void USceneDirectorSubsystem::CloseScene()
{
	bFinished = true;
	const EScenePhase Phase = Flow.GetPhase();
	UE_LOG(LogAshes, Log, TEXT("Scene: closed as %s after %d turns"), *UEnum::GetDisplayValueAsText(Phase).ToString(), TurnsTaken);

	if (Phase == EScenePhase::Caught)
	{
		OnTurnsChanged.Broadcast(GetTurnsLeft());
		OnCaught.Broadcast();
		return;
	}

	// Goal reached: the full closing (the romance line on that route, if written); turn limit: the weaker one
	FString Closing;
	if (Flow.WasFull())
	{
		const UNPCBrainComponent* NpcBrain = Brain.Get();
		const FString Romance = Flow.GetClosingRomance();
		Closing = (NpcBrain && NpcBrain->GetRoute() == EAshesRoute::Romance && !Romance.IsEmpty()) ? Romance : Scene.ClosingFull;
	}
	else
	{
		Closing = Scene.ClosingWeaker.IsEmpty() ? Scene.ClosingFull : Scene.ClosingWeaker;
		for (const FName& Beat : Flow.GetForcedOnClose())
		{
			UE_LOG(LogAshes, Log, TEXT("Scene: beat set by the Director so the story goes on: %s"), *Beat.ToString());
		}
	}

	if (Closing.IsEmpty())
	{
		OnLinePosted.Broadcast(FString(), TEXT("(The conversation is over.)"));
	}
	else
	{
		FLLMResponse Line;
		UOfflineLLMProvider::SplitGesture(Closing, Line.Reply, Line.Gesture);
		PostFallbackLine(Line);
	}
	OnTurnsChanged.Broadcast(0);
	OnSceneResolved.Broadcast(Flow.WasFull());
}

void USceneDirectorSubsystem::DeliverReply(const FLLMResponse& InResponse, bool bFromFallback, bool bAuthored)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(HesitateTimer);
		GetWorld()->GetTimerManager().ClearTimer(FirstWordsTimer);
	}

	// Keep the typing indicator up for at least 0.4 s (fallback lines arrive instantly)
	const float Elapsed = static_cast<float>(FPlatformTime::Seconds() - RequestStartTime);
	const float Delay = MinThinkingSeconds - Elapsed;
	if (Delay > 0.f && GetWorld())
	{
		const int32 Serial = GSceneSerial;
		TWeakObjectPtr<USceneDirectorSubsystem> WeakThis(this);
		FLLMResponse Copy = InResponse;
		GetWorld()->GetTimerManager().SetTimer(ThinkingTimer, FTimerDelegate::CreateLambda([WeakThis, Serial, Copy, bFromFallback, bAuthored]()
		{
			if (USceneDirectorSubsystem* Self = WeakThis.Get())
			{
				if (Serial == GSceneSerial && Self->bActive)
				{
					Self->RequestStartTime = 0.0; // show now
					Self->DeliverReply(Copy, bFromFallback, bAuthored);
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
	ClearLiveReply(); // the checked, final line replaces the words shown while writing

	// Values and beats change on real model replies and on offline tree answers;
	// a fallback line leaves them unchanged (GDD 8.1)
	const bool bCounts = !bFromFallback || bAuthored;
	if (bCounts)
	{
		for (const TPair<FName, int32>& Delta : Response.Deltas)
		{
			UNPCBrainComponent* NpcBrain = Brain.Get();
			if (NpcBrain && NpcBrain->HasValue(Delta.Key))
			{
				// The brain clamps to the card's per-turn limits and to 0-100, and reports thresholds
				const int32 Value = NpcBrain->ApplyDelta(Delta.Key, Delta.Value);
				UE_LOG(LogAshes, Log, TEXT("Scene: %s %+d -> %d"), *Delta.Key.ToString(), Delta.Value, Value);
			}
		}
		if (const UNPCBrainComponent* NpcBrain = Brain.Get())
		{
			State.Values = NpcBrain->GetValues();
		}
	}

	// The reply only claims beats; the scene's rules decide (order, earliest turn, values). Forced beats come from here too.
	const TArray<FName> Accepted = Flow.ApplyReply(bCounts ? Response.Beats : TArray<FName>(), State.Values, bAuthored);
	for (const FName& Beat : Accepted)
	{
		UE_LOG(LogAshes, Log, TEXT("Scene: beat reached: %s"), *Beat.ToString());
	}
	if (bCounts)
	{
		for (const FName& Beat : Response.Beats)
		{
			if (!Accepted.Contains(Beat))
			{
				UE_LOG(LogAshes, Log, TEXT("Scene: beat '%s' not accepted (already reached or its rule is not met yet)"), *Beat.ToString());
			}
		}
	}
	Response.Beats = Accepted;
	if (bAuthored && bTreeEnded)
	{
		Flow.EndEarly(); // the tree has no more nodes
		bTreeEnded = false;
	}

	// Always 3 reply buttons (Phase 6 fix 4): offline without a tree answer, the tree's replies or the stock lines;
	// a model reply with fewer than 3 suggestions is filled up from the stock lines. Tree answers keep their own replies.
	if (!bAuthored)
	{
		const FString PlayerLine = (History.Num() > 0 && History.Last().bFromPlayer) ? History.Last().Text : FString();
		// Offline: pick-only choices; also for a model reply that arrived just after the AI went offline
		const bool bPickOnly = bFromFallback || !IsTypingAllowed();
		if (bPickOnly)
		{
			Response.Suggestions = GetChoicesWithoutModel();
		}
		if (!(bPickOnly && Tree.IsActive()))
		{
			const int32 Before = Response.Suggestions.Num();
			TopUpSuggestions(Response.Suggestions, GetStockSuggestions(), PlayerLine);
			if (!bFromFallback && Response.Suggestions.Num() > Before)
			{
				UE_LOG(LogAshes, Log, TEXT("Scene: %d suggestion(s) from the model, filled up to %d"), Before, Response.Suggestions.Num());
			}
		}
	}

	FConversationTurn Turn;
	Turn.bFromPlayer = false;
	Turn.Text = Response.Reply;
	History.Add(Turn);
	LastSuggestions = Response.Suggestions;
	LastEmotion = Response.Emotion;

	bWaiting = false;
	OnThinkingChanged.Broadcast(false);
	UE_LOG(LogAshes, Log, TEXT("Scene: %s [%s]%s: %s"), *Card.DisplayName, *Response.Emotion.ToString(),
		bAuthored ? TEXT(" (offline tree)") : bFromFallback ? TEXT(" (fallback)") : TEXT(""), *Response.Reply);
	OnLinePosted.Broadcast(Card.DisplayName, Response.Reply);
	OnReplyReady.Broadcast(Response, bFromFallback);

	// Hints instead of numbers: the lines of the thresholds this reply reached
	for (const FString& Hint : PendingHints)
	{
		OnLinePosted.Broadcast(FString(), Hint);
	}
	PendingHints.Reset();

	if (Flow.IsOver())
	{
		CloseScene();
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
		GetWorld()->GetTimerManager().ClearTimer(HesitateTimer);
		GetWorld()->GetTimerManager().ClearTimer(FirstWordsTimer);
	}

	if (UNPCBrainComponent* NpcBrain = Brain.Get())
	{
		NpcBrain->OnThresholdReached.RemoveDynamic(this, &USceneDirectorSubsystem::HandleThresholdReached);
	}
	PendingHints.Reset();
	if (UAshesGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance<UAshesGameInstance>() : nullptr)
	{
		GI->OnAIModeChanged.Remove(AIModeHandle);
		GI->ClearPendingPrime(); // a warm-up still waiting for the model belongs to this scene
	}
	AIModeHandle.Reset();
	LiveReply.Reset();
	bLiveBlocked = false;
	Flow = FSceneFlow(); // back to Idle: GetPhase() must not report the old scene
	Tree.Reset();

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

void USceneDirectorSubsystem::FillFallbackContext(FLLMRequest& Request) const
{
	Request.FallbackScene = SceneRowName;
	Request.FallbackCharacter = CharacterRowName;
	Request.FallbackEmotion = LastEmotion;
	Request.FallbackBeat = Flow.GetPendingBeat(); // the next beat the scene is pushing toward
}

static FAutoConsoleCommandWithWorldAndArgs GAshesSceneStatus(
	TEXT("Ashes.Scene.Status"), TEXT("Logs the open conversation's phase, turns, beats and values"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World)
	{
		if (const USceneDirectorSubsystem* Director = GetDirectorFor(World)) { UE_LOG(LogAshes, Display, TEXT("Scene status: %s"), *Director->GetStatusText()); }
	}));
