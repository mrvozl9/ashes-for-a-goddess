// AI-assisted: Claude Code, 10.11.2026, pre-written fallback replies in the same JSON format (Phase 3); lines from DT_FallbackLines (Phase 4, 12.11.2026); no repeat in a row. Reviewed by <pending>.

#include "OfflineLLMProvider.h"
#include "Engine/DataTable.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

namespace
{
	const TCHAR* GFallbackTablePath = TEXT("/Game/Ashes/Data/DT_FallbackLines.DT_FallbackLines");

	// Used only when DT_FallbackLines has nothing for the scene
	const TCHAR* GGenericLines[] =
	{
		TEXT("He studies you for a long moment, then looks away. \"Not here. Not now.\""),
		TEXT("\"Ask me again when the lanterns burn lower.\""),
		TEXT("A thin smile, nothing more. The question hangs in the smoke between you."),
	};
}

FString UOfflineLLMProvider::MakeFallbackJson(const FString& Line, FName Emotion)
{
	TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
	Root->SetStringField(TEXT("reply"), Line);
	Root->SetStringField(TEXT("emotion"), Emotion.IsNone() ? TEXT("neutral") : Emotion.ToString());
	Root->SetStringField(TEXT("gesture"), TEXT("hesitates"));
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

bool UOfflineLLMProvider::PickLine(const TArray<FFallbackLineRow>& Rows, FName Scene, FName Beat, FName Emotion, int32 Rotation, FFallbackLineRow& OutRow, const FString& AvoidLine)
{
	static const FName Any(TEXT("any"));

	TArray<const FFallbackLineRow*> SceneRows;
	for (const FFallbackLineRow& Row : Rows)
	{
		if (Row.Scene == Scene && !Row.Line.IsEmpty() && Row.Line != AvoidLine)
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
				if (Row.Scene == Scene && Row.Line == AvoidLine)
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

	// 1) the beat being pushed, 2) beat "any", 3) any line of the scene
	TArray<const FFallbackLineRow*> Candidates;
	if (!Beat.IsNone())
	{
		Candidates = Filter([Beat](const FFallbackLineRow& Row) { return Row.Beat == Beat; });
	}
	if (Candidates.Num() == 0)
	{
		Candidates = Filter([](const FFallbackLineRow& Row) { return Row.Beat == Any; });
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
		bFound = PickLine(Rows, Request.FallbackScene, Request.FallbackBeat, Request.FallbackEmotion, NextLine, Picked, LastLine);
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
