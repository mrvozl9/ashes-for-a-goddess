// AI-assisted: Claude Code, 11.11.2026, Automation tests for the response validator and prompt builder (Phase 4); spoiler unlock chapters + fallback pick, hesitate vs content, turn refund (12.11.2026). Reviewed by <pending>.
// Run: Session Frontend > Automation, filter "Ashes." (or console: Automation RunTests Ashes.)

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "ResponseValidator.h"
#include "PromptBuilder.h"
#include "OfflineLLMProvider.h"
#include "SceneDirectorSubsystem.h"
#include "AshesAISettings.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace AshesTestData
{
	FCharacterCardRow MakeZafir()
	{
		FCharacterCardRow Card;
		Card.DisplayName = TEXT("Zafir");
		Card.Pronouns = TEXT("he/him");
		Card.Voice = TEXT("Teasing, fire metaphors.");
		Card.Emotions = { TEXT("neutral"), TEXT("teasing"), TEXT("tender"), TEXT("jealous"), TEXT("cold"), TEXT("angry") };
		Card.FallbackEmotion = TEXT("neutral");
		FRelationshipValueDef Trust;     Trust.Name = TEXT("trust");         Trust.Start = 20;     Trust.MaxIncrease = 8;     Trust.MaxDecrease = 8;
		FRelationshipValueDef Affection; Affection.Name = TEXT("affection"); Affection.Start = 10; Affection.MaxIncrease = 8; Affection.MaxDecrease = 8;
		Card.Values = { Trust, Affection };
		FSpoilerKeyword Contract; Contract.Keyword = TEXT("contract"); Contract.UnlockChapter = 7;
		FSpoilerKeyword Phoenix;  Phoenix.Keyword = TEXT("you are the phoenix"); Phoenix.UnlockChapter = 0;
		Card.SpoilerKeywords = { Contract, Phoenix };
		FChapterSecret Early; Early.Chapter = 1; Early.Text = TEXT("SECRET_CH1");
		FChapterSecret Late;  Late.Chapter = 4;  Late.Text = TEXT("SECRET_CH4");
		Card.Secrets = { Early, Late };
		return Card;
	}

	FSceneCardRow MakeScene()
	{
		FSceneCardRow Scene;
		Scene.Chapter = 1;
		Scene.Place = TEXT("Garden of Ash");
		FBeatDef A; A.Id = TEXT("zafir_reveals_target"); A.Rule = TEXT("He says he is here for the king.");
		FBeatDef B; B.Id = TEXT("alliance_agreed");      B.Rule = TEXT("They agree to work together.");
		Scene.Beats = { A, B };
		return Scene;
	}

	FAshesValidationContext MakeContext()
	{
		return FAshesValidationContext::FromCard(MakeZafir(), MakeScene());
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesValidatorValidJson, "Ashes.Validator.ValidJson", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesValidatorValidJson::RunTest(const FString& Parameters)
{
	const FString Json = TEXT(R"({"reply":"Careful, Little Ember.","emotion":"teasing","gesture":"lean_in","trust_delta":3,"affection_delta":5,"beats":["zafir_reveals_target"],"suggestions":["Why?","Stay away.","Who sent you?"],"off_topic":false})");
	const FAshesValidationResult R = UResponseValidator::Validate(Json, AshesTestData::MakeContext());
	TestTrue(TEXT("outcome ok"), R.IsOk());
	TestEqual(TEXT("reply"), R.Response.Reply, FString(TEXT("Careful, Little Ember.")));
	TestTrue(TEXT("emotion"), R.Response.Emotion == FName(TEXT("teasing")));
	TestEqual(TEXT("trust"), R.Response.Deltas.FindRef(TEXT("trust")), 3);
	TestEqual(TEXT("affection"), R.Response.Deltas.FindRef(TEXT("affection")), 5);
	TestEqual(TEXT("beat kept"), R.Response.Beats.Num(), 1);
	TestEqual(TEXT("3 suggestions"), R.Response.Suggestions.Num(), 3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesValidatorBrokenJson, "Ashes.Validator.BrokenJson", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesValidatorBrokenJson::RunTest(const FString& Parameters)
{
	const FAshesValidationResult A = UResponseValidator::Validate(TEXT(R"({"reply": "Careful, Little Em)"), AshesTestData::MakeContext());
	TestTrue(TEXT("cut-off JSON -> retry"), A.Outcome == EValidationOutcome::RetryBrokenJson);
	const FAshesValidationResult B = UResponseValidator::Validate(TEXT("Sure! Here is my answer."), AshesTestData::MakeContext());
	TestTrue(TEXT("no JSON -> retry"), B.Outcome == EValidationOutcome::RetryBrokenJson);
	const FAshesValidationResult C = UResponseValidator::Validate(TEXT(R"({"emotion":"teasing"})"), AshesTestData::MakeContext());
	TestTrue(TEXT("missing reply -> retry"), C.Outcome == EValidationOutcome::RetryBrokenJson);
	// JSON inside a code fence with chatter around it is still accepted
	const FAshesValidationResult D = UResponseValidator::Validate(TEXT("Here:\n```json\n{\"reply\":\"Hm.\",\"emotion\":\"cold\"}\n```"), AshesTestData::MakeContext());
	TestTrue(TEXT("fenced JSON ok"), D.IsOk());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesValidatorUnknownEmotion, "Ashes.Validator.UnknownEmotion", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesValidatorUnknownEmotion::RunTest(const FString& Parameters)
{
	const FAshesValidationResult R = UResponseValidator::Validate(TEXT(R"({"reply":"Hm.","emotion":"Smug"})"), AshesTestData::MakeContext());
	TestTrue(TEXT("ok"), R.IsOk());
	TestTrue(TEXT("falls back to neutral"), R.Response.Emotion == FName(TEXT("neutral")));
	const FAshesValidationResult Upper = UResponseValidator::Validate(TEXT(R"({"reply":"Hm.","emotion":"Jealous"})"), AshesTestData::MakeContext());
	TestTrue(TEXT("case-insensitive"), Upper.Response.Emotion == FName(TEXT("jealous")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesValidatorClamp, "Ashes.Validator.ClampDeltas", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesValidatorClamp::RunTest(const FString& Parameters)
{
	const FAshesValidationResult R = UResponseValidator::Validate(TEXT(R"({"reply":"Fine.","emotion":"tender","trust_delta":50,"affection_delta":"-20"})"), AshesTestData::MakeContext());
	TestTrue(TEXT("ok"), R.IsOk());
	TestEqual(TEXT("+50 Trust clamped to +8"), R.Response.Deltas.FindRef(TEXT("trust")), 8);
	TestEqual(TEXT("-20 Affection (as string) clamped to -8"), R.Response.Deltas.FindRef(TEXT("affection")), -8);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesValidatorLongReply, "Ashes.Validator.LongReply", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesValidatorLongReply::RunTest(const FString& Parameters)
{
	FString Long;
	for (int32 i = 0; i < 120; ++i)
	{
		Long += TEXT("ember ");
	}
	const FAshesValidationResult R = UResponseValidator::Validate(FString::Printf(TEXT(R"({"reply":"%s","emotion":"teasing"})"), *Long), AshesTestData::MakeContext());
	TArray<FString> Words;
	R.Response.Reply.ParseIntoArrayWS(Words);
	TestTrue(TEXT("ok"), R.IsOk());
	TestEqual(TEXT("cut to 80 words"), Words.Num(), 80);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesValidatorBeats, "Ashes.Validator.FreeSentenceBeats", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesValidatorBeats::RunTest(const FString& Parameters)
{
	// Seen with Qwen3.5-4B on 11.11.2026: sentences instead of beat ids
	const FAshesValidationResult R = UResponseValidator::Validate(TEXT(R"({"reply":"Hm.","emotion":"jealous","beats":["The fire in my veins flares up, hot and angry.","ALLIANCE_AGREED"]})"), AshesTestData::MakeContext());
	TestTrue(TEXT("ok"), R.IsOk());
	TestEqual(TEXT("only the real id is kept"), R.Response.Beats.Num(), 1);
	TestTrue(TEXT("alliance_agreed kept"), R.Response.Beats.Contains(FName(TEXT("alliance_agreed"))));
	return true;
}

// AI-assisted: Claude Code, 13.11.2026, id slip test (Phase 6 fix 5). Reviewed by <pending>.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesValidatorIdSlips, "Ashes.Validator.IdSlips", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesValidatorIdSlips::RunTest(const FString& Parameters)
{
	const TArray<FName> Ch1 = { TEXT("sahra_confronts_blade"), TEXT("uneasy_truce") };
	TestEqual(TEXT("exact"), UResponseValidator::MatchId(TEXT("uneasy_truce"), Ch1), FName(TEXT("uneasy_truce")));
	TestEqual(TEXT("playtest slip: extra underscore"), UResponseValidator::MatchId(TEXT("sa_hra_confronts_blade"), Ch1), FName(TEXT("sahra_confronts_blade")));
	TestEqual(TEXT("case and spaces"), UResponseValidator::MatchId(TEXT(" Uneasy Truce "), Ch1), FName(TEXT("uneasy_truce")));
	TestEqual(TEXT("one wrong letter"), UResponseValidator::MatchId(TEXT("sahra_confronts_blase"), Ch1), FName(TEXT("sahra_confronts_blade")));
	TestEqual(TEXT("one missing letter"), UResponseValidator::MatchId(TEXT("uneasy_truc"), Ch1), FName(TEXT("uneasy_truce")));
	TestTrue(TEXT("two wrong letters: no"), UResponseValidator::MatchId(TEXT("uneazy_trace"), Ch1).IsNone());
	TestTrue(TEXT("a sentence: no"), UResponseValidator::MatchId(TEXT("They agree on an uneasy truce."), Ch1).IsNone());
	TestTrue(TEXT("empty: no"), UResponseValidator::MatchId(TEXT("  "), Ch1).IsNone());

	// One letter apart from two ids: no guess
	const TArray<FName> Close = { TEXT("clue_garden_a"), TEXT("clue_garden_b") };
	TestTrue(TEXT("ambiguous: no"), UResponseValidator::MatchId(TEXT("clue_garden_c"), Close).IsNone());
	// A different digit is a different id, not a slip (review 13.11)
	const TArray<FName> Stage = { TEXT("stage_1_reached") };
	TestTrue(TEXT("digit changed: no"), UResponseValidator::MatchId(TEXT("stage_2_reached"), Stage).IsNone());
	TestTrue(TEXT("digit added at the end: no"), UResponseValidator::MatchId(TEXT("stage_1_reached1"), Stage).IsNone());
	TestEqual(TEXT("letter slip next to a digit: yes"), UResponseValidator::MatchId(TEXT("stage_1_reachet"), Stage), FName(TEXT("stage_1_reached")));
	// Short ids need the exact letters
	const TArray<FName> Short = { TEXT("kiss") };
	TestTrue(TEXT("short id, wrong letter: no"), UResponseValidator::MatchId(TEXT("kis5"), Short).IsNone());

	// Through Validate: the slip is kept as the real id and noted
	const FAshesValidationResult R = UResponseValidator::Validate(TEXT(R"({"reply":"Hm.","emotion":"teasing","beats":["ALLIANCE-AGREED"]})"), AshesTestData::MakeContext());
	TestTrue(TEXT("kept through Validate"), R.Response.Beats.Contains(FName(TEXT("alliance_agreed"))));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesValidatorSpoiler, "Ashes.Validator.SpoilerAndSafety", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesValidatorSpoiler::RunTest(const FString& Parameters)
{
	const FAshesValidationResult R = UResponseValidator::Validate(TEXT(R"({"reply":"Vashir paid me; the contract is in my boot.","emotion":"cold"})"), AshesTestData::MakeContext());
	TestTrue(TEXT("spoiler -> regenerate"), R.Outcome == EValidationOutcome::Regenerate);

	// Unlock chapters: "contract" is allowed from chapter 7; UnlockChapter 0 is never allowed
	FAshesValidationContext Late = AshesTestData::MakeContext();
	Late.Chapter = 7;
	TestTrue(TEXT("contract allowed in chapter 7"), UResponseValidator::Validate(TEXT(R"({"reply":"Vashir paid me; the contract is in my boot.","emotion":"cold"})"), Late).IsOk());
	Late.Chapter = 99;
	TestTrue(TEXT("unlock 0 blocked even in a late chapter"), UResponseValidator::Validate(TEXT(R"({"reply":"Little Ember, you are the phoenix.","emotion":"tender"})"), Late).Outcome == EValidationOutcome::Regenerate);
	TestTrue(TEXT("unsafe player line"), UResponseValidator::IsUnsafeText(TEXT("you are a NAZI"), UResponseValidator::DefaultSafetyWords()));
	TestFalse(TEXT("normal player line"), UResponseValidator::IsUnsafeText(TEXT("Who sent you?"), UResponseValidator::DefaultSafetyWords()));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesPromptBuilder, "Ashes.PromptBuilder.OrderSecretsHistory", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesPromptBuilder::RunTest(const FString& Parameters)
{
	TArray<FConversationTurn> History;
	for (int32 i = 0; i < 12; ++i)
	{
		FConversationTurn Turn;
		Turn.bFromPlayer = (i % 2 == 0);
		Turn.Text = FString::Printf(TEXT("turn %d"), i);
		History.Add(Turn);
	}
	FPromptState State;
	State.Values.Add(TEXT("trust"), 55);

	const FLLMRequest Request = UPromptBuilder::Build(AshesTestData::MakeZafir(), TEXT("LORE_TEXT"), AshesTestData::MakeScene(), State, History, TEXT("Who sent you?"), 1);
	const FString& System = Request.Messages[0].Content;

	TestEqual(TEXT("system + 8 turns + new line"), Request.Messages.Num(), 10);
	TestEqual(TEXT("oldest kept turn is turn 4"), Request.Messages[1].Content, FString(TEXT("turn 4")));
	TestTrue(TEXT("last message ends with the player's line"), Request.Messages.Last().Content.EndsWith(TEXT("Who sent you?")));
	TestTrue(TEXT("chapter 1 secret included"), System.Contains(TEXT("SECRET_CH1")));
	TestFalse(TEXT("chapter 4 secret NOT included in chapter 1"), System.Contains(TEXT("SECRET_CH4")));
	TestTrue(TEXT("order: card < lore < scene < state"),
		System.Find(TEXT("## Character")) < System.Find(TEXT("LORE_TEXT")) &&
		System.Find(TEXT("LORE_TEXT")) < System.Find(TEXT("## Scene")) &&
		System.Find(TEXT("## Scene")) < System.Find(TEXT("## State")));
	TestTrue(TEXT("trust 55 as a word, in the newest message"), Request.Messages.Last().Content.Contains(TEXT("trust toward Sahra: warming")));
	TestFalse(TEXT("values are not in the system message (cache)"), System.Contains(TEXT("toward Sahra: warming")));
	TestTrue(TEXT("beat ids listed"), System.Contains(TEXT("zafir_reveals_target")));
	TestTrue(TEXT("delta keys from the card"), System.Contains(TEXT("\"trust_delta\"")) && System.Contains(TEXT("\"affection_delta\"")));
	TestTrue(TEXT("pronouns for gestures"), System.Contains(TEXT("You are Zafir (he/him)")) && System.Contains(TEXT("tilts his head")));
	return true;
}

// AI-assisted: Claude Code, 13.11.2026, prompt cache test (Phase 6 fix 1). Reviewed by <pending>.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesPromptCache, "Ashes.PromptBuilder.CacheFriendly", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesPromptCache::RunTest(const FString& Parameters)
{
	auto MakeHistory = [](int32 Num)
	{
		TArray<FConversationTurn> History;
		for (int32 i = 0; i < Num; ++i)
		{
			FConversationTurn Turn;
			Turn.bFromPlayer = (i % 2 == 1); // turn 0 is the opener
			Turn.Text = FString::Printf(TEXT("turn %d"), i);
			History.Add(Turn);
		}
		return History;
	};

	// The system message does not change when the values, memories or clue hints change
	FPromptState Cold;
	Cold.Values.Add(TEXT("trust"), 20);
	FPromptState Warm;
	Warm.Values.Add(TEXT("trust"), 60);
	Warm.Memories.Add(TEXT("She kept my secret."));
	Warm.ClueHints.Add(TEXT("HINT_TEXT"));
	const FLLMRequest A = UPromptBuilder::Build(AshesTestData::MakeZafir(), TEXT("LORE"), AshesTestData::MakeScene(), Cold, MakeHistory(3), TEXT("Hello"), 1);
	const FLLMRequest B = UPromptBuilder::Build(AshesTestData::MakeZafir(), TEXT("LORE"), AshesTestData::MakeScene(), Warm, MakeHistory(3), TEXT("Hello"), 1);
	TestEqual(TEXT("same system message for other values"), A.Messages[0].Content, B.Messages[0].Content);
	TestTrue(TEXT("memory and hint in the newest message"), B.Messages.Last().Content.Contains(TEXT("She kept my secret.")) && B.Messages.Last().Content.Contains(TEXT("HINT_TEXT")));
	TestTrue(TEXT("state note before the line"), B.Messages.Last().Content.StartsWith(TEXT("[")) && B.Messages.Last().Content.EndsWith(TEXT("Hello")));
	TestEqual(TEXT("earlier lines stay plain"), B.Messages[2].Content, FString(TEXT("turn 1")));

	// The history window moves in blocks, not every turn
	TestEqual(TEXT("8 turns: from 0"), UPromptBuilder::GetHistoryStart(8), 0);
	for (int32 Num = 9; Num <= 12; ++Num)
	{
		TestEqual(FString::Printf(TEXT("%d turns: same start"), Num), UPromptBuilder::GetHistoryStart(Num), 4);
	}
	TestEqual(TEXT("13 turns: next block"), UPromptBuilder::GetHistoryStart(13), 8);
	TestEqual(TEXT("23 turns (11-turn scene): at most 8 sent"), 23 - UPromptBuilder::GetHistoryStart(23), 7);

	// Same start of the prompt for turns that share a block: only new messages are added at the end
	const FLLMRequest Turn9 = UPromptBuilder::Build(AshesTestData::MakeZafir(), TEXT("LORE"), AshesTestData::MakeScene(), Cold, MakeHistory(9), TEXT("x"), 1);
	const FLLMRequest Turn11 = UPromptBuilder::Build(AshesTestData::MakeZafir(), TEXT("LORE"), AshesTestData::MakeScene(), Warm, MakeHistory(11), TEXT("y"), 1);
	bool bPrefixSame = Turn9.Messages.Num() < Turn11.Messages.Num();
	for (int32 i = 0; bPrefixSame && i < Turn9.Messages.Num() - 1; ++i)
	{
		bPrefixSame = Turn9.Messages[i].Content == Turn11.Messages[i].Content;
	}
	TestTrue(TEXT("turn 11 starts with turn 9's prompt"), bPrefixSame);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesFallbackPick, "Ashes.Fallback.PickLine", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesFallbackPick::RunTest(const FString& Parameters)
{
	auto Row = [](const TCHAR* Scene, const TCHAR* Beat, const TCHAR* Emotion, const TCHAR* Line)
	{
		FFallbackLineRow R; R.Scene = Scene; R.Beat = Beat; R.Emotion = Emotion; R.Line = Line; return R;
	};
	const TArray<FFallbackLineRow> Rows = {
		Row(TEXT("S1"), TEXT("any"), TEXT("teasing"), TEXT("any-teasing")),
		Row(TEXT("S1"), TEXT("any"), TEXT("cold"), TEXT("any-cold")),
		Row(TEXT("S1"), TEXT("beat_a"), TEXT("teasing"), TEXT("beat-a")),
		Row(TEXT("S2"), TEXT("any"), TEXT("cold"), TEXT("other-scene")),
	};
	FFallbackLineRow Out;
	TestTrue(TEXT("pushed beat wins"), UOfflineLLMProvider::PickLine(Rows, TEXT("S1"), TEXT("beat_a"), TEXT("cold"), 0, Out) && Out.Line == TEXT("beat-a"));
	TestTrue(TEXT("no beat: any + same emotion"), UOfflineLLMProvider::PickLine(Rows, TEXT("S1"), NAME_None, TEXT("cold"), 0, Out) && Out.Line == TEXT("any-cold"));
	TestTrue(TEXT("unknown beat falls back to any"), UOfflineLLMProvider::PickLine(Rows, TEXT("S1"), TEXT("beat_x"), TEXT("teasing"), 0, Out) && Out.Line == TEXT("any-teasing"));
	TestTrue(TEXT("other scenes never used"), UOfflineLLMProvider::PickLine(Rows, TEXT("S2"), NAME_None, TEXT("teasing"), 0, Out) && Out.Line == TEXT("other-scene"));
	TestFalse(TEXT("unknown scene -> nothing"), UOfflineLLMProvider::PickLine(Rows, TEXT("S9"), NAME_None, TEXT("teasing"), 0, Out));
	TestTrue(TEXT("no repeat: beat line just used -> next tier"), UOfflineLLMProvider::PickLine(Rows, TEXT("S1"), TEXT("beat_a"), TEXT("teasing"), 0, Out, TEXT("beat-a")) && Out.Line == TEXT("any-teasing"));
	TestTrue(TEXT("only line left is repeated rather than nothing"), UOfflineLLMProvider::PickLine(Rows, TEXT("S2"), NAME_None, TEXT("cold"), 0, Out, TEXT("other-scene")) && Out.Line == TEXT("other-scene"));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesValidatorEmphasis, "Ashes.Validator.StripEmphasis", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesValidatorEmphasis::RunTest(const FString& Parameters)
{
	const FAshesValidationResult R = UResponseValidator::Validate(TEXT(R"({"reply":"Why do *you* dance while the others _sleep_?","emotion":"teasing","gesture":"*tilts his head*"})"), AshesTestData::MakeContext());
	TestTrue(TEXT("ok"), R.IsOk());
	TestEqual(TEXT("asterisks and underscores removed"), R.Response.Reply, FString(TEXT("Why do you dance while the others sleep?")));
	TestEqual(TEXT("gesture cleaned"), R.Response.Gesture, FString(TEXT("tilts his head")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesFallbackTypes, "Ashes.Fallback.HesitateVsContent", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesFallbackTypes::RunTest(const FString& Parameters)
{
	auto Row = [](EFallbackLineType Type, const TCHAR* Beat, const TCHAR* Line)
	{
		FFallbackLineRow R; R.Scene = TEXT("S1"); R.Type = Type; R.Beat = Beat; R.Emotion = TEXT("neutral"); R.Line = Line; return R;
	};
	const TArray<FFallbackLineRow> Rows = {
		Row(EFallbackLineType::Content, TEXT("beat_a"), TEXT("content-beat")),
		Row(EFallbackLineType::Content, TEXT("any"), TEXT("content-any")),
		Row(EFallbackLineType::Hesitate, TEXT("any"), TEXT("hesitate-1")),
		Row(EFallbackLineType::Hesitate, TEXT("any"), TEXT("hesitate-2")),
	};
	FFallbackLineRow Out;
	// Online timeout: only Hesitate lines, even when a beat is being pushed
	for (int32 i = 0; i < 4; ++i)
	{
		TestTrue(TEXT("timeout picks only hesitate lines"), UOfflineLLMProvider::PickLine(Rows, TEXT("S1"), TEXT("beat_a"), TEXT("neutral"), i, Out, FString(), EFallbackLineType::Hesitate) && Out.Line.StartsWith(TEXT("hesitate")));
	}
	// Offline: only Content lines
	for (int32 i = 0; i < 4; ++i)
	{
		TestTrue(TEXT("offline picks only content lines"), UOfflineLLMProvider::PickLine(Rows, TEXT("S1"), NAME_None, TEXT("neutral"), i, Out) && Out.Line.StartsWith(TEXT("content")));
	}
	// No hesitate rows for a scene -> nothing (the provider then uses its generic dodge lines, never a content line)
	const TArray<FFallbackLineRow> ContentOnly = { Row(EFallbackLineType::Content, TEXT("any"), TEXT("content-any")) };
	TestFalse(TEXT("no hesitate rows -> no content line"), UOfflineLLMProvider::PickLine(ContentOnly, TEXT("S1"), NAME_None, TEXT("neutral"), 0, Out, FString(), EFallbackLineType::Hesitate));
	// Per character: Vashir's evasive line is never Zafir's; scene "any" fits every scene
	FFallbackLineRow VashirLine = Row(EFallbackLineType::Hesitate, TEXT("any"), TEXT("vashir-dodge")); VashirLine.Character = TEXT("Vashir"); VashirLine.Scene = TEXT("any");
	FFallbackLineRow ZafirLine = Row(EFallbackLineType::Hesitate, TEXT("any"), TEXT("zafir-dodge")); ZafirLine.Character = TEXT("Zafir"); ZafirLine.Scene = TEXT("any");
	const TArray<FFallbackLineRow> PerCharacter = { VashirLine, ZafirLine };
	for (int32 i = 0; i < 4; ++i)
	{
		TestTrue(TEXT("Zafir gets only his own evasive lines"), UOfflineLLMProvider::PickLine(PerCharacter, TEXT("Ch1_ZafirKeyScene"), NAME_None, TEXT("neutral"), i, Out, FString(), EFallbackLineType::Hesitate, TEXT("Zafir")) && Out.Line == TEXT("zafir-dodge"));
	}

	// [Gestures] are split from speech
	FString Speech, Gesture;
	UOfflineLLMProvider::SplitGesture(TEXT("[He tilts his head, smiling.] Not tonight."), Speech, Gesture);
	TestEqual(TEXT("speech"), Speech, FString(TEXT("Not tonight.")));
	TestEqual(TEXT("gesture"), Gesture, FString(TEXT("He tilts his head, smiling.")));
	UOfflineLLMProvider::SplitGesture(TEXT("[He says nothing at all.]"), Speech, Gesture);
	TestTrue(TEXT("gesture-only line has no speech"), Speech.IsEmpty() && Gesture == TEXT("He says nothing at all."));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesTurnRefund, "Ashes.SceneDirector.UnansweredTurnNotCounted", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesTurnRefund::RunTest(const FString& Parameters)
{
	TArray<FConversationTurn> History;
	FConversationTurn Opener; Opener.bFromPlayer = false; Opener.Text = TEXT("Careful, Little Ember.");
	FConversationTurn Question; Question.bFromPlayer = true; Question.Text = TEXT("Why do you carry a blade?");
	History = { Opener, Question };
	int32 Turns = 1; // counted when the line was sent
	USceneDirectorSubsystem::RefundUnansweredTurn(Turns, History);
	TestEqual(TEXT("turn not counted"), Turns, 0);
	TestEqual(TEXT("unanswered line removed from the history"), History.Num(), 1);
	TestFalse(TEXT("last turn is no longer the player"), History.Last().bFromPlayer);
	USceneDirectorSubsystem::RefundUnansweredTurn(Turns, History);
	TestEqual(TEXT("never below 0, Zafir's lines stay"), Turns + History.Num(), 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesAILimits, "Ashes.Settings.EditorLimits", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesAILimits::RunTest(const FString& Parameters)
{
	// In the editor (and PIE) the hard limit is the editor value; the soft limit stays the game's 6 s
	const UAshesAISettings* Settings = GetDefault<UAshesAISettings>();
	TestTrue(TEXT("running in the editor"), GIsEditor);
	TestEqual(TEXT("hard limit = EditorHardTimeoutSeconds"), Settings->GetEffectiveRequestTimeout(), Settings->EditorHardTimeoutSeconds);
	TestTrue(TEXT("editor hard limit at least 60 s"), Settings->EditorHardTimeoutSeconds >= 60.f);
	TestEqual(TEXT("soft limit = RequestTimeoutSeconds"), Settings->GetHesitateAfterSeconds(), Settings->RequestTimeoutSeconds);
	TestTrue(TEXT("soft before hard"), Settings->GetHesitateAfterSeconds() < Settings->GetEffectiveRequestTimeout());
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
