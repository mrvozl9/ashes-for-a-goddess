// AI-assisted: Claude Code, 13.11.2026, pure Scene Director state machine (turns, beats, nudge, close). Reviewed by <pending>.

#include "SceneFlow.h"

const FName FSceneFlow::CaughtValueName = FName(TEXT("suspicion"));

void FSceneFlow::Start(FName InSceneRow, const FSceneCardRow& InScene, const TArray<FSceneRuleRow>& AllRules)
{
	SceneRow = InSceneRow;
	Scene = InScene;
	Rules.Reset();
	for (const FSceneRuleRow& Rule : AllRules)
	{
		if (Rule.Scene == InSceneRow)
		{
			Rules.Add(Rule);
		}
	}
	Reached.Reset();
	ForcedOnClose.Reset();
	Deferred.Reset();
	TurnsTaken = 0;
	StalledTurns = 0;
	bFull = false;
	Phase = EScenePhase::Intro;
}

const FBeatDef* FSceneFlow::FindBeat(FName Beat) const
{
	if (Beat.IsNone())
	{
		return nullptr;
	}
	return Scene.Beats.FindByPredicate([Beat](const FBeatDef& B) { return B.Id == Beat; });
}

const FSceneRuleRow* FSceneFlow::FindRule(FName Beat) const
{
	return Rules.FindByPredicate([Beat](const FSceneRuleRow& R) { return R.Beat == Beat; });
}

bool FSceneFlow::HasRequired() const
{
	for (const FBeatDef& B : Scene.Beats)
	{
		if (B.bRequired)
		{
			return true;
		}
	}
	return false;
}

bool FSceneFlow::AllRequiredReached() const
{
	for (const FBeatDef& B : Scene.Beats)
	{
		if (B.bRequired && !Reached.Contains(B.Id))
		{
			return false;
		}
	}
	return true;
}

void FSceneFlow::BeginTurn()
{
	if (Phase == EScenePhase::Idle || IsOver())
	{
		return;
	}
	++TurnsTaken;
	if (Phase == EScenePhase::Intro)
	{
		Phase = EScenePhase::FreeTalk;
	}
}

void FSceneFlow::RefreshOpenPhase()
{
	if (Phase != EScenePhase::FreeTalk && Phase != EScenePhase::Nudge)
	{
		return;
	}
	Phase = (GetTurnsLeft() <= NudgeTurnsLeft || StalledTurns >= StallLimit) ? EScenePhase::Nudge : EScenePhase::FreeTalk;
}

void FSceneFlow::RefundTurn()
{
	if (Phase == EScenePhase::Idle || IsOver())
	{
		return;
	}
	TurnsTaken = FMath::Max(0, TurnsTaken - 1);
	RefreshOpenPhase();
}

bool FSceneFlow::CanAcceptBeat(FName Beat, const TMap<FName, int32>& Values) const
{
	if (!FindBeat(Beat) || Reached.Contains(Beat))
	{
		return false;
	}
	const FSceneRuleRow* Rule = FindRule(Beat);
	if (!Rule)
	{
		return true;
	}
	if (Rule->MinTurn > 0 && TurnsTaken < Rule->MinTurn)
	{
		return false;
	}
	if (!Rule->RequiresBeat.IsNone() && !Reached.Contains(Rule->RequiresBeat))
	{
		return false;
	}
	if (!Rule->RequiresValue.IsNone())
	{
		const int32* Value = Values.Find(Rule->RequiresValue);
		if ((Value ? *Value : 0) < Rule->RequiresMin)
		{
			return false;
		}
	}
	return true;
}

bool FSceneFlow::IsOnlyTooEarly(FName Beat, const TMap<FName, int32>& Values) const
{
	if (!FindBeat(Beat) || Reached.Contains(Beat))
	{
		return false;
	}
	// A failed value rule is a real "no" (the stand-in happened); only turn and order rules can be waited out
	const FSceneRuleRow* Rule = FindRule(Beat);
	if (Rule && !Rule->RequiresValue.IsNone())
	{
		const int32* Value = Values.Find(Rule->RequiresValue);
		if ((Value ? *Value : 0) < Rule->RequiresMin)
		{
			return false;
		}
	}
	return true;
}

void FSceneFlow::CloseAtLimit()
{
	if (!HasRequired())
	{
		Phase = EScenePhase::Resolve;
		bFull = true;
		return;
	}
	Phase = EScenePhase::ForcedClose;
	bFull = false;
	for (const FBeatDef& B : Scene.Beats)
	{
		if (B.bRequired && !Reached.Contains(B.Id))
		{
			ForcedOnClose.Add(B.Id);
			Reached.Add(B.Id);
		}
	}
}

TArray<FName> FSceneFlow::ApplyReply(const TArray<FName>& ClaimedBeats, const TMap<FName, int32>& Values, bool bTrusted)
{
	TArray<FName> NewBeats;
	if (Phase == EScenePhase::Idle || IsOver())
	{
		return NewBeats;
	}

	// 1. Claimed beats, in order
	for (const FName& Beat : ClaimedBeats)
	{
		const bool bOk = bTrusted ? (FindBeat(Beat) != nullptr && !Reached.Contains(Beat)) : CanAcceptBeat(Beat, Values);
		if (bOk)
		{
			Reached.Add(Beat);
			NewBeats.Add(Beat);
		}
		else if (!bTrusted && IsOnlyTooEarly(Beat, Values))
		{
			// The moment happened in the talk before its rule allowed it: remember it instead of losing it
			Deferred.Add(Beat);
		}
	}

	// 2. Forced beats
	for (const FSceneRuleRow& Rule : Rules)
	{
		if (Rule.ForceAtTurn > 0 && TurnsTaken >= Rule.ForceAtTurn && FindBeat(Rule.Beat) && !Reached.Contains(Rule.Beat))
		{
			Reached.Add(Rule.Beat);
			NewBeats.Add(Rule.Beat);
		}
	}

	// 2b. Earlier claims whose turn / order rule is met now (card order; repeat so one can unlock the next)
	for (bool bChanged = true; bChanged; )
	{
		bChanged = false;
		for (const FBeatDef& B : Scene.Beats)
		{
			if (Deferred.Contains(B.Id) && (Reached.Contains(B.Id) || CanAcceptBeat(B.Id, Values)))
			{
				Deferred.Remove(B.Id);
				if (!Reached.Contains(B.Id))
				{
					Reached.Add(B.Id);
					NewBeats.Add(B.Id);
				}
				bChanged = true;
			}
		}
	}

	// 3. Stall counter
	StalledTurns = NewBeats.Num() > 0 ? 0 : StalledTurns + 1;

	// 4. Phase
	const int32* Suspicion = Values.Find(CaughtValueName);
	if (Suspicion && *Suspicion >= CaughtAt)
	{
		Phase = EScenePhase::Caught;
		bFull = false;
	}
	else if (HasRequired() && AllRequiredReached())
	{
		Phase = EScenePhase::Resolve;
		bFull = true;
	}
	else if (TurnsTaken >= Scene.TurnLimit)
	{
		CloseAtLimit();
	}
	else
	{
		Phase = EScenePhase::FreeTalk;
		RefreshOpenPhase();
	}
	return NewBeats;
}

void FSceneFlow::ForceBeat(FName Beat)
{
	if (FindBeat(Beat))
	{
		Reached.Add(Beat);
	}
}

void FSceneFlow::EndEarly()
{
	if (Phase == EScenePhase::Idle || IsOver())
	{
		return;
	}
	if (!HasRequired() || AllRequiredReached())
	{
		Phase = EScenePhase::Resolve;
		bFull = true;
		return;
	}
	CloseAtLimit();
}

FName FSceneFlow::GetPendingBeat() const
{
	FName FirstUnreached = NAME_None;
	for (const FBeatDef& B : Scene.Beats)
	{
		if (!B.bRequired || Reached.Contains(B.Id))
		{
			continue;
		}
		if (FirstUnreached.IsNone())
		{
			FirstUnreached = B.Id;
		}
		const FSceneRuleRow* Rule = FindRule(B.Id);
		if (!Rule || Rule->RequiresBeat.IsNone() || Reached.Contains(Rule->RequiresBeat))
		{
			return B.Id;
		}
	}
	return FirstUnreached;
}

FString FSceneFlow::GetNudgeNote() const
{
	if (Phase != EScenePhase::Nudge)
	{
		return FString();
	}
	const FName Pending = GetPendingBeat();
	if (const FSceneRuleRow* Rule = FindRule(Pending); Rule && !Pending.IsNone() && !Rule->NudgeNote.IsEmpty())
	{
		return Rule->NudgeNote;
	}
	if (const FSceneRuleRow* SceneRule = FindRule(NAME_None); SceneRule && !SceneRule->NudgeNote.IsEmpty())
	{
		return SceneRule->NudgeNote;
	}
	if (const FBeatDef* Def = FindBeat(Pending))
	{
		return FString::Printf(TEXT("Steer the conversation toward this story moment now: %s"), *Def->Rule);
	}
	return FString();
}

FString FSceneFlow::GetClosingRomance() const
{
	const FSceneRuleRow* SceneRule = FindRule(NAME_None);
	return SceneRule ? SceneRule->ClosingRomance : FString();
}
