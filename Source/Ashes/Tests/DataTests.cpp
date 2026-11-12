// AI-assisted: Claude Code, 12.11.2026, Automation tests for the CSV import and the data checks (Phase 4 data). Reviewed by Baha.
// Run: Session Frontend > Automation, filter "Ashes.Data" (or console: Automation RunTests Ashes.Data)

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Engine/DataTable.h"
#include "AshesDataValidator.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace AshesDataTestData
{
	const TCHAR* Characters = TEXT("DT_Characters");
	const TCHAR* Scenes = TEXT("DT_Scenes");
	const TCHAR* Clues = TEXT("DT_Clues");
	const TCHAR* Fallback = TEXT("DT_FallbackLines");
	const TCHAR* Offline = TEXT("DT_OfflineTrees");

	UDataTable* MakeTable(UScriptStruct* RowStruct)
	{
		UDataTable* Table = NewObject<UDataTable>(GetTransientPackage());
		Table->RowStruct = RowStruct;
		return Table;
	}

	FCharacterCardRow MakeCard(const TCHAR* Name)
	{
		FCharacterCardRow Card;
		Card.DisplayName = Name;
		Card.Voice = TEXT("Short and dry.");
		Card.SpeechRules = TEXT("Never more than two sentences.");
		Card.Emotions = { TEXT("neutral"), TEXT("teasing") };
		Card.FallbackEmotion = TEXT("neutral");
		Card.TurnLimit = 11;
		FRelationshipValueDef Trust;
		Trust.Name = TEXT("trust");
		Trust.Start = 20;
		Card.Values = { Trust };
		return Card;
	}

	FSceneCardRow MakeScene()
	{
		FSceneCardRow Scene;
		Scene.Chapter = 1;
		Scene.GoalText = TEXT("Sahra confronts Zafir about the blade.");
		Scene.Opener = TEXT("Careful, Little Ember.");
		Scene.TurnLimit = 11;
		Scene.Characters = { TEXT("Zafir") };
		FBeatDef Beat;
		Beat.Id = TEXT("uneasy_truce");
		Beat.Rule = TEXT("They agree to keep each other's secret.");
		Scene.Beats = { Beat };
		return Scene;
	}

	FClueRow MakeClue(const TCHAR* Holder)
	{
		FClueRow Clue;
		Clue.Chapter = 1;
		Clue.Holder = Holder;
		Clue.Source = EClueSource::Talk;
		Clue.JournalText = TEXT("Nura: Zafir is never there after the dance.");
		Clue.TopicText = TEXT("Where does Zafir go after the dance?");
		Clue.PromptHint = TEXT("If Sahra asks about Zafir, say he is never there after the dance.");
		return Clue;
	}

	FFallbackLineRow MakeFallback()
	{
		FFallbackLineRow Line;
		Line.Character = TEXT("Zafir");
		Line.Scene = TEXT("Ch1_ZafirKeyScene");
		Line.Beat = TEXT("uneasy_truce");
		Line.Emotion = TEXT("teasing");
		Line.Clue = TEXT("ch1_nura_disappears");
		Line.Line = TEXT("Ask nicely and I might lie to you more gently.");
		return Line;
	}

	FOfflineNodeRow MakeNode(int32 Node, int32 NextNode)
	{
		FOfflineNodeRow Row;
		Row.Scene = TEXT("Ch1_ZafirKeyScene");
		Row.Character = TEXT("Zafir");
		Row.Node = Node;
		Row.PlayerLine = TEXT("You carry a blade. Why?");
		Row.CharacterLine = TEXT("A blade? On a dancer?");
		Row.Emotion = TEXT("teasing");
		Row.Changes = TEXT("trust:+2");
		Row.Beats = TEXT("uneasy_truce");
		Row.NextNode = NextNode;
		return Row;
	}

	/** A small, correct data set: Zafir and Nura, one scene, one clue, one fallback line, a two-node tree */
	struct FSet
	{
		UDataTable* Characters = MakeTable(FCharacterCardRow::StaticStruct());
		UDataTable* Scenes = MakeTable(FSceneCardRow::StaticStruct());
		UDataTable* Clues = MakeTable(FClueRow::StaticStruct());
		UDataTable* Fallback = MakeTable(FFallbackLineRow::StaticStruct());
		UDataTable* Offline = MakeTable(FOfflineNodeRow::StaticStruct());

		FSet()
		{
			Characters->AddRow(TEXT("Zafir"), MakeCard(TEXT("Zafir")));
			Characters->AddRow(TEXT("Nura"), MakeCard(TEXT("Nura")));
			Scenes->AddRow(TEXT("Ch1_ZafirKeyScene"), MakeScene());
			Clues->AddRow(TEXT("ch1_nura_disappears"), MakeClue(TEXT("Nura")));
			Fallback->AddRow(TEXT("fb_1"), MakeFallback());
			Offline->AddRow(TEXT("n1_r1"), MakeNode(1, 2));
			Offline->AddRow(TEXT("n2_r1"), MakeNode(2, 0));
		}

		FAshesDataReport Validate() const
		{
			FAshesDataTables Tables;
			Tables.Characters = Characters;
			Tables.Scenes = Scenes;
			Tables.Clues = Clues;
			Tables.FallbackLines = Fallback;
			Tables.OfflineTrees = Offline;

			FAshesDataReport Report;
			AshesData::Validate(Tables, Report);
			return Report;
		}
	};

	const TCHAR* ClueCsv = TEXT(
		"Id,Chapter,Holder,Source,JournalText,TopicText,PromptHint,RequiresClue\n"
		"ch1_nura_disappears,1,Nura,Talk,\"Nura: Zafir never misses a dance, and he is never there after it.\",\"Where does Zafir go after the dance?\",\"If Sahra asks about Zafir, say so.\",\n"
		"ch1_guards_blade,1,Guards at the lattice screen,Eavesdrop,\"Two guards:\nsomeone saw a blade under Zafir's silks.\",\"They say Zafir carries a blade.\",,ch1_nura_disappears\n");
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesDataParsers, "Ashes.Data.Parsers", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesDataParsers::RunTest(const FString& Parameters)
{
	const TArray<FName> Ids = AshesData::SplitIds(TEXT(" a ; b;;c "));
	TestEqual(TEXT("three ids"), Ids.Num(), 3);
	TestEqual(TEXT("ids are trimmed"), Ids.IsValidIndex(1) ? Ids[1] : NAME_None, FName(TEXT("b")));

	TMap<FName, int32> Changes;
	FString Error;
	TestTrue(TEXT("changes parse"), AshesData::ParseChanges(TEXT("Trust:+2; affection:-3"), Changes, &Error));
	TestEqual(TEXT("trust +2"), Changes.FindRef(TEXT("trust")), 2);
	TestEqual(TEXT("affection -3"), Changes.FindRef(TEXT("affection")), -3);
	TestTrue(TEXT("\"--\" means no change"), AshesData::ParseChanges(TEXT("--"), Changes) && Changes.Num() == 0);
	TestFalse(TEXT("missing amount is refused"), AshesData::ParseChanges(TEXT("trust"), Changes, &Error));
	TestFalse(TEXT("text amount is refused"), AshesData::ParseChanges(TEXT("trust:lots"), Changes, &Error));
	TestFalse(TEXT("a value listed twice is refused"), AshesData::ParseChanges(TEXT("trust:1;trust:2"), Changes, &Error));

	TArray<TPair<float, int32>> Notes;
	TestTrue(TEXT("notes parse"), AshesData::ParseNotes(TEXT("4:1; 6.5:3"), Notes, &Error));
	TestEqual(TEXT("two notes"), Notes.Num(), 2);
	TestEqual(TEXT("second note lane"), Notes.IsValidIndex(1) ? Notes[1].Value : 0, 3);
	TestFalse(TEXT("a note without a lane is refused"), AshesData::ParseNotes(TEXT("4"), Notes, &Error));

	TestTrue(TEXT("empty is any"), AshesData::IsAny(NAME_None));
	TestTrue(TEXT("Any is any"), AshesData::IsAny(TEXT("Any")));
	TestFalse(TEXT("a beat id is not any"), AshesData::IsAny(TEXT("uneasy_truce")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesDataCsvIds, "Ashes.Data.CsvIds", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesDataCsvIds::RunTest(const FString& Parameters)
{
	using namespace AshesDataTestData;

	FAshesDataReport Clean;
	TestEqual(TEXT("two data rows"), AshesData::CheckCsvIds(ClueCsv, Clues, Clean), 2);
	TestEqual(TEXT("no id problems in a clean CSV"), Clean.NumErrors(), 0);

	const TCHAR* Broken = TEXT(
		"Id,Chapter\n"
		"ch1_a,1\n"
		",1\n"
		"CH1_A,2\n"
		"\n"
		"ch1_b,1\n");
	FAshesDataReport Report;
	TestEqual(TEXT("blank lines are not rows"), AshesData::CheckCsvIds(Broken, Clues, Report), 4);
	TestEqual(TEXT("two id errors"), Report.NumErrors(), 2);
	TestTrue(TEXT("empty id reported with its CSV row"), Report.Has(EAshesDataSeverity::Error, Clues, TEXT("CSV row 3"), TEXT("empty")));
	TestTrue(TEXT("duplicate id reported (case ignored)"), Report.Has(EAshesDataSeverity::Error, Clues, TEXT("CH1_A"), TEXT("duplicate id")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesDataCsvImport, "Ashes.Data.CsvImport", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesDataCsvImport::RunTest(const FString& Parameters)
{
	using namespace AshesDataTestData;

	UDataTable* Table = MakeTable(FClueRow::StaticStruct());
	const TArray<FString> Problems = Table->CreateTableFromCSVString(ClueCsv);
	for (const FString& Problem : Problems)
	{
		AddError(FString::Printf(TEXT("import problem: %s"), *Problem));
	}
	TestEqual(TEXT("two rows imported"), Table->GetRowMap().Num(), 2);

	const FClueRow* Guards = Table->FindRow<FClueRow>(TEXT("ch1_guards_blade"), TEXT("test"), false);
	if (TestNotNull(TEXT("eavesdrop row"), Guards))
	{
		TestTrue(TEXT("enum column"), Guards->Source == EClueSource::Eavesdrop);
		TestEqual(TEXT("free-text holder"), Guards->Holder, FString(TEXT("Guards at the lattice screen")));
		TestTrue(TEXT("line break inside quotes is kept"), Guards->JournalText.Contains(TEXT("\n")));
		TestEqual(TEXT("requires clue"), Guards->RequiresClue, FName(TEXT("ch1_nura_disappears")));
	}

	// Merve changes a text in the CSV and imports again: the table shows the new text, no code change
	const FString Edited = FString(ClueCsv).Replace(TEXT("Where does Zafir go after the dance?"), TEXT("Where does he vanish to?"));
	Table->CreateTableFromCSVString(Edited);
	const FClueRow* Nura = Table->FindRow<FClueRow>(TEXT("ch1_nura_disappears"), TEXT("test"), false);
	if (TestNotNull(TEXT("talk row after reimport"), Nura))
	{
		TestEqual(TEXT("reimport shows the edited text"), Nura->TopicText, FString(TEXT("Where does he vanish to?")));
	}
	TestEqual(TEXT("still two rows after reimport"), Table->GetRowMap().Num(), 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesDataValidSet, "Ashes.Data.ValidSet", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesDataValidSet::RunTest(const FString& Parameters)
{
	const AshesDataTestData::FSet Set;
	const FAshesDataReport Report = Set.Validate();
	if (Report.NumErrors() != 0 || Report.NumWarnings() != 0)
	{
		AddError(Report.ToText());
	}
	TestEqual(TEXT("row counts for the five tables"), Report.RowCounts.Num(), 5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesDataMissingReferences, "Ashes.Data.MissingReferences", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesDataMissingReferences::RunTest(const FString& Parameters)
{
	using namespace AshesDataTestData;
	const EAshesDataSeverity Err = EAshesDataSeverity::Error;

	FSet Set;

	FSceneCardRow Scene = MakeScene();
	Scene.Characters = { TEXT("Ghost") };
	Set.Scenes->AddRow(TEXT("Ch9_Nowhere"), Scene);

	FClueRow Clue = MakeClue(TEXT("Nobody"));
	Clue.RequiresClue = TEXT("ch1_missing");
	Set.Clues->AddRow(TEXT("ch1_bad"), Clue);

	FFallbackLineRow Line = MakeFallback();
	Line.Character = TEXT("Ghost");
	Set.Fallback->AddRow(TEXT("fb_character"), Line);
	Line = MakeFallback();
	Line.Scene = TEXT("Ch9_Missing");
	Set.Fallback->AddRow(TEXT("fb_scene"), Line);
	Line = MakeFallback();
	Line.Beat = TEXT("no_such_beat");
	Set.Fallback->AddRow(TEXT("fb_beat"), Line);
	Line = MakeFallback();
	Line.Emotion = TEXT("furious");
	Set.Fallback->AddRow(TEXT("fb_emotion"), Line);
	Line = MakeFallback();
	Line.Clue = TEXT("ch1_missing");
	Set.Fallback->AddRow(TEXT("fb_clue"), Line);

	FOfflineNodeRow Node = MakeNode(2, 7);
	Node.Changes = TEXT("favor:3");
	Node.Beats = TEXT("no_such_beat");
	Node.Clues = TEXT("ch1_missing");
	Set.Offline->AddRow(TEXT("n2_bad"), Node);

	const FAshesDataReport Report = Set.Validate();
	TestTrue(TEXT("scene -> character"), Report.Has(Err, Scenes, TEXT("Ch9_Nowhere"), TEXT("\"Ghost\" is not in DT_Characters")));
	TestTrue(TEXT("talk clue -> holder"), Report.Has(Err, Clues, TEXT("ch1_bad"), TEXT("holder \"Nobody\"")));
	TestTrue(TEXT("clue -> required clue"), Report.Has(Err, Clues, TEXT("ch1_bad"), TEXT("RequiresClue: clue \"ch1_missing\"")));
	TestTrue(TEXT("fallback -> character"), Report.Has(Err, Fallback, TEXT("fb_character"), TEXT("not in DT_Characters")));
	TestTrue(TEXT("fallback -> scene"), Report.Has(Err, Fallback, TEXT("fb_scene"), TEXT("not in DT_Scenes")));
	TestTrue(TEXT("fallback -> beat"), Report.Has(Err, Fallback, TEXT("fb_beat"), TEXT("beat \"no_such_beat\"")));
	TestTrue(TEXT("fallback -> emotion"), Report.Has(Err, Fallback, TEXT("fb_emotion"), TEXT("emotion \"furious\"")));
	TestTrue(TEXT("fallback -> clue"), Report.Has(Err, Fallback, TEXT("fb_clue"), TEXT("clue \"ch1_missing\"")));
	TestTrue(TEXT("offline -> next node"), Report.Has(Err, Offline, TEXT("n2_bad"), TEXT("NextNode 7")));
	TestTrue(TEXT("offline -> value name"), Report.Has(Err, Offline, TEXT("n2_bad"), TEXT("\"favor\" is not a relationship value")));
	TestTrue(TEXT("offline -> beat"), Report.Has(Err, Offline, TEXT("n2_bad"), TEXT("beat \"no_such_beat\"")));
	TestTrue(TEXT("offline -> clue"), Report.Has(Err, Offline, TEXT("n2_bad"), TEXT("clue \"ch1_missing\"")));

	// The correct rows of the set must stay clean
	TestFalse(TEXT("good fallback row has no error"), Report.Has(Err, Fallback, TEXT("fb_1"), TEXT("")));
	TestFalse(TEXT("good clue row has no error"), Report.Has(Err, Clues, TEXT("ch1_nura_disappears"), TEXT("")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesDataEmptyFields, "Ashes.Data.EmptyRequiredFields", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesDataEmptyFields::RunTest(const FString& Parameters)
{
	using namespace AshesDataTestData;
	const EAshesDataSeverity Err = EAshesDataSeverity::Error;

	FSet Set;

	FCharacterCardRow Card = MakeCard(TEXT(""));
	Card.Emotions.Reset();
	Card.TurnLimit = 0;
	Set.Characters->AddRow(TEXT("Blank"), Card);

	FSceneCardRow Scene = MakeScene();
	Scene.Characters.Reset();
	Set.Scenes->AddRow(TEXT("Ch1_Blank"), Scene);

	FClueRow Clue = MakeClue(TEXT("Nura"));
	Clue.JournalText = TEXT("  ");
	Set.Clues->AddRow(TEXT("ch1_blank"), Clue);

	FFallbackLineRow Line = MakeFallback();
	Line.Line.Reset();
	Line.Beat = NAME_None;
	Line.Emotion = TEXT("any");
	Set.Fallback->AddRow(TEXT("fb_blank"), Line);

	FOfflineNodeRow Node = MakeNode(2, 0);
	Node.PlayerLine.Reset();
	Node.CharacterLine.Reset();
	Node.Emotion = NAME_None;
	Node.RequiresValue = TEXT("trust");
	Node.RequiresMin = 30;
	Set.Offline->AddRow(TEXT("n2_blank"), Node);

	const FAshesDataReport Report = Set.Validate();
	TestTrue(TEXT("character DisplayName"), Report.Has(Err, Characters, TEXT("Blank"), TEXT("DisplayName is empty")));
	TestTrue(TEXT("character Emotions"), Report.Has(Err, Characters, TEXT("Blank"), TEXT("Emotions is empty")));
	TestTrue(TEXT("character TurnLimit"), Report.Has(Err, Characters, TEXT("Blank"), TEXT("TurnLimit")));
	TestTrue(TEXT("scene Characters"), Report.Has(Err, Scenes, TEXT("Ch1_Blank"), TEXT("Characters is empty")));
	TestTrue(TEXT("clue JournalText (spaces only)"), Report.Has(Err, Clues, TEXT("ch1_blank"), TEXT("JournalText is empty")));
	TestTrue(TEXT("fallback Line"), Report.Has(Err, Fallback, TEXT("fb_blank"), TEXT("Line is empty")));
	TestFalse(TEXT("\"any\" beat and emotion are allowed"), Report.Has(Err, Fallback, TEXT("fb_blank"), TEXT("beat")) || Report.Has(Err, Fallback, TEXT("fb_blank"), TEXT("emotion")));
	TestTrue(TEXT("offline PlayerLine"), Report.Has(Err, Offline, TEXT("n2_blank"), TEXT("PlayerLine is empty")));
	TestTrue(TEXT("offline CharacterLine"), Report.Has(Err, Offline, TEXT("n2_blank"), TEXT("CharacterLine is empty")));
	TestTrue(TEXT("offline Emotion"), Report.Has(Err, Offline, TEXT("n2_blank"), TEXT("Emotion is empty")));
	TestTrue(TEXT("offline rule without stand-in"), Report.Has(Err, Offline, TEXT("n2_blank"), TEXT("needs a StandInLine")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesDataWrongStruct, "Ashes.Data.WrongRowStruct", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesDataWrongStruct::RunTest(const FString& Parameters)
{
	using namespace AshesDataTestData;

	// A clue table passed where the scene table belongs must be reported, not read
	FSet Set;
	FAshesDataTables Tables;
	Tables.Characters = Set.Characters;
	Tables.Scenes = Set.Clues;

	FAshesDataReport Report;
	AshesData::Validate(Tables, Report);
	TestTrue(TEXT("wrong row struct reported"), Report.Has(EAshesDataSeverity::Error, Scenes, TEXT(""), TEXT("wrong row struct")));
	TestEqual(TEXT("only that one error"), Report.NumErrors(), 1);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
