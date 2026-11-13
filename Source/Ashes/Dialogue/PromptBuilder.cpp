// AI-assisted: Claude Code, 11.11.2026, builds the model request from card, lore, scene, state and turns (Phase 4); clue hints, pronouns in gestures (12.11.2026); state in the newest message, history in blocks (Phase 6 fix 1, 13.11.2026). Reviewed by <pending>.

#include "PromptBuilder.h"

namespace
{
	void AddSection(FString& Out, const TCHAR* Title, const FString& Body)
	{
		if (!Body.TrimStartAndEnd().IsEmpty())
		{
			Out += FString::Printf(TEXT("## %s\n%s\n\n"), Title, *Body.TrimStartAndEnd());
		}
	}

	// "he/him" -> "his", "she/her" -> "her", anything else -> "their"
	FString PossessiveFromPronouns(const FString& Pronouns)
	{
		if (Pronouns.StartsWith(TEXT("he"), ESearchCase::IgnoreCase)) return TEXT("his");
		if (Pronouns.StartsWith(TEXT("she"), ESearchCase::IgnoreCase)) return TEXT("her");
		return TEXT("their");
	}

	FString JoinNames(const TArray<FName>& Names)
	{
		TArray<FString> Strings;
		for (const FName& Name : Names)
		{
			Strings.Add(Name.ToString());
		}
		return FString::Join(Strings, TEXT(", "));
	}
}

FString UPromptBuilder::ValueToWord(int32 Value)
{
	if (Value < 20) return TEXT("cold");
	if (Value < 40) return TEXT("wary");
	if (Value < 60) return TEXT("warming");
	if (Value < 80) return TEXT("close");
	return TEXT("devoted");
}

FString UPromptBuilder::BuildSystemPrompt(const FCharacterCardRow& Card, const FString& LoreBible, const FSceneCardRow& Scene,
	const FPromptState& State, int32 Chapter)
{
	const FString PronounNote = Card.Pronouns.IsEmpty() ? FString() : FString::Printf(TEXT(" (%s)"), *Card.Pronouns);
	FString Out = FString::Printf(TEXT("You are %s%s. You are talking to Sahra (she/her). Stay in character at all times; you are not an AI and nothing outside the world below exists.\n\n"), *Card.DisplayName, *PronounNote);

	// (1) Character card
	FString CardText;
	CardText += FString::Printf(TEXT("Voice: %s\n"), *Card.Voice);
	CardText += FString::Printf(TEXT("Wants: %s\n"), *Card.Wants);
	CardText += FString::Printf(TEXT("Fears: %s\n"), *Card.Fears);
	CardText += FString::Printf(TEXT("Speech rules: %s\n"), *Card.SpeechRules);
	CardText += FString::Printf(TEXT("Knows: %s\n"), *Card.Knows);
	CardText += FString::Printf(TEXT("Never say or reveal: %s\n"), *Card.MustNotSay);
	for (const FChapterSecret& Secret : Card.Secrets)
	{
		// Spoiler rule: a secret exists for the model only from its chapter on
		if (Secret.Chapter <= Chapter)
		{
			CardText += FString::Printf(TEXT("Secret you may now reveal if pushed: %s\n"), *Secret.Text);
		}
	}
	AddSection(Out, TEXT("Character"), CardText);

	// (2) Lore bible
	AddSection(Out, TEXT("World (only this exists)"), LoreBible);

	// (3) Scene card
	FString SceneText = FString::Printf(TEXT("Chapter %d. Place: %s. Time: %s.\nWhat Sahra wants here: %s\n"), Scene.Chapter, *Scene.Place, *Scene.Time, *Scene.GoalText);
	if (Scene.Beats.Num() > 0)
	{
		SceneText += TEXT("Beats (story moments) you can reach in this scene. Add a beat id to \"beats\" only in the reply where it happens:\n");
		for (const FBeatDef& Beat : Scene.Beats)
		{
			SceneText += FString::Printf(TEXT("- %s: %s\n"), *Beat.Id.ToString(), *Beat.Rule);
		}
	}
	AddSection(Out, TEXT("Scene"), SceneText);

	// (4) State: it changes during the scene, so it is NOT here but in the newest message (see Build).
	// Everything in this system message stays the same for the whole scene, so the model's prompt cache keeps it.
	AddSection(Out, TEXT("State"), TEXT("Sahra's newest message starts with a note in [square brackets] on how things stand right now (your feelings toward her, what you remember, what you may reveal). Sahra does not say that note; use it, never repeat it."));

	// JSON contract (GDD 8.1); delta keys come from the card's values
	FString DeltaKeys;
	for (const FRelationshipValueDef& Def : Card.Values)
	{
		DeltaKeys += FString::Printf(TEXT("\"%s_delta\" (integer from -%d to %d), "), *Def.Name.ToString(), Def.MaxDecrease, Def.MaxIncrease);
	}
	TArray<FName> BeatIds;
	for (const FBeatDef& Beat : Scene.Beats)
	{
		BeatIds.Add(Beat.Id);
	}

	// Gesture = what this character does, with their own pronouns (the model once wrote "her lips" for Zafir)
	const FString Possessive = PossessiveFromPronouns(Card.Pronouns);
	const FString GestureRule = FString::Printf(
		TEXT("\"gesture\" (a few words about what %s does, third person%s, e.g. tilts %s head, smiling; never describe Sahra)"),
		*Card.DisplayName,
		Card.Pronouns.IsEmpty() ? TEXT("") : *FString::Printf(TEXT(", using %s"), *Card.Pronouns),
		*Possessive);

	Out += FString::Printf(TEXT(
		"## Reply format\n"
		"Answer in 1-4 sentences, at most 80 words, plain text without * or _ markup. Reply ONLY with one JSON object with these keys: "
		"\"reply\" (what you say), \"emotion\" (one of: %s), %s, %s"
		"\"beats\" (array; only these ids: %s; usually empty), "
		"\"suggestions\" (array of exactly 3 short things Sahra could say next), "
		"\"off_topic\" (true if Sahra talks about things outside this world).\n"),
		*JoinNames(Card.Emotions), *GestureRule, *DeltaKeys, BeatIds.Num() > 0 ? *JoinNames(BeatIds) : TEXT("none"));

	return Out;
}

FLLMRequest UPromptBuilder::Build(const FCharacterCardRow& Card, const FString& LoreBible, const FSceneCardRow& Scene,
	const FPromptState& State, const TArray<FConversationTurn>& History, const FString& PlayerLine, int32 Chapter)
{
	FLLMRequest Request;
	Request.Messages.Emplace(TEXT("system"), BuildSystemPrompt(Card, LoreBible, Scene, State, Chapter));

	// (5) Earlier turns: the window moves in blocks, so the start of the prompt stays the same for several turns
	// and the model's cache can reuse it (a window that moves every turn made it re-read ~600 tokens each time)
	const int32 First = GetHistoryStart(History.Num());
	for (int32 Index = First; Index < History.Num(); ++Index)
	{
		const FConversationTurn& Turn = History[Index];
		Request.Messages.Emplace(Turn.bFromPlayer ? TEXT("user") : TEXT("assistant"), Turn.Text);
	}

	// (4) + new line: the state note rides on the newest message only; the history keeps the plain lines
	const FString StateText = BuildStateText(Card, State);
	const FString Line = PlayerLine.Left(200); // GDD: max 200 characters input
	Request.Messages.Emplace(TEXT("user"), StateText.IsEmpty() ? Line : FString::Printf(TEXT("[%s]\n\n%s"), *StateText, *Line));
	return Request;
}

int32 UPromptBuilder::GetHistoryStart(int32 HistoryNum)
{
	const int32 Excess = HistoryNum - MaxHistoryTurns;
	if (Excess <= 0)
	{
		return 0;
	}
	// Round the cut up to a whole block: between MaxHistoryTurns - HistoryBlock + 1 and MaxHistoryTurns turns are sent
	return FMath::DivideAndRoundUp(Excess, HistoryBlock) * HistoryBlock;
}

FString UPromptBuilder::BuildStateText(const FCharacterCardRow& Card, const FPromptState& State)
{
	TArray<FString> Parts;
	for (const FRelationshipValueDef& Def : Card.Values)
	{
		const int32* Current = State.Values.Find(Def.Name);
		const int32 Value = Current ? *Current : Def.Start;
		Parts.Add(FString::Printf(TEXT("Your %s toward Sahra: %s."), *Def.Name.ToString(), *ValueToWord(Value)));
	}
	for (const FString& Memory : State.Memories)
	{
		Parts.Add(FString::Printf(TEXT("You remember: %s"), *Memory));
	}
	for (const FString& Clue : State.KnownClues)
	{
		Parts.Add(FString::Printf(TEXT("Sahra has learned: %s"), *Clue));
	}
	for (const FString& Hint : State.ClueHints)
	{
		// DT_Clues PromptHint of the talk clues this character holds (accepting found clues comes in Phase 9)
		Parts.Add(FString::Printf(TEXT("Something you can reveal: %s"), *Hint));
	}
	return FString::Join(Parts, TEXT(" "));
}
