// AI-assisted: Claude Code, 11.11.2026, conversation screen logic; WBP_Conversation provides the layout (Phase 5); hesitation status (12.11.2026). Reviewed by <pending>.

#include "ConversationWidget.h"
#include "SceneDirectorSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/Image.h"
#include "Components/ScrollBox.h"
#include "Engine/Texture2D.h"

#define LOCTEXT_NAMESPACE "AshesConversation"

void UConversationWidget::InitConversation(USceneDirectorSubsystem* InDirector)
{
	Director = InDirector;
	if (!InDirector)
	{
		return;
	}
	InDirector->OnLinePosted.AddDynamic(this, &UConversationWidget::HandleLinePosted);
	InDirector->OnReplyReady.AddDynamic(this, &UConversationWidget::HandleReplyReady);
	InDirector->OnThinkingChanged.AddDynamic(this, &UConversationWidget::HandleThinking);
	InDirector->OnTurnsChanged.AddDynamic(this, &UConversationWidget::HandleTurns);
	InDirector->OnHesitating.AddDynamic(this, &UConversationWidget::HandleHesitating);

	NameText->SetText(FText::FromString(InDirector->GetSpeakerName()));
	ThinkingText->SetText(FText::Format(LOCTEXT("Thinking", "{0} is thinking..."), FText::FromString(InDirector->GetSpeakerName())));
	ThinkingText->SetVisibility(ESlateVisibility::Hidden);
	SetSuggestions({});
	SetEmotion(TEXT("neutral"));
}

TSharedPtr<SWidget> UConversationWidget::GetFocusTarget() const
{
	return InputBox ? InputBox->TakeWidget() : TSharedPtr<SWidget>();
}

void UConversationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Reply1->OnClicked.AddUniqueDynamic(this, &UConversationWidget::HandleReply1);
	Reply2->OnClicked.AddUniqueDynamic(this, &UConversationWidget::HandleReply2);
	Reply3->OnClicked.AddUniqueDynamic(this, &UConversationWidget::HandleReply3);
	SendButton->OnClicked.AddUniqueDynamic(this, &UConversationWidget::HandleSend);
	LeaveButton->OnClicked.AddUniqueDynamic(this, &UConversationWidget::HandleLeave);
	InputBox->OnTextCommitted.AddUniqueDynamic(this, &UConversationWidget::HandleTextCommitted);
	InputBox->OnTextChanged.AddUniqueDynamic(this, &UConversationWidget::HandleTextChanged);
	InputBox->SetHintText(LOCTEXT("InputHint", "Say something (Enter) or press 1 / 2 / 3"));
}

void UConversationWidget::HandleLinePosted(const FString& Speaker, const FString& Text)
{
	Lines.Add(Speaker.IsEmpty() ? Text : FString::Printf(TEXT("%s: %s"), *Speaker, *Text));
	RefreshDialogue();
}

void UConversationWidget::RefreshDialogue()
{
	const int32 First = FMath::Max(0, Lines.Num() - VisibleLines);
	TArray<FString> Visible;
	for (int32 Index = First; Index < Lines.Num(); ++Index)
	{
		Visible.Add(Lines[Index]);
	}
	DialogueText->SetText(FText::FromString(FString::Join(Visible, TEXT("\n\n"))));
	if (DialogueScroll)
	{
		DialogueScroll->ScrollToEnd();
	}
}

void UConversationWidget::HandleReplyReady(const FLLMResponse& Response, bool bFromFallback)
{
	if (!Response.Gesture.IsEmpty())
	{
		Lines.Add(FString::Printf(TEXT("(%s)"), *Response.Gesture));
		RefreshDialogue();
	}
	SetSuggestions(Response.Suggestions);
	SetEmotion(Response.Emotion.IsNone() ? FName(TEXT("neutral")) : Response.Emotion);
	UpdateInteractivity();
	InputBox->SetKeyboardFocus();
}

void UConversationWidget::HandleThinking(bool bInThinking)
{
	bThinking = bInThinking;
	if (bThinking && Director.IsValid())
	{
		ThinkingText->SetText(FText::Format(LOCTEXT("Thinking", "{0} is thinking..."), FText::FromString(Director->GetSpeakerName())));
	}
	ThinkingText->SetVisibility(bThinking ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	UpdateInteractivity();
}

void UConversationWidget::HandleHesitating()
{
	// Soft limit: only the status changes; the dialogue gets exactly one line per player line
	if (Director.IsValid())
	{
		ThinkingText->SetText(FText::Format(LOCTEXT("Hesitates", "{0} hesitates..."), FText::FromString(Director->GetSpeakerName())));
	}
}

void UConversationWidget::HandleTurns(int32 TurnsLeft)
{
	TurnsText->SetText(FText::Format(LOCTEXT("TurnsLeft", "Turns left: {0} · Tab: log"), FText::AsNumber(TurnsLeft)));
	UpdateInteractivity();
}

void UConversationWidget::SetSuggestions(const TArray<FString>& Suggestions)
{
	SuggestionCount = Suggestions.Num();
	UButton* Buttons[] = { Reply1, Reply2, Reply3 };
	UTextBlock* Texts[] = { Reply1Text, Reply2Text, Reply3Text };
	for (int32 i = 0; i < 3; ++i)
	{
		const bool bHas = Suggestions.IsValidIndex(i);
		Texts[i]->SetText(bHas ? FText::FromString(FString::Printf(TEXT("%d  %s"), i + 1, *Suggestions[i])) : FText::GetEmpty());
		Buttons[i]->SetVisibility(bHas ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UConversationWidget::SetEmotion(FName Emotion)
{
	const FString EmotionString = Emotion.ToString();
	if (EmotionText)
	{
		EmotionText->SetText(FText::FromString(EmotionString));
	}
	if (!PortraitImage || !Director.IsValid())
	{
		return;
	}

	// Real portraits later: /Game/Ashes/Art/Portraits/T_Portrait_<Name>_<Emotion> (design.md naming)
	const FString Name = Director->GetSpeakerName();
	const FString Capitalized = EmotionString.Left(1).ToUpper() + EmotionString.Mid(1);
	const FString Path = FString::Printf(TEXT("/Game/Ashes/Art/Portraits/T_Portrait_%s_%s.T_Portrait_%s_%s"), *Name, *Capitalized, *Name, *Capitalized);
	if (UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, *Path, nullptr, LOAD_Quiet | LOAD_NoWarn))
	{
		PortraitImage->SetBrushFromTexture(Texture);
		PortraitImage->SetColorAndOpacity(FLinearColor::White);
		return;
	}

	// Placeholder: one colour per emotion
	static const TMap<FName, FLinearColor> Colors =
	{
		{ TEXT("neutral"), FLinearColor(0.35f, 0.33f, 0.32f) },
		{ TEXT("teasing"), FLinearColor(0.58f, 0.36f, 0.10f) },
		{ TEXT("tender"),  FLinearColor(0.60f, 0.25f, 0.30f) },
		{ TEXT("jealous"), FLinearColor(0.20f, 0.40f, 0.15f) },
		{ TEXT("cold"),    FLinearColor(0.15f, 0.25f, 0.45f) },
		{ TEXT("angry"),   FLinearColor(0.55f, 0.08f, 0.06f) },
	};
	const FLinearColor* Color = Colors.Find(Emotion);
	PortraitImage->SetColorAndOpacity(Color ? *Color : FLinearColor::Gray);
}

void UConversationWidget::UpdateInteractivity()
{
	const bool bCanTalk = Director.IsValid() && !bThinking && !Director->IsFinished();
	InputBox->SetIsEnabled(bCanTalk);
	SendButton->SetIsEnabled(bCanTalk);
	Reply1->SetIsEnabled(bCanTalk);
	Reply2->SetIsEnabled(bCanTalk);
	Reply3->SetIsEnabled(bCanTalk);
}

void UConversationWidget::HandleReply1() { if (Director.IsValid()) Director->PickSuggestion(1); }
void UConversationWidget::HandleReply2() { if (Director.IsValid()) Director->PickSuggestion(2); }
void UConversationWidget::HandleReply3() { if (Director.IsValid()) Director->PickSuggestion(3); }

void UConversationWidget::HandleSend()
{
	if (Director.IsValid())
	{
		const FString Text = InputBox->GetText().ToString();
		InputBox->SetText(FText::GetEmpty());
		Director->SubmitLine(Text);
	}
}

void UConversationWidget::HandleLeave()
{
	if (Director.IsValid())
	{
		Director->EndScene();
	}
}

void UConversationWidget::HandleTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		HandleSend();
	}
}

void UConversationWidget::HandleTextChanged(const FText& Text)
{
	// 1 / 2 / 3 typed into an empty box picks a suggestion
	const FString String = Text.ToString();
	if (String.Len() == 1 && String[0] >= TEXT('1') && String[0] <= TEXT('3'))
	{
		const int32 Index = String[0] - TEXT('0');
		if (Index <= SuggestionCount && Director.IsValid())
		{
			InputBox->SetText(FText::GetEmpty());
			Director->PickSuggestion(Index);
		}
	}
	else if (String.Len() > 200)
	{
		// GDD: max 200 characters; the extra is cut (warning in a later UI pass)
		InputBox->SetText(FText::FromString(String.Left(200)));
	}
}

#undef LOCTEXT_NAMESPACE
