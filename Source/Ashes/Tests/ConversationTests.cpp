// AI-assisted: Claude Code, 11.11.2026, Automation tests for the response validator and prompt builder (Phase 4). Reviewed by <pending>.
// Run: Session Frontend > Automation, filter "Ashes." (or console: Automation RunTests Ashes.)

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "ResponseValidator.h"
#include "PromptBuilder.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace AshesTestData
{
	FCharacterCardRow MakeZafir()
	{
		FCharacterCardRow Card;
		Card.DisplayName = TEXT("Zafir");
		Card.Voice = TEXT("Teasing, fire metaphors.");
		Card.Emotions = { TEXT("neutral"), TEXT("teasing"), TEXT("tender"), TEXT("jealous"), TEXT("cold"), TEXT("angry") };
		Card.FallbackEmotion = TEXT("neutral");
		FRelationshipValueDef Trust;     Trust.Name = TEXT("trust");         Trust.Start = 20;     Trust.MaxIncrease = 8;     Trust.MaxDecrease = 8;
		FRelationshipValueDef Affection; Affection.Name = TEXT("affection"); Affection.Start = 10; Affection.MaxIncrease = 8; Affection.MaxDecrease = 8;
		Card.Values = { Trust, Affection };
		Card.SpoilerKeywords = { TEXT("contract") };
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesValidatorSpoiler, "Ashes.Validator.SpoilerAndSafety", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesValidatorSpoiler::RunTest(const FString& Parameters)
{
	const FAshesValidationResult R = UResponseValidator::Validate(TEXT(R"({"reply":"Vashir paid me; the contract is in my boot.","emotion":"cold"})"), AshesTestData::MakeContext());
	TestTrue(TEXT("spoiler -> regenerate"), R.Outcome == EValidationOutcome::Regenerate);
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
	TestEqual(TEXT("last message is the player's line"), Request.Messages.Last().Content, FString(TEXT("Who sent you?")));
	TestTrue(TEXT("chapter 1 secret included"), System.Contains(TEXT("SECRET_CH1")));
	TestFalse(TEXT("chapter 4 secret NOT included in chapter 1"), System.Contains(TEXT("SECRET_CH4")));
	TestTrue(TEXT("order: card < lore < scene < state"),
		System.Find(TEXT("## Character")) < System.Find(TEXT("LORE_TEXT")) &&
		System.Find(TEXT("LORE_TEXT")) < System.Find(TEXT("## Scene")) &&
		System.Find(TEXT("## Scene")) < System.Find(TEXT("## State")));
	TestTrue(TEXT("trust 55 as a word"), System.Contains(TEXT("trust toward Sahra: warming")));
	TestTrue(TEXT("beat ids listed"), System.Contains(TEXT("zafir_reveals_target")));
	TestTrue(TEXT("delta keys from the card"), System.Contains(TEXT("\"trust_delta\"")) && System.Contains(TEXT("\"affection_delta\"")));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
