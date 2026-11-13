// AI-assisted: Claude Code, 11.11.2026, checks and clamps every model reply before the game uses it (Phase 4); spoiler unlock chapters, strip * and _ (12.11.2026); small slips in beat and clue ids accepted (Phase 6 fix 5, 13.11.2026). Reviewed by <pending>.

#include "ResponseValidator.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

FAshesValidationContext FAshesValidationContext::FromCard(const FCharacterCardRow& Card, const FSceneCardRow& Scene)
{
	FAshesValidationContext Context;
	Context.AllowedEmotions = Card.Emotions;
	Context.FallbackEmotion = Card.FallbackEmotion;
	Context.Values = Card.Values;
	Context.SpoilerKeywords = Card.SpoilerKeywords;
	Context.Chapter = Scene.Chapter;
	Context.SafetyWords = UResponseValidator::DefaultSafetyWords();
	for (const FBeatDef& Beat : Scene.Beats)
	{
		Context.AllowedBeats.Add(Beat.Id);
	}
	return Context;
}

TArray<FString> UResponseValidator::DefaultSafetyWords()
{
	// MISSING (team): the real safety word list. Placeholder only.
	return { TEXT("kill yourself"), TEXT("nazi"), TEXT("rape") };
}

namespace
{
	/** Lower case, letters and digits only: "Sa_hra-Confronts Blade" -> "sahraconfrontsblade" */
	FString NormalizeId(const FString& Id)
	{
		FString Out;
		for (const TCHAR Char : Id.ToLower())
		{
			if (FChar::IsAlnum(Char))
			{
				Out.AppendChar(Char);
			}
		}
		return Out;
	}

	/** True if A and B differ by at most one inserted, removed or changed letter */
	bool WithinOneEdit(const FString& A, const FString& B)
	{
		const FString& Short = A.Len() <= B.Len() ? A : B;
		const FString& Long = A.Len() <= B.Len() ? B : A;
		if (Long.Len() - Short.Len() > 1)
		{
			return false;
		}
		// A digit is never the slip: stage_1 and stage_2 are different ids, not a typo
		auto IsDigitAt = [](const FString& S, int32 Index) { return S.IsValidIndex(Index) && FChar::IsDigit(S[Index]); };
		int32 i = 0;
		int32 j = 0;
		bool bEdited = false;
		while (i < Short.Len() && j < Long.Len())
		{
			if (Short[i] == Long[j])
			{
				++i;
				++j;
				continue;
			}
			if (bEdited || IsDigitAt(Long, j) || (Short.Len() == Long.Len() && IsDigitAt(Short, i)))
			{
				return false;
			}
			bEdited = true;
			if (Short.Len() == Long.Len())
			{
				++i; // changed letter
			}
			++j; // extra letter in the longer one
		}
		// The longer one may end with one extra character that the loop never looked at
		return !(j < Long.Len() && (bEdited || IsDigitAt(Long, j)));
	}
}

FName UResponseValidator::MatchId(const FString& RawId, const TArray<FName>& Allowed)
{
	const FString Trimmed = RawId.TrimStartAndEnd();
	if (Trimmed.IsEmpty())
	{
		return NAME_None;
	}
	for (const FName& Id : Allowed)
	{
		if (Id.ToString().Equals(Trimmed, ESearchCase::IgnoreCase))
		{
			return Id;
		}
	}

	// Small slips (Phase 6 playtest: "sa_hra_confronts_blade"): compare without case and punctuation,
	// then allow one wrong letter in long ids, but only if exactly one id fits
	const FString Wanted = NormalizeId(Trimmed);
	if (Wanted.IsEmpty())
	{
		return NAME_None;
	}
	for (const FName& Id : Allowed)
	{
		if (NormalizeId(Id.ToString()) == Wanted)
		{
			return Id;
		}
	}
	if (Wanted.Len() < MinLengthForSlip)
	{
		return NAME_None;
	}
	FName Found = NAME_None;
	for (const FName& Id : Allowed)
	{
		if (WithinOneEdit(NormalizeId(Id.ToString()), Wanted))
		{
			if (!Found.IsNone())
			{
				return NAME_None; // two ids fit: do not guess
			}
			Found = Id;
		}
	}
	return Found;
}

bool UResponseValidator::IsUnsafeText(const FString& Text, const TArray<FString>& SafetyWords)
{
	for (const FString& Word : SafetyWords)
	{
		if (!Word.IsEmpty() && Text.Contains(Word, ESearchCase::IgnoreCase))
		{
			return true;
		}
	}
	return false;
}

namespace
{
	/** Finds the outermost {...} so ```json fences or chatter around the object do not break parsing */
	FString ExtractJsonObject(const FString& Raw)
	{
		int32 Start = INDEX_NONE;
		int32 End = INDEX_NONE;
		Raw.FindChar(TEXT('{'), Start);
		Raw.FindLastChar(TEXT('}'), End);
		return (Start != INDEX_NONE && End > Start) ? Raw.Mid(Start, End - Start + 1) : FString();
	}

	FString CutToWords(const FString& Text, int32 MaxWords, bool& bWasCut)
	{
		TArray<FString> Words;
		Text.ParseIntoArrayWS(Words);
		bWasCut = Words.Num() > MaxWords;
		if (!bWasCut)
		{
			return Text;
		}
		Words.SetNum(MaxWords);
		FString Out = FString::Join(Words, TEXT(" "));
		Out.TrimEndInline();
		return Out + TEXT("...");
	}

	/** Removes * and _ emphasis markers (the model sometimes writes *you* or _you_) */
	FString StripEmphasis(const FString& Text)
	{
		return Text.Replace(TEXT("*"), TEXT("")).Replace(TEXT("_"), TEXT(""));
	}

	int32 ReadInt(const TSharedPtr<FJsonObject>& Obj, const FString& Key)
	{
		double Number = 0.0;
		if (Obj->TryGetNumberField(Key, Number))
		{
			return FMath::RoundToInt(Number);
		}
		FString AsString; // some models write "+5"
		if (Obj->TryGetStringField(Key, AsString))
		{
			return FCString::Atoi(*AsString.Replace(TEXT("+"), TEXT("")));
		}
		return 0;
	}
}

FAshesValidationResult UResponseValidator::Validate(const FString& RawJson, const FAshesValidationContext& Context)
{
	FAshesValidationResult Result;

	// 1) Parse (broken JSON -> one silent retry by the caller, then the fallback line)
	const FString JsonText = ExtractJsonObject(RawJson);
	TSharedPtr<FJsonObject> Obj;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
	if (JsonText.IsEmpty() || !FJsonSerializer::Deserialize(Reader, Obj) || !Obj.IsValid())
	{
		Result.Outcome = EValidationOutcome::RetryBrokenJson;
		Result.Notes.Add(TEXT("broken JSON"));
		return Result;
	}

	FLLMResponse& R = Result.Response;
	if (!Obj->TryGetStringField(TEXT("reply"), R.Reply) || R.Reply.TrimStartAndEnd().IsEmpty())
	{
		Result.Outcome = EValidationOutcome::RetryBrokenJson;
		Result.Notes.Add(TEXT("no reply text"));
		return Result;
	}
	// Markdown emphasis shows literally on screen ("*you*")
	R.Reply = StripEmphasis(R.Reply);
	R.Reply.TrimStartAndEndInline();

	// 2) Reply length: 1–4 sentences, at most 80 words
	bool bCut = false;
	R.Reply = CutToWords(R.Reply, Context.MaxWords, bCut);
	if (bCut)
	{
		Result.Notes.Add(FString::Printf(TEXT("reply cut to %d words"), Context.MaxWords));
	}

	// 3) Emotion whitelist with fallback (portrait must exist)
	FString Emotion;
	Obj->TryGetStringField(TEXT("emotion"), Emotion);
	const FName EmotionName(*Emotion.TrimStartAndEnd().ToLower());
	if (Context.AllowedEmotions.Contains(EmotionName))
	{
		R.Emotion = EmotionName;
	}
	else
	{
		R.Emotion = Context.FallbackEmotion;
		Result.Notes.Add(FString::Printf(TEXT("emotion '%s' -> %s"), *Emotion, *Context.FallbackEmotion.ToString()));
	}

	Obj->TryGetStringField(TEXT("gesture"), R.Gesture);
	R.Gesture = StripEmphasis(R.Gesture).TrimStartAndEnd();

	// 4) Clamp every relationship delta to the per-turn limits (an impossible +50 becomes +8)
	for (const FRelationshipValueDef& Def : Context.Values)
	{
		const int32 Raw = ReadInt(Obj, Def.Name.ToString() + TEXT("_delta"));
		const int32 Clamped = FMath::Clamp(Raw, -Def.MaxDecrease, Def.MaxIncrease);
		if (Clamped != Raw)
		{
			Result.Notes.Add(FString::Printf(TEXT("%s_delta %d clamped to %d"), *Def.Name.ToString(), Raw, Clamped));
		}
		R.Deltas.Add(Def.Name, Clamped);
	}

	// 5) Beats and clues: only ids of this scene (the model sometimes writes sentences here)
	auto ReadIds = [&Obj, &Result](const TCHAR* Key, const TArray<FName>& Allowed, TArray<FName>& Out)
	{
		const TArray<TSharedPtr<FJsonValue>>* Items = nullptr;
		if (!Obj->TryGetArrayField(Key, Items))
		{
			return;
		}
		for (const TSharedPtr<FJsonValue>& Item : *Items)
		{
			FString Id;
			if (Item.IsValid() && Item->TryGetString(Id))
			{
				const FName IdName = UResponseValidator::MatchId(Id, Allowed);
				if (!IdName.IsNone())
				{
					if (!IdName.ToString().Equals(Id.TrimStartAndEnd(), ESearchCase::IgnoreCase))
					{
						Result.Notes.Add(FString::Printf(TEXT("%s: read '%s' as '%s'"), Key, *Id.Left(40), *IdName.ToString()));
					}
					Out.AddUnique(IdName);
				}
				else
				{
					Result.Notes.Add(FString::Printf(TEXT("%s: dropped unknown '%s'"), Key, *Id.Left(40)));
				}
			}
		}
	};
	ReadIds(TEXT("beats"), Context.AllowedBeats, R.Beats);
	ReadIds(TEXT("clues"), Context.AllowedClues, R.Clues);

	// 6) Suggestions: up to 3 short strings
	const TArray<TSharedPtr<FJsonValue>>* Suggestions = nullptr;
	if (Obj->TryGetArrayField(TEXT("suggestions"), Suggestions))
	{
		for (const TSharedPtr<FJsonValue>& Item : *Suggestions)
		{
			FString Text;
			if (Item.IsValid() && Item->TryGetString(Text) && !Text.TrimStartAndEnd().IsEmpty() && R.Suggestions.Num() < Context.MaxSuggestions)
			{
				R.Suggestions.Add(Text.TrimStartAndEnd().Left(200));
			}
		}
	}

	Obj->TryGetBoolField(TEXT("off_topic"), R.bOffTopic);

	// 7) Spoilers and unsafe words -> regenerate once, then the fallback line
	for (const FSpoilerKeyword& Spoiler : Context.SpoilerKeywords)
	{
		const bool bLocked = Spoiler.UnlockChapter == 0 || Context.Chapter < Spoiler.UnlockChapter;
		if (bLocked && !Spoiler.Keyword.IsEmpty() && R.Reply.Contains(Spoiler.Keyword, ESearchCase::IgnoreCase))
		{
			Result.Outcome = EValidationOutcome::Regenerate;
			Result.Notes.Add(FString::Printf(TEXT("spoiler keyword '%s' (allowed from chapter %d)"), *Spoiler.Keyword, Spoiler.UnlockChapter));
			return Result;
		}
	}
	if (IsUnsafeText(R.Reply, Context.SafetyWords))
	{
		Result.Outcome = EValidationOutcome::Regenerate;
		Result.Notes.Add(TEXT("unsafe words in reply"));
		return Result;
	}

	Result.Outcome = EValidationOutcome::Ok;
	return Result;
}
