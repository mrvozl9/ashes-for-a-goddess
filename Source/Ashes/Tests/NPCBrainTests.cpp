// AI-assisted: Claude Code, 13.11.2026, Automation tests for the NPC brain component (Phase 7). Reviewed by <pending>.
// Run: Session Frontend > Automation, filter "Ashes.NPCBrain" (or console: Automation RunTests Ashes.NPCBrain)

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "NPCBrainComponent.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace NPCBrainTestData
{
	FRelationshipValueDef MakeDef(const TCHAR* Name, int32 Start, int32 MaxInc, int32 MaxDec)
	{
		FRelationshipValueDef Def;
		Def.Name = FName(Name);
		Def.Start = Start;
		Def.MaxIncrease = MaxInc;
		Def.MaxDecrease = MaxDec;
		return Def;
	}

	FCharacterCardRow MakeZafir()
	{
		FCharacterCardRow Card;
		Card.Values = { MakeDef(TEXT("trust"), 20, 8, 8), MakeDef(TEXT("affection"), 10, 8, 8) };
		return Card;
	}

	FCharacterCardRow MakeVashir()
	{
		FCharacterCardRow Card;
		Card.Values = { MakeDef(TEXT("favor"), 30, 6, 6), MakeDef(TEXT("suspicion"), 10, 10, 5) };
		return Card;
	}

	FValueThresholdRow MakeThreshold(const TCHAR* Character, const TCHAR* Value, int32 AtLeast, const TCHAR* UnlockId, bool bRomanceOnly = false)
	{
		FValueThresholdRow Row;
		Row.Character = FName(Character);
		Row.Value = FName(Value);
		Row.AtLeast = AtLeast;
		Row.UnlockId = FName(UnlockId);
		Row.HintLine = TEXT("hint");
		Row.bRomanceOnly = bRomanceOnly;
		return Row;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesNPCBrainStartAndClamp, "Ashes.NPCBrain.StartAndClamp", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesNPCBrainStartAndClamp::RunTest(const FString& Parameters)
{
	const FName Trust(TEXT("trust"));
	const FName Affection(TEXT("affection"));

	UNPCBrainComponent* Zafir = NewObject<UNPCBrainComponent>();
	TestFalse(TEXT("not initialized yet"), Zafir->IsInitialized());
	Zafir->InitFromCard(TEXT("zafir"), NPCBrainTestData::MakeZafir(), {});
	TestTrue(TEXT("initialized"), Zafir->IsInitialized());
	TestEqual(TEXT("trust start"), Zafir->GetValue(Trust), 20);
	TestEqual(TEXT("affection start"), Zafir->GetValue(Affection), 10);
	TestEqual(TEXT("+50 limited to +8"), Zafir->ApplyDelta(Trust, 50), 28);
	TestEqual(TEXT("-50 limited to -8"), Zafir->ApplyDelta(Trust, -50), 20);

	for (int32 i = 0; i < 30; ++i)
	{
		const int32 Up = Zafir->ApplyDelta(Trust, 8);
		TestTrue(TEXT("value stays in 0..100 (up)"), Up >= 0 && Up <= 100);
	}
	TestEqual(TEXT("capped at 100"), Zafir->GetValue(Trust), 100);
	for (int32 i = 0; i < 30; ++i)
	{
		const int32 Down = Zafir->ApplyDelta(Trust, -8);
		TestTrue(TEXT("value stays in 0..100 (down)"), Down >= 0 && Down <= 100);
	}
	TestEqual(TEXT("floored at 0"), Zafir->GetValue(Trust), 0);

	const FName Unknown(TEXT("nonsense"));
	TestEqual(TEXT("unknown value returns 0"), Zafir->ApplyDelta(Unknown, 5), 0);
	TestFalse(TEXT("unknown value not added"), Zafir->HasValue(Unknown));
	TestEqual(TEXT("unknown value reads 0"), Zafir->GetValue(Unknown), 0);

	UNPCBrainComponent* Vashir = NewObject<UNPCBrainComponent>();
	Vashir->InitFromCard(TEXT("vashir"), NPCBrainTestData::MakeVashir(), {});
	const FName Suspicion(TEXT("suspicion"));
	const FName Favor(TEXT("favor"));
	TestEqual(TEXT("suspicion +99 -> +10"), Vashir->ApplyDelta(Suspicion, 99), 20);
	TestEqual(TEXT("suspicion -99 -> -5"), Vashir->ApplyDelta(Suspicion, -99), 15);
	TestEqual(TEXT("favor +99 -> +6"), Vashir->ApplyDelta(Favor, 99), 36);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesNPCBrainThresholds, "Ashes.NPCBrain.Thresholds", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesNPCBrainThresholds::RunTest(const FString& Parameters)
{
	using namespace NPCBrainTestData;
	const FName Trust(TEXT("trust"));
	const FName Affection(TEXT("affection"));

	const TArray<FValueThresholdRow> Rows = {
		MakeThreshold(TEXT("zafir"), TEXT("trust"), 50, TEXT("admits_hunt")),
		MakeThreshold(TEXT("zafir"), TEXT("trust"), 30, TEXT("one_true_thing")),
		MakeThreshold(TEXT("zafir"), TEXT("affection"), 40, TEXT("jealousy_shows")),
		MakeThreshold(TEXT("vashir"), TEXT("suspicion"), 100, TEXT("caught")),
		MakeThreshold(TEXT("vashir"), TEXT("trust"), 20, TEXT("other_character_row")),
	};

	UNPCBrainComponent* Zafir = NewObject<UNPCBrainComponent>();
	Zafir->InitFromCard(TEXT("zafir"), MakeZafir(), Rows);

	TestFalse(TEXT("one_true_thing locked at start"), Zafir->IsUnlocked(TEXT("one_true_thing")));
	Zafir->ApplyDelta(Trust, 8); // 28
	TestFalse(TEXT("one_true_thing not before 30"), Zafir->IsUnlocked(TEXT("one_true_thing")));
	Zafir->ApplyDelta(Trust, 8); // 36
	TestTrue(TEXT("one_true_thing at >= 30"), Zafir->IsUnlocked(TEXT("one_true_thing")));
	TestFalse(TEXT("admits_hunt not yet"), Zafir->IsUnlocked(TEXT("admits_hunt")));

	Zafir->ApplyDelta(Trust, -8); // 28
	TestTrue(TEXT("stays unlocked after the value drops"), Zafir->IsUnlocked(TEXT("one_true_thing")));

	Zafir->ApplyDelta(Trust, 8);  // 36
	Zafir->ApplyDelta(Trust, 8);  // 44
	Zafir->ApplyDelta(Trust, 8);  // 52
	TestTrue(TEXT("admits_hunt at >= 50"), Zafir->IsUnlocked(TEXT("admits_hunt")));

	TestFalse(TEXT("jealousy_shows not yet"), Zafir->IsUnlocked(TEXT("jealousy_shows")));
	for (int32 i = 0; i < 4; ++i)
	{
		Zafir->ApplyDelta(Affection, 8); // 18, 26, 34, 42
	}
	TestTrue(TEXT("jealousy_shows at >= 40"), Zafir->IsUnlocked(TEXT("jealousy_shows")));

	TestFalse(TEXT("another character's row ignored (vashir caught)"), Zafir->IsUnlocked(TEXT("caught")));
	TestFalse(TEXT("another character's row ignored (vashir trust)"), Zafir->IsUnlocked(TEXT("other_character_row")));
	TestEqual(TEXT("three unlocks"), Zafir->GetUnlocked().Num(), 3);

	// A second init must not reset values
	const int32 TrustBefore = Zafir->GetValue(Trust);
	Zafir->InitFromCard(TEXT("zafir"), MakeZafir(), Rows);
	TestEqual(TEXT("second init keeps values"), Zafir->GetValue(Trust), TrustBefore);

	UNPCBrainComponent* Vashir = NewObject<UNPCBrainComponent>();
	Vashir->InitFromCard(TEXT("vashir"), MakeVashir(), Rows);
	const FName Suspicion(TEXT("suspicion"));
	for (int32 i = 0; i < 8; ++i)
	{
		Vashir->ApplyDelta(Suspicion, 10); // 20 .. 90
	}
	TestFalse(TEXT("caught not before 100"), Vashir->IsUnlocked(TEXT("caught")));
	Vashir->ApplyDelta(Suspicion, 10); // 100
	TestEqual(TEXT("suspicion 100"), Vashir->GetValue(Suspicion), 100);
	TestTrue(TEXT("caught at 100"), Vashir->IsUnlocked(TEXT("caught")));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAshesNPCBrainRouteAndWords, "Ashes.NPCBrain.RouteAndWords", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FAshesNPCBrainRouteAndWords::RunTest(const FString& Parameters)
{
	using namespace NPCBrainTestData;
	const FName Trust(TEXT("trust"));
	const FName Affection(TEXT("affection"));
	const TArray<FValueThresholdRow> Rows = { MakeThreshold(TEXT("zafir"), TEXT("trust"), 60, TEXT("romance_trust"), true) };

	UNPCBrainComponent* Brain = NewObject<UNPCBrainComponent>();
	Brain->InitFromCard(TEXT("zafir"), MakeZafir(), Rows);

	TestEqual(TEXT("wary at 20"), Brain->GetWord(Trust), FString(TEXT("wary")));
	Brain->ApplyDelta(Trust, 8);
	Brain->ApplyDelta(Trust, 8);
	TestEqual(TEXT("still wary at 36"), Brain->GetWord(Trust), FString(TEXT("wary")));
	Brain->ApplyDelta(Trust, 4); // 40
	TestEqual(TEXT("warming at 40"), Brain->GetWord(Trust), FString(TEXT("warming")));

	TestTrue(TEXT("comrade at affection 10"), Brain->GetRoute() == EAshesRoute::Comrade);
	for (int32 i = 0; i < 6; ++i)
	{
		Brain->ApplyDelta(Affection, 8); // 18 .. 58
	}
	TestEqual(TEXT("affection 58"), Brain->GetValue(Affection), 58);
	TestTrue(TEXT("still comrade below 60"), Brain->GetRoute() == EAshesRoute::Comrade);

	// Romance-only threshold is skipped on the comrade route
	for (int32 i = 0; i < 3; ++i)
	{
		Brain->ApplyDelta(Trust, 8); // 48, 56, 64
	}
	TestTrue(TEXT("trust >= 60"), Brain->GetValue(Trust) >= 60);
	TestFalse(TEXT("romance-only threshold locked on comrade route"), Brain->IsUnlocked(TEXT("romance_trust")));

	Brain->ApplyDelta(Affection, 8); // 66
	TestTrue(TEXT("romance at affection >= 60"), Brain->GetRoute() == EAshesRoute::Romance);
	Brain->ApplyDelta(Trust, 1);
	TestTrue(TEXT("romance-only threshold unlocks later on romance"), Brain->IsUnlocked(TEXT("romance_trust")));

	// LockRoute keeps the route after the value drops
	TestFalse(TEXT("route not locked yet"), Brain->IsRouteLocked());
	Brain->LockRoute();
	TestTrue(TEXT("route locked"), Brain->IsRouteLocked());
	for (int32 i = 0; i < 5; ++i)
	{
		Brain->ApplyDelta(Affection, -8);
	}
	TestTrue(TEXT("affection dropped below 60"), Brain->GetValue(Affection) < UNPCBrainComponent::RomanceAffection);
	TestTrue(TEXT("locked route stays romance"), Brain->GetRoute() == EAshesRoute::Romance);

	// Romance-only threshold also unlocks when the route is romance from the start of the climb
	UNPCBrainComponent* Romantic = NewObject<UNPCBrainComponent>();
	Romantic->InitFromCard(TEXT("zafir"), MakeZafir(), Rows);
	for (int32 i = 0; i < 7; ++i)
	{
		Romantic->ApplyDelta(Affection, 8); // 66
	}
	for (int32 i = 0; i < 6; ++i)
	{
		Romantic->ApplyDelta(Trust, 8); // 68
	}
	TestTrue(TEXT("romance-only threshold unlocks on romance"), Romantic->IsUnlocked(TEXT("romance_trust")));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
