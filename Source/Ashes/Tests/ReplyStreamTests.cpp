// AI-assisted: Claude Code, 13.11.2026, tests for the word-by-word reply helpers (Phase 7). Reviewed by <pending>.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "ReplyStream.h"
#include "ResponseValidator.h"
#include "OpenAICompatibleLLMProvider.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesReplyStreamExtract, "Ashes.ReplyStream.ExtractReply", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesReplyStreamExtract::RunTest(const FString& Parameters)
{
	using namespace AshesReplyStream;
	TestEqual(TEXT("nothing yet"), ExtractReply(TEXT("")), FString());
	TestEqual(TEXT("key not complete"), ExtractReply(TEXT("{\"rep")), FString());
	TestEqual(TEXT("no colon yet"), ExtractReply(TEXT("{\"reply\"")), FString());
	TestEqual(TEXT("no opening quote yet"), ExtractReply(TEXT("{\"reply\": ")), FString());
	TestEqual(TEXT("first words"), ExtractReply(TEXT("{\"reply\": \"Following me")), FString(TEXT("Following me")));
	TestEqual(TEXT("complete reply stops at the quote"), ExtractReply(TEXT("{\"reply\":\"Not now.\",\"emotion\":\"cold\"")), FString(TEXT("Not now.")));
	TestEqual(TEXT("escaped quote kept"), ExtractReply(TEXT("{\"reply\": \"She said \\\"no\\\" twice")), FString(TEXT("She said \"no\" twice")));
	TestEqual(TEXT("half escape at the end left out"), ExtractReply(TEXT("{\"reply\": \"Wait\\")), FString(TEXT("Wait")));
	TestEqual(TEXT("unicode escape"), ExtractReply(TEXT("{\"reply\": \"caf\\u00e9!\"}")), FString(TEXT("café!")));
	TestEqual(TEXT("half unicode escape left out"), ExtractReply(TEXT("{\"reply\": \"caf\\u00e")), FString(TEXT("caf")));
	TestEqual(TEXT("markup removed like the validator"), ExtractReply(TEXT("{\"reply\": \"You *never* ask")), FString(TEXT("You never ask")));
	TestEqual(TEXT("inside a code fence"), ExtractReply(TEXT("```json\n{\"reply\": \"Hm.\"")), FString(TEXT("Hm.")));
	TestEqual(TEXT("the word reply inside text is not the key"), ExtractReply(TEXT("{\"note\": \"no \\\"reply\\\" here\", \"reply\": \"Yes")), FString(TEXT("Yes")));
	TestEqual(TEXT("reply that is not a string"), ExtractReply(TEXT("{\"reply\": null")), FString());
	TestEqual(TEXT("a quoted \"reply\" without a colon is skipped"), ExtractReply(TEXT("{\"reply\" \"x\", \"reply\": \"Yes")), FString(TEXT("Yes")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesReplyStreamSafeToShow, "Ashes.ReplyStream.SafeToShow", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesReplyStreamSafeToShow::RunTest(const FString& Parameters)
{
	FAshesValidationContext Context;
	Context.Chapter = 1;
	FSpoilerKeyword Never; Never.Keyword = TEXT("contract"); Never.UnlockChapter = 0;
	FSpoilerKeyword Later; Later.Keyword = TEXT("Vashir hired"); Later.UnlockChapter = 4;
	FSpoilerKeyword Open;  Open.Keyword = TEXT("silk road"); Open.UnlockChapter = 1;
	Context.SpoilerKeywords = { Never, Later, Open };
	Context.SafetyWords = { TEXT("kill yourself") };

	using AshesReplyStream::SafeToShow;
	TestEqual(TEXT("no whole word yet"), SafeToShow(TEXT("Follow"), Context), FString());
	TestEqual(TEXT("whole words only"), SafeToShow(TEXT("Following me after the dan"), Context), FString(TEXT("Following me after the")));
	TestEqual(TEXT("half of a locked word is never shown"), SafeToShow(TEXT("There is a contr"), Context), FString(TEXT("There is a")));
	TestEqual(TEXT("first word of a locked phrase waits"), SafeToShow(TEXT("Yes, Vashir hi"), Context), FString(TEXT("Yes,")));
	TestEqual(TEXT("it shows once the phrase goes elsewhere"), SafeToShow(TEXT("Yes, Vashir watches you "), Context), FString(TEXT("Yes, Vashir watches you")));
	TestEqual(TEXT("unlocked phrase is not held back"), SafeToShow(TEXT("Along the silk "), Context), FString(TEXT("Along the silk")));
	TestEqual(TEXT("safety phrase start waits"), SafeToShow(TEXT("Go and kill yo"), Context), FString(TEXT("Go and")));
	TestEqual(TEXT("not inside another word"), SafeToShow(TEXT("Its contrast is "), Context), FString(TEXT("Its contrast is")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesReplyStreamBlocked, "Ashes.ReplyStream.StopOnSpoiler", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesReplyStreamBlocked::RunTest(const FString& Parameters)
{
	FAshesValidationContext Context;
	Context.Chapter = 1;
	FSpoilerKeyword Never;  Never.Keyword = TEXT("contract"); Never.UnlockChapter = 0;
	FSpoilerKeyword Later;  Later.Keyword = TEXT("Vashir hired"); Later.UnlockChapter = 4;
	FSpoilerKeyword Open;   Open.Keyword = TEXT("dance"); Open.UnlockChapter = 1;
	Context.SpoilerKeywords = { Never, Later, Open };
	Context.SafetyWords = UResponseValidator::DefaultSafetyWords();

	TestFalse(TEXT("plain words"), AshesReplyStream::IsBlocked(TEXT("Following me after the"), Context));
	TestTrue(TEXT("never-allowed keyword"), AshesReplyStream::IsBlocked(TEXT("There is a Contract on"), Context));
	TestTrue(TEXT("keyword of a later chapter"), AshesReplyStream::IsBlocked(TEXT("Yes, Vashir hired me"), Context));
	TestFalse(TEXT("keyword unlocked in this chapter"), AshesReplyStream::IsBlocked(TEXT("You dance well"), Context));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesStreamContentSoFar, "Ashes.ReplyStream.ContentSoFar", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesStreamContentSoFar::RunTest(const FString& Parameters)
{
	const FString Body =
		TEXT("data: {\"choices\":[{\"delta\":{\"content\":\"{\\\"reply\\\": \\\"Follow\"}}]}\n\n")
		TEXT("data: {\"choices\":[{\"delta\":{\"content\":\"ing me\"}}]}\n\n")
		TEXT("data: {\"choices\":[{\"delta\":{\"cont"); // cut in the middle
	const FString SoFar = UOpenAICompatibleLLMProvider::ContentSoFar(Body);
	TestEqual(TEXT("complete chunks joined, the cut one skipped"), SoFar, FString(TEXT("{\"reply\": \"Following me")));
	TestEqual(TEXT("and the reply read from it"), AshesReplyStream::ExtractReply(SoFar), FString(TEXT("Following me")));
	TestEqual(TEXT("empty body"), UOpenAICompatibleLLMProvider::ContentSoFar(FString()), FString());
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
