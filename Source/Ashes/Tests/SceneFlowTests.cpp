// AI-assisted: Claude Code, 13.11.2026, Automation tests for FSceneFlow (Phase 7). Reviewed by <pending>.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "SceneFlow.h"
#include "SceneDirectorSubsystem.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace SceneFlowTestData
{
	const FName Blade(TEXT("sahra_confronts_blade"));
	const FName Truce(TEXT("uneasy_truce"));
	const FName SceneRow(TEXT("Ch1_ZafirKeyScene"));

	FSceneCardRow MakeScene(int32 Limit)
	{
		FSceneCardRow Scene;
		Scene.TurnLimit = Limit;
		FBeatDef A; A.Id = Blade; A.Rule = TEXT("Sahra holds the blade to Zafir."); A.bRequired = true;
		FBeatDef B; B.Id = Truce; B.Rule = TEXT("They reach an uneasy truce.");     B.bRequired = true;
		Scene.Beats = { A, B };
		return Scene;
	}

	TArray<FSceneRuleRow> MakeRules()
	{
		FSceneRuleRow TruceRule;
		TruceRule.Scene = SceneRow;
		TruceRule.Beat = Truce;
		TruceRule.MinTurn = 4;
		TruceRule.RequiresBeat = Blade;
		return { TruceRule };
	}

	FSceneFlow MakeFlow(int32 Limit, const TArray<FSceneRuleRow>& Rules)
	{
		FSceneFlow Flow;
		Flow.Start(SceneRow, MakeScene(Limit), Rules);
		return Flow;
	}

	/** One counted turn: BeginTurn + ApplyReply */
	TArray<FName> Turn(FSceneFlow& Flow, const TArray<FName>& Claimed, const TMap<FName, int32>& Values = TMap<FName, int32>(), bool bTrusted = false)
	{
		Flow.BeginTurn();
		return Flow.ApplyReply(Claimed, Values, bTrusted);
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesSceneFlowResolveFull, "Ashes.SceneFlow.ResolveFull", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesSceneFlowResolveFull::RunTest(const FString& Parameters)
{
	using namespace SceneFlowTestData;
	FSceneFlow Flow = MakeFlow(11, MakeRules());
	TestTrue(TEXT("Intro at start"), Flow.GetPhase() == EScenePhase::Intro);

	TArray<FName> R = Turn(Flow, { Blade });
	TestTrue(TEXT("FreeTalk after first turn"), Flow.GetPhase() == EScenePhase::FreeTalk);
	TestTrue(TEXT("blade accepted at turn 1"), R.Contains(Blade));

	R = Turn(Flow, { Truce });
	TestEqual(TEXT("truce rejected at turn 2"), R.Num(), 0);
	Turn(Flow, {});
	R = Turn(Flow, { Truce });
	TestEqual(TEXT("turn count"), Flow.GetTurnsTaken(), 4);
	TestTrue(TEXT("truce accepted at turn 4"), R.Contains(Truce));
	TestTrue(TEXT("Resolve"), Flow.GetPhase() == EScenePhase::Resolve);
	TestTrue(TEXT("WasFull"), Flow.WasFull());
	TestTrue(TEXT("IsOver"), Flow.IsOver());

	R = Turn(Flow, { Blade });
	TestEqual(TEXT("nothing after over"), R.Num(), 0);
	TestEqual(TEXT("turns frozen"), Flow.GetTurnsTaken(), 4);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesSceneFlowEarlyClaim, "Ashes.SceneFlow.EarlyClaimCountsLater", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesSceneFlowEarlyClaim::RunTest(const FString& Parameters)
{
	using namespace SceneFlowTestData;
	{
		// The truce is played out at turn 3 (rule: from turn 4). The model will not claim it again,
		// so it must count by itself at turn 4 instead of ending in the weaker closing.
		FSceneFlow Flow = MakeFlow(11, MakeRules());
		Turn(Flow, { Blade });
		Turn(Flow, {});
		TArray<FName> R = Turn(Flow, { Truce });
		TestEqual(TEXT("too early at turn 3"), R.Num(), 0);
		TestFalse(TEXT("not over yet"), Flow.IsOver());
		R = Turn(Flow, {});
		TestTrue(TEXT("counts at turn 4 without a new claim"), R.Contains(Truce));
		TestTrue(TEXT("Resolve"), Flow.GetPhase() == EScenePhase::Resolve && Flow.WasFull());
	}
	{
		// Claimed before the beat it depends on: counts in the reply where that beat arrives
		FSceneFlow Flow = MakeFlow(11, MakeRules());
		for (int32 i = 0; i < 4; ++i) { Flow.BeginTurn(); }
		Flow.BeginTurn(); // turn 5
		TestEqual(TEXT("truce waits for blade"), Flow.ApplyReply({ Truce }, {}).Num(), 0);
		Flow.BeginTurn();
		const TArray<FName> R = Flow.ApplyReply({ Blade }, {});
		TestEqual(TEXT("blade brings the waiting truce"), R.Num(), 2);
		TestTrue(TEXT("Resolve"), Flow.GetPhase() == EScenePhase::Resolve);
	}
	{
		// A failed value rule is not waited out
		FSceneRuleRow Rule;
		Rule.Scene = SceneRow;
		Rule.Beat = Truce;
		Rule.RequiresValue = TEXT("trust");
		Rule.RequiresMin = 30;
		FSceneFlow Flow = MakeFlow(11, { Rule });
		TMap<FName, int32> Low;  Low.Add(TEXT("trust"), 20);
		TMap<FName, int32> High; High.Add(TEXT("trust"), 40);
		TestEqual(TEXT("rejected below the value"), Turn(Flow, { Truce }, Low).Num(), 0);
		TestEqual(TEXT("not remembered when the value rises"), Turn(Flow, {}, High).Num(), 0);
		TestTrue(TEXT("a new claim counts"), Turn(Flow, { Truce }, High).Contains(Truce));
	}
	return true;
}

// AI-assisted: Claude Code, 13.11.2026, reply buttons always 3 (Phase 6 fix 4). Reviewed by <pending>.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesSuggestionTopUp, "Ashes.SceneDirector.SuggestionsTopUp", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesSuggestionTopUp::RunTest(const FString& Parameters)
{
	using namespace SceneFlowTestData;
	const TArray<FString> Stock = { TEXT("Why do they say you carry a knife?"), TEXT("Who are you, really?"), TEXT("Why do you call me Little Ember?") };

	TArray<FString> None;
	USceneDirectorSubsystem::TopUpSuggestions(None, Stock);
	TestEqual(TEXT("no suggestions -> 3 stock lines"), None.Num(), 3);

	TArray<FString> One = { TEXT("who are you, really?") };
	USceneDirectorSubsystem::TopUpSuggestions(One, Stock);
	TestEqual(TEXT("one -> 3"), One.Num(), 3);
	TestEqual(TEXT("the model's line stays first"), One[0], FString(TEXT("who are you, really?")));
	TestFalse(TEXT("no duplicate (any case)"), One.Contains(TEXT("Who are you, really?")) && One.Contains(TEXT("who are you, really?")) && One.Num() > 3);

	TArray<FString> Avoid;
	USceneDirectorSubsystem::TopUpSuggestions(Avoid, Stock, TEXT("Why do they say you carry a knife?"));
	TestFalse(TEXT("the line just said is not offered again"), Avoid.Contains(TEXT("Why do they say you carry a knife?")));

	TArray<FString> Full = { TEXT("a"), TEXT("b"), TEXT("c") };
	USceneDirectorSubsystem::TopUpSuggestions(Full, Stock);
	TestEqual(TEXT("3 from the model: unchanged"), Full.Num(), 3);

	// Stock lines come from the scene-level DT_SceneRules row
	FSceneRuleRow SceneRow_;
	SceneRow_.Scene = SceneRow;
	SceneRow_.StockSuggestions = TEXT(" One? ;Two;; Three ");
	FSceneFlow Flow = MakeFlow(11, { SceneRow_ });
	const TArray<FString> Lines = Flow.GetStockSuggestions();
	TestTrue(TEXT("split, trimmed, empty parts dropped"), Lines.Num() == 3 && Lines[0] == TEXT("One?") && Lines[2] == TEXT("Three"));
	TestEqual(TEXT("no scene-level row: none"), MakeFlow(11, MakeRules()).GetStockSuggestions().Num(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesSceneFlowBeatOrder, "Ashes.SceneFlow.BeatOrder", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesSceneFlowBeatOrder::RunTest(const FString& Parameters)
{
	using namespace SceneFlowTestData;
	{
		FSceneFlow Flow = MakeFlow(11, MakeRules());
		for (int32 i = 0; i < 4; ++i) { Flow.BeginTurn(); }
		Flow.BeginTurn(); // turn 5
		TArray<FName> R = Flow.ApplyReply({ Truce }, {});
		TestEqual(TEXT("truce without blade rejected"), R.Num(), 0);
	}
	{
		FSceneFlow Flow = MakeFlow(11, MakeRules());
		for (int32 i = 0; i < 5; ++i) { Flow.BeginTurn(); }
		TArray<FName> R = Flow.ApplyReply({ Blade, Truce }, {});
		TestEqual(TEXT("both accepted in one reply"), R.Num(), 2);
		TestTrue(TEXT("Resolve"), Flow.GetPhase() == EScenePhase::Resolve);
	}
	{
		FSceneFlow Flow = MakeFlow(11, MakeRules());
		Flow.BeginTurn();
		TArray<FName> R = Flow.ApplyReply({ FName(TEXT("not_a_beat")) }, {});
		TestEqual(TEXT("unknown id ignored"), R.Num(), 0);
		R = Flow.ApplyReply({ FName(TEXT("not_a_beat")) }, {}, true);
		TestEqual(TEXT("unknown id ignored when trusted"), R.Num(), 0);
		R = Flow.ApplyReply({ Truce }, {}, true);
		TestTrue(TEXT("trusted ignores MinTurn and order"), R.Contains(Truce));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesSceneFlowNudgeOnStall, "Ashes.SceneFlow.NudgeOnStall", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesSceneFlowNudgeOnStall::RunTest(const FString& Parameters)
{
	using namespace SceneFlowTestData;
	FSceneFlow Flow = MakeFlow(11, MakeRules());
	Turn(Flow, {});
	Turn(Flow, {});
	TestTrue(TEXT("still FreeTalk"), Flow.GetPhase() == EScenePhase::FreeTalk);
	TestTrue(TEXT("no note outside Nudge"), Flow.GetNudgeNote().IsEmpty());
	Turn(Flow, {});
	TestTrue(TEXT("Nudge after 3 stalled turns"), Flow.GetPhase() == EScenePhase::Nudge);
	TestTrue(TEXT("pending is blade"), Flow.GetPendingBeat() == Blade);
	TestTrue(TEXT("default note holds the Rule"), Flow.GetNudgeNote().Contains(TEXT("Sahra holds the blade to Zafir.")));

	// A rule NudgeNote wins over the default
	TArray<FSceneRuleRow> Rules = MakeRules();
	FSceneRuleRow BladeRule;
	BladeRule.Scene = SceneRow;
	BladeRule.Beat = Blade;
	BladeRule.NudgeNote = TEXT("Zafir leaves his dagger unguarded.");
	Rules.Add(BladeRule);
	FSceneFlow Flow2 = MakeFlow(11, Rules);
	Turn(Flow2, {}); Turn(Flow2, {}); Turn(Flow2, {});
	TestEqual(TEXT("rule note used"), Flow2.GetNudgeNote(), FString(TEXT("Zafir leaves his dagger unguarded.")));

	const TArray<FName> R = Turn(Flow, { Blade });
	TestTrue(TEXT("new beat"), R.Contains(Blade));
	TestTrue(TEXT("back to FreeTalk"), Flow.GetPhase() == EScenePhase::FreeTalk);
	TestEqual(TEXT("stall reset"), Flow.GetStalledTurns(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesSceneFlowNudgeNearLimit, "Ashes.SceneFlow.NudgeNearLimit", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesSceneFlowNudgeNearLimit::RunTest(const FString& Parameters)
{
	using namespace SceneFlowTestData;
	FSceneFlow Flow = MakeFlow(5, MakeRules());
	Turn(Flow, {});
	Turn(Flow, {});
	TestTrue(TEXT("FreeTalk with 3 turns left"), Flow.GetPhase() == EScenePhase::FreeTalk);
	Turn(Flow, {}); // turn 3: 2 left, stalled 3 is not needed to be below limit here
	TestEqual(TEXT("turns left"), Flow.GetTurnsLeft(), 2);
	TestTrue(TEXT("Nudge near the limit"), Flow.GetPhase() == EScenePhase::Nudge);

	// Same with a fresh beat on turn 3: stall is 0 but the limit still nudges
	FSceneFlow Flow2 = MakeFlow(5, MakeRules());
	Turn(Flow2, {});
	Turn(Flow2, {});
	Turn(Flow2, { Blade });
	TestEqual(TEXT("stall 0"), Flow2.GetStalledTurns(), 0);
	TestTrue(TEXT("Nudge although not stalled"), Flow2.GetPhase() == EScenePhase::Nudge);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesSceneFlowForcedClose, "Ashes.SceneFlow.ForcedClose", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesSceneFlowForcedClose::RunTest(const FString& Parameters)
{
	using namespace SceneFlowTestData;
	FSceneFlow Flow = MakeFlow(11, MakeRules());
	for (int32 i = 0; i < 5; ++i) { Turn(Flow, {}); }
	TestEqual(TEXT("turns left"), Flow.GetTurnsLeft(), 6);
	Flow.RefundTurn();
	TestEqual(TEXT("refund gives the turn back"), Flow.GetTurnsLeft(), 7);

	for (int32 i = 0; i < 7; ++i) { Turn(Flow, {}); }
	TestTrue(TEXT("ForcedClose at the limit"), Flow.GetPhase() == EScenePhase::ForcedClose);
	TestFalse(TEXT("not full"), Flow.WasFull());
	TestTrue(TEXT("blade set"), Flow.GetReachedBeats().Contains(Blade));
	TestTrue(TEXT("truce set"), Flow.GetReachedBeats().Contains(Truce));
	TestEqual(TEXT("both forced"), Flow.GetForcedOnClose().Num(), 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesSceneFlowCaughtAndForced, "Ashes.SceneFlow.CaughtAndForced", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesSceneFlowCaughtAndForced::RunTest(const FString& Parameters)
{
	using namespace SceneFlowTestData;
	// Caught wins over Resolve
	{
		FSceneFlow Flow = MakeFlow(11, MakeRules());
		TMap<FName, int32> Values;
		Values.Add(FSceneFlow::CaughtValueName, 100);
		Flow.BeginTurn();
		Flow.BeginTurn();
		Flow.BeginTurn();
		Flow.BeginTurn();
		Flow.ApplyReply({ Blade, Truce }, Values);
		TestTrue(TEXT("Caught"), Flow.GetPhase() == EScenePhase::Caught);
		TestFalse(TEXT("not full"), Flow.WasFull());
		TestTrue(TEXT("over"), Flow.IsOver());
	}
	// ForceAtTurn
	{
		TArray<FSceneRuleRow> Rules = MakeRules();
		FSceneRuleRow Forced;
		Forced.Scene = SceneRow;
		Forced.Beat = Truce;
		Forced.ForceAtTurn = 3;
		Rules.Add(Forced);
		FSceneFlow Flow = MakeFlow(11, Rules);
		TArray<FName> R = Turn(Flow, {});
		R = Turn(Flow, {});
		TestEqual(TEXT("not forced yet"), R.Num(), 0);
		R = Turn(Flow, {});
		TestTrue(TEXT("forced at turn 3"), R.Contains(Truce));
		TestTrue(TEXT("reached"), Flow.GetReachedBeats().Contains(Truce));
	}
	// No required beats: resolves only at the limit, full
	{
		FSceneCardRow Scene = MakeScene(3);
		for (FBeatDef& B : Scene.Beats) { B.bRequired = false; }
		FSceneFlow Flow;
		Flow.Start(SceneRow, Scene, {});
		Turn(Flow, { Blade, Truce });
		Turn(Flow, {});
		TestFalse(TEXT("no early resolve"), Flow.IsOver());
		Turn(Flow, {});
		TestTrue(TEXT("Resolve at the limit"), Flow.GetPhase() == EScenePhase::Resolve);
		TestTrue(TEXT("full"), Flow.WasFull());
	}
	// EndEarly
	{
		FSceneFlow Flow = MakeFlow(11, MakeRules());
		Turn(Flow, { Blade });
		Flow.EndEarly();
		TestTrue(TEXT("ForcedClose with missing beat"), Flow.GetPhase() == EScenePhase::ForcedClose);
		TestFalse(TEXT("not full"), Flow.WasFull());
		TestTrue(TEXT("forced truce listed"), Flow.GetForcedOnClose().Contains(Truce));

		FSceneFlow Flow2 = MakeFlow(11, MakeRules());
		Flow2.ForceBeat(Blade);
		Flow2.ForceBeat(Truce);
		Flow2.EndEarly();
		TestTrue(TEXT("Resolve with all beats"), Flow2.GetPhase() == EScenePhase::Resolve);
		TestTrue(TEXT("full"), Flow2.WasFull());
	}
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
