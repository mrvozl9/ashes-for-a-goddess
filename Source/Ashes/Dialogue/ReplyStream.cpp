// AI-assisted: Claude Code, 13.11.2026, word-by-word reply: reads the "reply" field out of the model's JSON while it is still being written (Phase 7). Reviewed by <pending>.

#include "ReplyStream.h"
#include "ResponseValidator.h"

FString AshesReplyStream::ExtractReply(const FString& PartialJson)
{
	// Find "reply" used as a key: the quoted word followed by a colon
	int32 Search = 0;
	int32 ValueStart = INDEX_NONE;
	while (ValueStart == INDEX_NONE)
	{
		const int32 Key = PartialJson.Find(TEXT("\"reply\""), ESearchCase::IgnoreCase, ESearchDir::FromStart, Search);
		if (Key == INDEX_NONE)
		{
			return FString();
		}
		int32 i = Key + 7;
		while (i < PartialJson.Len() && FChar::IsWhitespace(PartialJson[i])) { ++i; }
		if (i >= PartialJson.Len())
		{
			return FString(); // the colon has not arrived yet
		}
		if (PartialJson[i] != TEXT(':'))
		{
			Search = Key + 1; // "reply" inside some text, not the key
			continue;
		}
		++i;
		while (i < PartialJson.Len() && FChar::IsWhitespace(PartialJson[i])) { ++i; }
		if (i >= PartialJson.Len() || PartialJson[i] != TEXT('"'))
		{
			return FString(); // the opening quote has not arrived yet (or the value is not a string)
		}
		ValueStart = i + 1;
	}

	FString Out;
	for (int32 i = ValueStart; i < PartialJson.Len(); ++i)
	{
		const TCHAR Char = PartialJson[i];
		if (Char == TEXT('"'))
		{
			break; // the reply is complete
		}
		if (Char != TEXT('\\'))
		{
			Out.AppendChar(Char);
			continue;
		}
		if (i + 1 >= PartialJson.Len())
		{
			break; // half an escape at the end: wait for the rest
		}
		const TCHAR Next = PartialJson[++i];
		switch (Next)
		{
		case TEXT('n'): Out.AppendChar(TEXT('\n')); break;
		case TEXT('t'): Out.AppendChar(TEXT(' ')); break;
		case TEXT('r'): break;
		case TEXT('u'):
			if (i + 4 >= PartialJson.Len())
			{
				i = PartialJson.Len(); // the 4 hex digits are not all here yet
				break;
			}
			Out.AppendChar(static_cast<TCHAR>(FParse::HexNumber(*PartialJson.Mid(i + 1, 4))));
			i += 4;
			break;
		default: Out.AppendChar(Next); break; // \" \\ \/
		}
	}

	// Same markup removal as the validator (it strips * and _ from finished replies)
	Out.ReplaceInline(TEXT("*"), TEXT(""));
	Out.ReplaceInline(TEXT("_"), TEXT(""));
	return Out.TrimStart();
}

bool AshesReplyStream::IsBlocked(const FString& ReplySoFar, const FAshesValidationContext& Context)
{
	for (const FSpoilerKeyword& Spoiler : Context.SpoilerKeywords)
	{
		const bool bLocked = Spoiler.UnlockChapter == 0 || Context.Chapter < Spoiler.UnlockChapter;
		if (bLocked && !Spoiler.Keyword.IsEmpty() && ReplySoFar.Contains(Spoiler.Keyword, ESearchCase::IgnoreCase))
		{
			return true;
		}
	}
	return UResponseValidator::IsUnsafeText(ReplySoFar, Context.SafetyWords);
}

FString AshesReplyStream::SafeToShow(const FString& ReplySoFar, const FAshesValidationContext& Context)
{
	// 1) Whole words only: a word still being written could turn into anything
	int32 LastSpace = INDEX_NONE;
	for (int32 i = ReplySoFar.Len() - 1; i >= 0; --i)
	{
		if (FChar::IsWhitespace(ReplySoFar[i]))
		{
			LastSpace = i;
			break;
		}
	}
	if (LastSpace == INDEX_NONE)
	{
		return FString();
	}
	FString Text = ReplySoFar.Left(LastSpace).TrimEnd();

	// 2) Hold back an end that could be the beginning of a locked phrase (multi-word spoilers, safety phrases)
	TArray<FString> Locked;
	for (const FSpoilerKeyword& Spoiler : Context.SpoilerKeywords)
	{
		if ((Spoiler.UnlockChapter == 0 || Context.Chapter < Spoiler.UnlockChapter) && !Spoiler.Keyword.TrimStartAndEnd().IsEmpty())
		{
			Locked.Add(Spoiler.Keyword.TrimStartAndEnd());
		}
	}
	for (const FString& Word : Context.SafetyWords)
	{
		if (!Word.TrimStartAndEnd().IsEmpty())
		{
			Locked.Add(Word.TrimStartAndEnd());
		}
	}

	bool bChanged = true;
	while (bChanged && !Text.IsEmpty())
	{
		bChanged = false;
		for (const FString& Phrase : Locked)
		{
			// The longest beginning of Phrase that the text ends with, starting at a word boundary
			for (int32 Len = FMath::Min(Phrase.Len(), Text.Len()); Len >= 1; --Len)
			{
				const int32 Start = Text.Len() - Len;
				const bool bAtWordStart = Start == 0 || !FChar::IsAlnum(Text[Start - 1]);
				if (bAtWordStart && Text.Right(Len).Equals(Phrase.Left(Len), ESearchCase::IgnoreCase))
				{
					Text = Text.Left(Start).TrimEnd();
					bChanged = true;
					break;
				}
			}
			if (bChanged)
			{
				break;
			}
		}
	}
	return Text;
}
