// AI-assisted: Claude Code, 11.11.2026, conversation screen logic; WBP_Conversation provides the layout (Phase 5); hesitation status (12.11.2026); pick-only with 1 / 2 / 3 keys while the AI is offline; reply shown word by word (13.11.2026). Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "AshesTypes.h"
#include "ConversationWidget.generated.h"

class USceneDirectorSubsystem;
class UTextBlock;
class UButton;
class UEditableTextBox;
class UImage;
class UScrollBox;

/**
 *  Base class of WBP_Conversation (design.md S2, placeholder style until Tuana's final UI).
 *  The Widget Blueprint must contain widgets with these exact names (BindWidget).
 *  Keys: type + Enter to send; 1 / 2 / 3 in the empty input box pick a suggested reply.
 */
UCLASS(Abstract)
class ASHES_API UConversationWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void InitConversation(USceneDirectorSubsystem* InDirector);

	/** Keyboard focus goes here when the screen opens */
	TSharedPtr<SWidget> GetFocusTarget() const;

protected:

	virtual void NativeConstruct() override;

	/** Offline the text box is hidden: 1 / 2 / 3 on the keyboard pick a reply here instead */
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> NameText;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> DialogueText;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> ThinkingText;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> TurnsText;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UButton> Reply1;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UButton> Reply2;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UButton> Reply3;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Reply1Text;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Reply2Text;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UTextBlock> Reply3Text;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UEditableTextBox> InputBox;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UButton> SendButton;
	UPROPERTY(meta=(BindWidget)) TObjectPtr<UButton> LeaveButton;

	/** Scrolls the dialogue so the newest line stays visible */
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UScrollBox> DialogueScroll;

	/** Large emotion portrait (texture T_Portrait_<Name>_<Emotion> if it exists, else a placeholder colour) */
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UImage> PortraitImage;
	UPROPERTY(meta=(BindWidgetOptional)) TObjectPtr<UTextBlock> EmotionText;

	/** Lines kept on screen */
	UPROPERTY(EditAnywhere, Category="Conversation")
	int32 VisibleLines = 40;

private:

	UFUNCTION() void HandleLinePosted(const FString& Speaker, const FString& Text);
	UFUNCTION() void HandleReplyReady(const FLLMResponse& Response, bool bFromFallback);
	UFUNCTION() void HandleThinking(bool bThinking);
	UFUNCTION() void HandleHesitating();
	UFUNCTION() void HandleReplyPartial(const FString& TextSoFar);
	UFUNCTION() void HandleTurns(int32 TurnsLeft);
	UFUNCTION() void HandleReply1();
	UFUNCTION() void HandleReply2();
	UFUNCTION() void HandleReply3();
	UFUNCTION() void HandleSend();
	UFUNCTION() void HandleLeave();
	UFUNCTION() void HandleTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	UFUNCTION() void HandleTextChanged(const FText& Text);

	void SetSuggestions(const TArray<FString>& Suggestions);
	void SetEmotion(FName Emotion);
	void UpdateInteractivity();
	void RefreshDialogue();

	TWeakObjectPtr<USceneDirectorSubsystem> Director;
	TArray<FString> Lines;
	int32 SuggestionCount = 0;
	bool bThinking = false;

	/** Index in Lines of the reply that is still being written (word by word), or INDEX_NONE */
	int32 LiveLineIndex = INDEX_NONE;

	/** Removes the unfinished reply line, if any */
	void RemoveLiveLine();
};
