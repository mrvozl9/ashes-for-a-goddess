// AI-assisted: Claude Code, 13.11.2026, Automation tests for the offline dialogue tree walker. Reviewed by <pending>.
// Run: Session Frontend > Automation, filter "Ashes.OfflineTree" (or console: Automation RunTests Ashes.OfflineTree)

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "OfflineTreeWalker.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace AshesOfflineTreeTest
{
	const FName TestScene(TEXT("Ch1_ZafirKeyScene"));

	FOfflineNodeRow MakeRow(FName Scene, int32 Node, const TCHAR* Player, const TCHAR* Line, int32 Next)
	{
		FOfflineNodeRow Row;
		Row.Scene = Scene;
		Row.Character = TEXT("Zafir");
		Row.Node = Node;
		Row.PlayerLine = Player;
		Row.CharacterLine = Line;
		Row.Emotion = TEXT("teasing");
		Row.NextNode = Next;
		return Row;
	}

	/** 3 nodes, 3 replies each; every path gives sahra_confronts_blade at node 1 and uneasy_truce at node 3 */
	TArray<FOfflineNodeRow> MakeTree()
	{
		TArray<FOfflineNodeRow> Rows;

		// Another scene first: must be ignored
		Rows.Add(MakeRow(TEXT("Other_Scene"), 1, TEXT("Other reply."), TEXT("Other line."), 0));

		FOfflineNodeRow A = MakeRow(TestScene, 1, TEXT("Put the blade down."), TEXT("[smirks] Make me."), 2);
		A.Changes = TEXT("trust:2;affection:-1");
		A.Beats = TEXT("sahra_confronts_blade");
		FOfflineNodeRow B = MakeRow(TestScene, 1, TEXT("Who sent you here?"), TEXT("A friend of the crown."), 2);
		B.Beats = TEXT("sahra_confronts_blade");
		B.Clues = TEXT("zafir_crown_friend");
		FOfflineNodeRow C = MakeRow(TestScene, 1, TEXT("Ignore him and walk away."), TEXT("[laughs] Bold."), 3);
		C.Beats = TEXT("sahra_confronts_blade");
		Rows.Add(A); Rows.Add(B); Rows.Add(C);

		Rows.Add(MakeRow(TestScene, 2, TEXT("Tell me the truth."), TEXT("Truth costs."), 3));
		Rows.Add(MakeRow(TestScene, 2, TEXT("I am not afraid of you."), TEXT("You should be."), 3));
		Rows.Add(MakeRow(TestScene, 2, TEXT("Give me the key."), TEXT("Ask nicer."), 3));

		for (const TCHAR* Player : { TEXT("Fine, a truce."), TEXT("Keep your secrets."), TEXT("Stay out of my way.") })
		{
			FOfflineNodeRow Row = MakeRow(TestScene, 3, Player, TEXT("[nods] Truce, then."), 0);
			Row.Beats = TEXT("uneasy_truce");
			Rows.Add(Row);
		}
		return Rows;
	}

	/** Walks every path (copy per branch). Returns paths finished; BadPaths counts paths missing a required beat, loops or dead ends. */
	int32 WalkAllPaths(const FOfflineTreeWalker& Walker, const TSet<FName>& Collected, const TSet<FName>& Required, int32 Depth, int32& BadPaths)
	{
		if (Depth > 20 || !Walker.IsActive())
		{
			++BadPaths;
			return 1;
		}
		int32 Finished = 0;
		const int32 Count = Walker.GetReplies().Num();
		for (int32 i = 0; i < Count; ++i)
		{
			FOfflineTreeWalker Branch = Walker;
			const FOfflineStep Step = Branch.Pick(i, TMap<FName, int32>(), TSet<FName>());
			TSet<FName> Got = Collected;
			Got.Append(Step.Beats);
			if (Step.bEnded)
			{
				++Finished;
				if (!Got.Includes(Required))
				{
					++BadPaths;
				}
			}
			else
			{
				Finished += WalkAllPaths(Branch, Got, Required, Depth + 1, BadPaths);
			}
		}
		return Finished;
	}

	/** Generic: walks every path of Scene and returns how many ended without all RequiredBeats (or looped). OutPaths = paths walked. */
	int32 CountBadPaths(const TArray<FOfflineNodeRow>& Rows, FName Scene, const TArray<FName>& RequiredBeats, int32& OutPaths)
	{
		FOfflineTreeWalker Walker;
		if (!Walker.Start(Rows, Scene))
		{
			OutPaths = 0;
			return 1;
		}
		int32 Bad = 0;
		OutPaths = WalkAllPaths(Walker, TSet<FName>(), TSet<FName>(RequiredBeats), 0, Bad);
		return Bad;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesOfflineTreeWalkToEnd, "Ashes.OfflineTree.WalkToEnd", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesOfflineTreeWalkToEnd::RunTest(const FString& Parameters)
{
	const TArray<FOfflineNodeRow> Rows = AshesOfflineTreeTest::MakeTree();
	FOfflineTreeWalker Walker;
	TestFalse(TEXT("unknown scene"), Walker.Start(Rows, TEXT("No_Such_Scene")));
	TestFalse(TEXT("not active after failed start"), Walker.IsActive());

	TestTrue(TEXT("start"), Walker.Start(Rows, AshesOfflineTreeTest::TestScene));
	TestTrue(TEXT("active"), Walker.IsActive());
	TestEqual(TEXT("node 1"), Walker.GetNode(), 1);
	TestEqual(TEXT("3 replies"), Walker.GetReplies().Num(), 3);

	const TMap<FName, int32> NoValues;
	const TSet<FName> NoClues;
	const FOfflineStep S1 = Walker.Pick(0, NoValues, NoClues);
	TestTrue(TEXT("pick 1 valid"), S1.bValid);
	TestFalse(TEXT("pick 1 not ended"), S1.bEnded);
	TestFalse(TEXT("no stand-in"), S1.bStandIn);
	TestEqual(TEXT("line"), S1.Line, FString(TEXT("[smirks] Make me.")));
	TestEqual(TEXT("2 changes"), S1.Changes.Num(), 2);
	TestEqual(TEXT("trust +2"), S1.Changes.FindRef(TEXT("trust")), 2);
	TestEqual(TEXT("affection -1"), S1.Changes.FindRef(TEXT("affection")), -1);
	TestTrue(TEXT("beat"), S1.Beats.Contains(FName(TEXT("sahra_confronts_blade"))));
	TestEqual(TEXT("node 2"), Walker.GetNode(), 2);
	TestEqual(TEXT("next replies"), S1.NextReplies.Num(), 3);

	const FOfflineStep S2 = Walker.Pick(1, NoValues, NoClues);
	TestTrue(TEXT("pick 2 valid"), S2.bValid);
	TestEqual(TEXT("node 3"), Walker.GetNode(), 3);

	const FOfflineStep S3 = Walker.Pick(0, NoValues, NoClues);
	TestTrue(TEXT("pick 3 valid"), S3.bValid);
	TestTrue(TEXT("ended"), S3.bEnded);
	TestTrue(TEXT("final beat"), S3.Beats.Contains(FName(TEXT("uneasy_truce"))));
	TestEqual(TEXT("no next replies"), S3.NextReplies.Num(), 0);
	TestFalse(TEXT("inactive"), Walker.IsActive());
	TestFalse(TEXT("pick after end invalid"), Walker.Pick(0, NoValues, NoClues).bValid);

	// Clue parsing on the second reply of node 1
	FOfflineTreeWalker Second;
	Second.Start(Rows, AshesOfflineTreeTest::TestScene);
	const FOfflineStep Clue = Second.Pick(1, NoValues, NoClues);
	TestTrue(TEXT("clue given"), Clue.Clues.Contains(FName(TEXT("zafir_crown_friend"))));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesOfflineTreeEveryPath, "Ashes.OfflineTree.EveryPathReachesRequiredBeats", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesOfflineTreeEveryPath::RunTest(const FString& Parameters)
{
	const TArray<FOfflineNodeRow> Rows = AshesOfflineTreeTest::MakeTree();
	int32 Paths = 0;
	const int32 Bad = AshesOfflineTreeTest::CountBadPaths(Rows, AshesOfflineTreeTest::TestScene,
		{ FName(TEXT("sahra_confronts_blade")), FName(TEXT("uneasy_truce")) }, Paths);
	TestEqual(TEXT("bad paths"), Bad, 0);
	TestTrue(TEXT("paths walked"), Paths > 0);

	// The helper must notice a missing beat
	int32 Paths2 = 0;
	const int32 Bad2 = AshesOfflineTreeTest::CountBadPaths(Rows, AshesOfflineTreeTest::TestScene,
		{ FName(TEXT("never_given")) }, Paths2);
	TestTrue(TEXT("missing beat detected"), Bad2 > 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesOfflineTreeStandIn, "Ashes.OfflineTree.StandIn", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesOfflineTreeStandIn::RunTest(const FString& Parameters)
{
	using namespace AshesOfflineTreeTest;
	const FName Scene(TEXT("StandInScene"));

	TArray<FOfflineNodeRow> Rows;
	FOfflineNodeRow Gated = MakeRow(Scene, 1, TEXT("Ask about the key."), TEXT("Here it is."), 2);
	Gated.Clues = TEXT("key_clue");
	Gated.RequiresValue = TEXT("trust");
	Gated.RequiresMin = 30;
	Gated.StandInLine = TEXT("Not yet.");
	Gated.StandInEmotion = TEXT("cold");
	Gated.StandInNextNode = 3;
	Rows.Add(Gated);
	Rows.Add(MakeRow(Scene, 2, TEXT("Thanks."), TEXT("Welcome."), 0));
	Rows.Add(MakeRow(Scene, 3, TEXT("Fine."), TEXT("Good."), 0));

	const TSet<FName> NoClues;

	{
		FOfflineTreeWalker W;
		W.Start(Rows, Scene);
		TMap<FName, int32> Values;
		Values.Add(TEXT("trust"), 20);
		const FOfflineStep S = W.Pick(0, Values, NoClues);
		TestTrue(TEXT("stand-in used"), S.bStandIn);
		TestEqual(TEXT("stand-in line"), S.Line, FString(TEXT("Not yet.")));
		TestTrue(TEXT("stand-in emotion"), S.Emotion == FName(TEXT("cold")));
		TestEqual(TEXT("no clue"), S.Clues.Num(), 0);
		TestEqual(TEXT("stand-in next node"), W.GetNode(), 3);
	}
	{
		FOfflineTreeWalker W;
		W.Start(Rows, Scene);
		TMap<FName, int32> Values;
		Values.Add(TEXT("trust"), 30);
		const FOfflineStep S = W.Pick(0, Values, NoClues);
		TestFalse(TEXT("rule passed"), S.bStandIn);
		TestEqual(TEXT("normal line"), S.Line, FString(TEXT("Here it is.")));
		TestTrue(TEXT("clue given"), S.Clues.Contains(FName(TEXT("key_clue"))));
		TestEqual(TEXT("normal next node"), W.GetNode(), 2);
	}
	{
		// Missing value counts as 0
		FOfflineTreeWalker W;
		W.Start(Rows, Scene);
		TestTrue(TEXT("missing value fails"), W.Pick(0, TMap<FName, int32>(), NoClues).bStandIn);
	}

	// Clue rule
	const FName ClueScene(TEXT("StandInClueScene"));
	TArray<FOfflineNodeRow> ClueRows;
	FOfflineNodeRow ClueGated = MakeRow(ClueScene, 1, TEXT("Show the letter."), TEXT("You found it."), 2);
	ClueGated.RequiresClue = TEXT("letter");
	ClueGated.StandInLine = TEXT("What letter?");
	ClueGated.StandInNextNode = 3;
	ClueRows.Add(ClueGated);
	ClueRows.Add(MakeRow(ClueScene, 2, TEXT("Yes."), TEXT("Hm."), 0));
	ClueRows.Add(MakeRow(ClueScene, 3, TEXT("Never mind."), TEXT("Hm."), 0));
	{
		FOfflineTreeWalker W;
		W.Start(ClueRows, ClueScene);
		const FOfflineStep S = W.Pick(0, TMap<FName, int32>(), NoClues);
		TestTrue(TEXT("clue missing: stand-in"), S.bStandIn);
		TestEqual(TEXT("stand-in node"), W.GetNode(), 3);
	}
	{
		FOfflineTreeWalker W;
		W.Start(ClueRows, ClueScene);
		TSet<FName> Known;
		Known.Add(TEXT("letter"));
		const FOfflineStep S = W.Pick(0, TMap<FName, int32>(), Known);
		TestFalse(TEXT("clue known: normal"), S.bStandIn);
		TestEqual(TEXT("normal node"), W.GetNode(), 2);
	}

	// Empty StandInLine: the rule counts as passed
	const FName NoneScene(TEXT("StandInNoneScene"));
	TArray<FOfflineNodeRow> NoneRows;
	FOfflineNodeRow Unauthored = MakeRow(NoneScene, 1, TEXT("Ask."), TEXT("Normal line."), 2);
	Unauthored.RequiresValue = TEXT("trust");
	Unauthored.RequiresMin = 99;
	NoneRows.Add(Unauthored);
	NoneRows.Add(MakeRow(NoneScene, 2, TEXT("Ok."), TEXT("Ok."), 0));
	{
		FOfflineTreeWalker W;
		W.Start(NoneRows, NoneScene);
		const FOfflineStep S = W.Pick(0, TMap<FName, int32>(), NoClues);
		TestFalse(TEXT("no stand-in authored"), S.bStandIn);
		TestEqual(TEXT("normal line used"), S.Line, FString(TEXT("Normal line.")));
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesOfflineTreeMatchReply, "Ashes.OfflineTree.MatchReply", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesOfflineTreeMatchReply::RunTest(const FString& Parameters)
{
	const TArray<FOfflineNodeRow> Rows = AshesOfflineTreeTest::MakeTree();
	FOfflineTreeWalker W;
	W.Start(Rows, AshesOfflineTreeTest::TestScene);

	TestEqual(TEXT("exact, other case, no period"), W.MatchReply(TEXT("put the BLADE down")), 0);
	TestEqual(TEXT("exact, no question mark"), W.MatchReply(TEXT("  who sent you here ")), 1);
	TestEqual(TEXT("exact, third"), W.MatchReply(TEXT("Ignore him and walk away.")), 2);
	TestEqual(TEXT("keyword"), W.MatchReply(TEXT("I will drop the blade, Zafir")), 0);
	TestEqual(TEXT("nothing matches"), W.MatchReply(TEXT("banana smoothie")), -1);
	TestEqual(TEXT("tie"), W.MatchReply(TEXT("blade sent")), -1);
	TestEqual(TEXT("empty"), W.MatchReply(TEXT("")), -1);

	const TMap<FName, int32> NoValues;
	const TSet<FName> NoClues;
	TestFalse(TEXT("index too high"), W.Pick(3, NoValues, NoClues).bValid);
	TestFalse(TEXT("negative index"), W.Pick(-1, NoValues, NoClues).bValid);
	TestEqual(TEXT("node unchanged"), W.GetNode(), 1);
	TestTrue(TEXT("still active"), W.IsActive());
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
