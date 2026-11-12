// AI-assisted: Claude Code, 10.11.2026, pre-written fallback replies in the same JSON format (Phase 3); lines from DT_FallbackLines (Phase 4, 12.11.2026); no repeat in a row; Hesitate / Content types; per character, [gestures]. Reviewed by <pending>.

#include "OfflineLLMProvider.h"
#include "Engine/DataTable.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

namespace
{
	const TCHAR* GFallbackTablePath = TEXT("/Game/Ashes/Data/DT_FallbackLines.DT_FallbackLines");

	// Used only when DT_FallbackLines has nothing for the character / scene (they fit any question, like Hesitate lines)
	const TCHAR* GGenericLines[] =
	{
		TEXT("[Looks away for a long moment.] Not here. Not now."),
		TEXT("Ask me again when the lanterns burn lower."),
		TEXT("[A thin smile, nothing more.]"),
	};

	const FName GAny(TEXT("any"));
}

void UOfflineLLMProvider::SplitGesture(const FString& Line, FString& OutSpeech, FString& OutGesture)
{
	// Text in [square brackets] is a gesture, the rest is speech
	TArray<FString> Gestures;
	FString Speech;
	int32 Index = 0;
	while (Index < Line.Len())
	{
		const int32 Open = Line.Find(TEXT("["), ESearchCase::CaseSensitive, ESearchDir::FromStart, Index);
		const int32 Close = Open == INDEX_NONE ? INDEX_NONE : Line.Find(TEXT("]"), ESearchCase::CaseSensitive, ESearchDir::FromStart, Open);
		if (Open == INDEX_NONE || Close == INDEX_NONE)
		{
			Speech += Line.Mid(Index);
			break;
		}
		Speech += Line.Mid(Index, Open - Index) + TEXT(" ");
		Gestures.Add(Line.Mid(Open + 1, Close - Open - 1).TrimStartAndEnd());
		Index = Close + 1;
	}

	// Collapse double spaces left by removed gestures
	while (Speech.ReplaceInline(TEXT("  "), TEXT(" ")) > 0) {}
	OutSpeech = Speech.TrimStartAndEnd();
	OutGesture = FString::Join(Gestures, TEXT(" "));
}

FString UOfflineLLMProvider::MakeFallbackJson(const FString& Line, FName Emotion)
{
	FString Speech;
	FString Gesture;
	SplitGesture(Line, Speech, Gesture);

	TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
	// A gesture-only line has no speech: "..." marks the silence; the gesture carries the moment
	Root->SetStringField(TEXT("reply"), Speech.IsEmpty() ? TEXT("...") : Speech);
	Root->SetStringField(TEXT("emotion"), Emotion.IsNone() ? TEXT("neutral") : Emotion.ToString());
	Root->SetStringField(TEXT("gesture"), Gesture);
	Root->SetNumberField(TEXT("trust_delta"), 0);
	Root->SetNumberField(TEXT("affection_delta"), 0);
	Root->SetArrayField(TEXT("beats"), {});
	Root->SetArrayField(TEXT("suggestions"), {});
	Root->SetBoolField(TEXT("off_topic"), false);

	FString Out;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Out);
	FJsonSerializer::Serialize(Root, Writer);
	return Out;
}

bool UOfflineLLMProvider::PickLine(const TArray<FFallbackLineRow>& Rows, FName Scene, FName Beat, FName Emotion, int32 Rotation, FFallbackLineRow& OutRow, const FString& AvoidLine, EFallbackLineType Type, FName Character)
{
	// Rows for this character (or for any character) and this scene (or "any" scene), of the wanted type
	auto Fits = [Scene, Character, Type](const FFallbackLineRow& Row)
	{
		const bool bCharacterOk = Row.Character.IsNone() || Row.Character == GAny || Character.IsNone() || Row.Character == Character;
		const bool bSceneOk = Row.Scene == Scene || Row.Scene == GAny;
		return bCharacterOk && bSceneOk && Row.Type == Type && !Row.Line.IsEmpty();
	};

	TArray<const FFallbackLineRow*> SceneRows;
	for (const FFallbackLineRow& Row : Rows)
	{
		if (Fits(Row) && Row.Line != AvoidLine)
		{
			SceneRows.Add(&Row);
		}
	}
	if (SceneRows.Num() == 0)
	{
		// Only the avoided line fits: repeating it beats the generic lines
		if (!AvoidLine.IsEmpty())
		{
			for (const FFallbackLineRow& Row : Rows)
			{
				if (Fits(Row) && Row.Line == AvoidLine)
				{
					OutRow = Row;
					return true;
				}
			}
		}
		return false;
	}

	auto Filter = [&SceneRows](TFunctionRef<bool(const FFallbackLineRow&)> Pred)
	{
		TArray<const FFallbackLineRow*> Out;
		for (const FFallbackLineRow* Row : SceneRows)
		{
			if (Pred(*Row))
			{
				Out.Add(Row);
			}
		}
		return Out;
	};

	// 1) the beat being pushed, 2) beat "any", 3) any line that fits
	TArray<const FFallbackLineRow*> Candidates;
	if (!Beat.IsNone())
	{
		Candidates = Filter([Beat](const FFallbackLineRow& Row) { return Row.Beat == Beat; });
	}
	if (Candidates.Num() == 0)
	{
		Candidates = Filter([](const FFallbackLineRow& Row) { return Row.Beat == GAny; });
	}
	if (Candidates.Num() == 0)
	{
		Candidates = SceneRows;
	}

	// Prefer the current emotion within the set
	TArray<const FFallbackLineRow*> SameEmotion;
	for (const FFallbackLineRow* Row : Candidates)
	{
		if (Row->Emotion == Emotion)
		{
			SameEmotion.Add(Row);
		}
	}
	const TArray<const FFallbackLineRow*>& Pool = SameEmotion.Num() > 0 ? SameEmotion : Candidates;
	OutRow = *Pool[FMath::Abs(Rotation) % Pool.Num()];
	return true;
}

void UOfflineLLMProvider::SendAsync(const FLLMRequest& Request, FOnLLMReply Callback)
{
	if (!bTriedLoad)
	{
		bTriedLoad = true;
		FallbackTable = LoadObject<UDataTable>(nullptr, GFallbackTablePath, nullptr, LOAD_Quiet | LOAD_NoWarn);
	}

	FLLMReply Reply;
	Reply.bSuccess = true;
	Reply.bFromFallback = true;

	FFallbackLineRow Picked;
	bool bFound = false;
	if (FallbackTable && !Request.FallbackScene.IsNone())
	{
		TArray<FFallbackLineRow*> RowPtrs;
		FallbackTable->GetAllRows<FFallbackLineRow>(TEXT("Fallback"), RowPtrs);
		TArray<FFallbackLineRow> Rows;
		for (const FFallbackLineRow* Row : RowPtrs)
		{
			Rows.Add(*Row);
		}
		// Online failure -> a Hesitate (evasive) line that fits any question; offline -> a Content line for the scene / beat
		const EFallbackLineType Type = Request.bHesitate ? EFallbackLineType::Hesitate : EFallbackLineType::Content;
		bFound = PickLine(Rows, Request.FallbackScene, Request.bHesitate ? NAME_None : Request.FallbackBeat, Request.FallbackEmotion,
			NextLine, Picked, LastLine, Type, Request.FallbackCharacter);
		if (bFound)
		{
			LastLine = Picked.Line;
		}
	}

	Reply.Text = bFound
		? MakeFallbackJson(Picked.Line, Picked.Emotion)
		: MakeFallbackJson(GGenericLines[NextLine % UE_ARRAY_COUNT(GGenericLines)]);
	++NextLine;
	Callback.ExecuteIfBound(Reply);
}
