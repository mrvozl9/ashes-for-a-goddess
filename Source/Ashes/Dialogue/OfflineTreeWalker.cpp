// AI-assisted: Claude Code, 13.11.2026, offline dialogue tree walker (pure logic, no world/UI/LLM). Reviewed by <pending>.

#include "OfflineTreeWalker.h"
#include "AshesDataValidator.h"

namespace
{
	/** Trimmed, lower case, trailing punctuation removed */
	FString NormalizeText(const FString& In)
	{
		FString Out = In.TrimStartAndEnd().ToLower();
		while (Out.Len() > 0 && !FChar::IsAlnum(Out[Out.Len() - 1]))
		{
			Out.LeftChopInline(1);
		}
		return Out.TrimStartAndEnd();
	}

	/** Lower-case words (letters/digits only) with at least MinLen characters */
	TSet<FString> Words(const FString& In, int32 MinLen)
	{
		TSet<FString> Out;
		const FString Lower = In.ToLower();
		FString Cur;
		for (int32 i = 0; i <= Lower.Len(); ++i)
		{
			if (i < Lower.Len() && FChar::IsAlnum(Lower[i]))
			{
				Cur.AppendChar(Lower[i]);
			}
			else
			{
				if (Cur.Len() >= MinLen)
				{
					Out.Add(Cur);
				}
				Cur.Reset();
			}
		}
		return Out;
	}

	void ParseChangesSafe(const FString& Text, TMap<FName, int32>& Out)
	{
		Out.Reset();
		if (Text.TrimStartAndEnd().IsEmpty())
		{
			return;
		}
		if (!AshesData::ParseChanges(Text, Out, nullptr))
		{
			Out.Reset();
		}
	}
}

TArray<const FOfflineNodeRow*> FOfflineTreeWalker::RowsAt(int32 NodeId) const
{
	TArray<const FOfflineNodeRow*> Out;
	for (const FOfflineNodeRow& Row : Rows)
	{
		if (Row.Node == NodeId && Out.Num() < MaxReplies)
		{
			Out.Add(&Row);
		}
	}
	return Out;
}

bool FOfflineTreeWalker::Start(const TArray<FOfflineNodeRow>& AllRows, FName Scene)
{
	Reset();
	int32 Lowest = MAX_int32;
	bool bHasFirst = false;
	for (const FOfflineNodeRow& Row : AllRows)
	{
		if (Row.Scene == Scene)
		{
			Rows.Add(Row);
			Lowest = FMath::Min(Lowest, Row.Node);
			bHasFirst |= (Row.Node == 1);
		}
	}
	if (Rows.Num() == 0)
	{
		return false;
	}
	Node = bHasFirst ? 1 : Lowest;
	bActive = true;
	return true;
}

void FOfflineTreeWalker::Reset()
{
	Rows.Reset();
	Node = 0;
	bActive = false;
}

TArray<FString> FOfflineTreeWalker::GetReplies() const
{
	TArray<FString> Out;
	for (const FOfflineNodeRow* Row : RowsAt(Node))
	{
		Out.Add(Row->PlayerLine);
	}
	return Out;
}

int32 FOfflineTreeWalker::MatchReply(const FString& PlayerText) const
{
	if (!bActive)
	{
		return -1;
	}
	const TArray<FString> Replies = GetReplies();

	const FString Wanted = NormalizeText(PlayerText);
	if (Wanted.IsEmpty())
	{
		return -1;
	}
	for (int32 i = 0; i < Replies.Num(); ++i)
	{
		if (NormalizeText(Replies[i]) == Wanted)
		{
			return i;
		}
	}

	const TSet<FString> PlayerWords = Words(PlayerText, 1);
	int32 Best = -1;
	int32 BestScore = 0;
	bool bTie = false;
	for (int32 i = 0; i < Replies.Num(); ++i)
	{
		int32 Score = 0;
		for (const FString& Keyword : Words(Replies[i], 4))
		{
			if (PlayerWords.Contains(Keyword))
			{
				++Score;
			}
		}
		if (Score > BestScore)
		{
			BestScore = Score;
			Best = i;
			bTie = false;
		}
		else if (Score == BestScore && Score > 0)
		{
			bTie = true;
		}
	}
	return (BestScore >= 1 && !bTie) ? Best : -1;
}

FOfflineStep FOfflineTreeWalker::Pick(int32 Index, const TMap<FName, int32>& Values, const TSet<FName>& KnownClues)
{
	FOfflineStep Step;
	if (!bActive)
	{
		return Step;
	}
	const TArray<const FOfflineNodeRow*> NodeRows = RowsAt(Node);
	if (!NodeRows.IsValidIndex(Index))
	{
		return Step;
	}
	const FOfflineNodeRow& Row = *NodeRows[Index];

	bool bPassed = true;
	if (!Row.RequiresValue.IsNone())
	{
		const int32* Found = Values.Find(Row.RequiresValue);
		bPassed &= ((Found ? *Found : 0) >= Row.RequiresMin);
	}
	if (!Row.RequiresClue.IsNone())
	{
		bPassed &= KnownClues.Contains(Row.RequiresClue);
	}
	// Nothing authored to stand in: treat the rule as passed
	if (!bPassed && Row.StandInLine.TrimStartAndEnd().IsEmpty())
	{
		bPassed = true;
	}

	Step.bValid = true;
	int32 Next = 0;
	if (bPassed)
	{
		Step.Line = Row.CharacterLine;
		Step.Emotion = Row.Emotion;
		ParseChangesSafe(Row.Changes, Step.Changes);
		Step.Beats = AshesData::SplitIds(Row.Beats);
		Step.Clues = AshesData::SplitIds(Row.Clues);
		Next = Row.NextNode;
	}
	else
	{
		Step.bStandIn = true;
		Step.Line = Row.StandInLine;
		Step.Emotion = Row.StandInEmotion.IsNone() ? Row.Emotion : Row.StandInEmotion;
		ParseChangesSafe(Row.StandInChanges, Step.Changes);
		Step.Beats = AshesData::SplitIds(Row.StandInBeats);
		Next = Row.StandInNextNode;
	}

	if (Next <= 0 || RowsAt(Next).Num() == 0)
	{
		Step.bEnded = true;
		bActive = false;
	}
	else
	{
		Node = Next;
		Step.NextReplies = GetReplies();
	}
	return Step;
}
