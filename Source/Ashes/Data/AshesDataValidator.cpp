// AI-assisted: Claude Code, 12.11.2026, data report and checks for the CSV-imported DataTables (duplicate ids, missing references, empty required fields). Reviewed by Baha.

#include "AshesDataValidator.h"
#include "Engine/DataTable.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Serialization/Csv/CsvParser.h"

// ---------------------------------------------------------------------------------------------
// Report
// ---------------------------------------------------------------------------------------------

void FAshesDataReport::Add(EAshesDataSeverity Severity, const FString& Table, const FString& Row, const FString& Message)
{
	FAshesDataIssue& Issue = Issues.AddDefaulted_GetRef();
	Issue.Severity = Severity;
	Issue.Table = Table;
	Issue.Row = Row;
	Issue.Message = Message;
}

int32 FAshesDataReport::Num(EAshesDataSeverity Severity) const
{
	int32 Count = 0;
	for (const FAshesDataIssue& Issue : Issues)
	{
		Count += (Issue.Severity == Severity) ? 1 : 0;
	}
	return Count;
}

bool FAshesDataReport::Has(EAshesDataSeverity Severity, const FString& Table, const FString& Row, const FString& MessagePart) const
{
	for (const FAshesDataIssue& Issue : Issues)
	{
		if (Issue.Severity == Severity && Issue.Table == Table && Issue.Row == Row && (MessagePart.IsEmpty() || Issue.Message.Contains(MessagePart)))
		{
			return true;
		}
	}
	return false;
}

FString FAshesDataReport::ToText() const
{
	static const TCHAR* Labels[] = { TEXT("ERROR  "), TEXT("WARNING"), TEXT("INFO   ") };

	FString Out = FString::Printf(TEXT("Ashes data report: %d error(s), %d warning(s)\n"), NumErrors(), NumWarnings());
	for (const EAshesDataSeverity Severity : { EAshesDataSeverity::Error, EAshesDataSeverity::Warning, EAshesDataSeverity::Info })
	{
		for (const FAshesDataIssue& Issue : Issues)
		{
			if (Issue.Severity != Severity)
			{
				continue;
			}
			Out += FString::Printf(TEXT("%s  %s"), Labels[static_cast<int32>(Severity)], *Issue.Table);
			if (!Issue.Row.IsEmpty())
			{
				Out += FString::Printf(TEXT(" [%s]"), *Issue.Row);
			}
			Out += FString::Printf(TEXT(": %s\n"), *Issue.Message);
		}
	}
	if (RowCounts.Num() > 0)
	{
		Out += TEXT("Rows:\n");
		for (const TPair<FString, int32>& Count : RowCounts)
		{
			Out += FString::Printf(TEXT("  %s: %d\n"), *Count.Key, Count.Value);
		}
	}
	return Out;
}

// ---------------------------------------------------------------------------------------------
// Tables and small parsers
// ---------------------------------------------------------------------------------------------

namespace AshesData
{
	const TCHAR* GetAssetFolder()
	{
		return TEXT("/Game/Ashes/Data");
	}

	FString GetCsvFolder()
	{
		return FPaths::Combine(FPaths::ProjectContentDir(), TEXT("Ashes"), TEXT("Data"), TEXT("CSV"));
	}

	TConstArrayView<FAshesTableDef> GetTableDefs()
	{
		static const FAshesTableDef Defs[] =
		{
			{ TEXT("DT_Characters"),    &FCharacterCardRow::StaticStruct, &FAshesDataTables::Characters },
			{ TEXT("DT_Scenes"),        &FSceneCardRow::StaticStruct,     &FAshesDataTables::Scenes },
			{ TEXT("DT_Clues"),         &FClueRow::StaticStruct,          &FAshesDataTables::Clues },
			{ TEXT("DT_FallbackLines"), &FFallbackLineRow::StaticStruct,  &FAshesDataTables::FallbackLines },
			{ TEXT("DT_OfflineTrees"),  &FOfflineNodeRow::StaticStruct,   &FAshesDataTables::OfflineTrees },
			{ TEXT("DT_Barks"),         &FBarkRow::StaticStruct,          &FAshesDataTables::Barks },
			{ TEXT("DT_DanceCharts"),   &FDanceChartRow::StaticStruct,    &FAshesDataTables::DanceCharts },
			{ TEXT("DT_Cutscenes"),     &FCutsceneLineRow::StaticStruct,  &FAshesDataTables::Cutscenes },
		};
		return Defs;
	}

	FAshesDataTables LoadTables()
	{
		FAshesDataTables Tables;
		for (const FAshesTableDef& Def : GetTableDefs())
		{
			const FString PackageName = FString::Printf(TEXT("%s/%s"), GetAssetFolder(), Def.Name);
			if (FPackageName::DoesPackageExist(PackageName))
			{
				Tables.*(Def.Slot) = LoadObject<UDataTable>(nullptr, *FString::Printf(TEXT("%s.%s"), *PackageName, Def.Name));
			}
		}
		return Tables;
	}

	bool IsAny(FName Name)
	{
		return Name.IsNone() || Name.ToString().Equals(TEXT("any"), ESearchCase::IgnoreCase);
	}

	TArray<FName> SplitIds(const FString& Text)
	{
		TArray<FString> Parts;
		Text.ParseIntoArray(Parts, TEXT(";"), true);

		TArray<FName> Ids;
		for (FString& Part : Parts)
		{
			Part.TrimStartAndEndInline();
			if (!Part.IsEmpty())
			{
				Ids.Add(FName(*Part));
			}
		}
		return Ids;
	}

	/** Splits "left:right" and trims both sides; false when there is not exactly one usable pair */
	static bool SplitPair(const FString& Part, FString& OutLeft, FString& OutRight)
	{
		if (!Part.Split(TEXT(":"), &OutLeft, &OutRight))
		{
			return false;
		}
		OutLeft.TrimStartAndEndInline();
		OutRight.TrimStartAndEndInline();
		return !OutLeft.IsEmpty() && !OutRight.IsEmpty();
	}

	bool ParseChanges(const FString& Text, TMap<FName, int32>& OutChanges, FString* OutError)
	{
		OutChanges.Reset();

		TArray<FString> Parts;
		Text.ParseIntoArray(Parts, TEXT(";"), true);
		for (FString& Part : Parts)
		{
			Part.TrimStartAndEndInline();
			if (Part.IsEmpty() || Part == TEXT("--") || Part == TEXT("-"))
			{
				continue; // the lore tables write "--" for "no change"
			}

			FString Name, Amount;
			if (!SplitPair(Part, Name, Amount))
			{
				if (OutError) { *OutError = FString::Printf(TEXT("\"%s\" is not name:amount"), *Part); }
				return false;
			}

			Amount.RemoveFromStart(TEXT("+"));
			const bool bNegative = Amount.RemoveFromStart(TEXT("-"));
			if (Amount.IsEmpty() || !Amount.IsNumeric() || Amount.Contains(TEXT(".")))
			{
				if (OutError) { *OutError = FString::Printf(TEXT("\"%s\": the amount must be a whole number"), *Part); }
				return false;
			}

			const FName Key(*Name.ToLower());
			if (OutChanges.Contains(Key))
			{
				if (OutError) { *OutError = FString::Printf(TEXT("\"%s\" is listed twice"), *Name); }
				return false;
			}
			OutChanges.Add(Key, FCString::Atoi(*Amount) * (bNegative ? -1 : 1));
		}
		return true;
	}

	bool ParseNotes(const FString& Text, TArray<TPair<float, int32>>& OutNotes, FString* OutError)
	{
		OutNotes.Reset();

		TArray<FString> Parts;
		Text.ParseIntoArray(Parts, TEXT(";"), true);
		for (FString& Part : Parts)
		{
			Part.TrimStartAndEndInline();
			if (Part.IsEmpty())
			{
				continue;
			}

			FString Beat, Lane;
			if (!SplitPair(Part, Beat, Lane) || !Beat.IsNumeric() || !Lane.IsNumeric() || Lane.Contains(TEXT(".")))
			{
				if (OutError) { *OutError = FString::Printf(TEXT("\"%s\" is not beat:lane"), *Part); }
				return false;
			}
			OutNotes.Emplace(FCString::Atof(*Beat), FCString::Atoi(*Lane));
		}
		return true;
	}

	int32 CheckCsvIds(const FString& Csv, const FString& TableName, FAshesDataReport& Report)
	{
		const FCsvParser Parser(Csv);
		const FCsvParser::FRows& Rows = Parser.GetRows();

		int32 DataRows = 0;
		TMap<FString, int32> FirstLineOfId;
		for (int32 RowIndex = 1; RowIndex < Rows.Num(); ++RowIndex) // row 0 is the header
		{
			const TArray<const TCHAR*>& Cells = Rows[RowIndex];

			bool bBlank = true;
			for (const TCHAR* Cell : Cells)
			{
				if (!FString(Cell).TrimStartAndEnd().IsEmpty())
				{
					bBlank = false;
					break;
				}
			}
			if (bBlank)
			{
				continue;
			}

			++DataRows;
			const FString Id = Cells.Num() > 0 ? FString(Cells[0]).TrimStartAndEnd() : FString();
			const FString Where = FString::Printf(TEXT("CSV row %d"), RowIndex + 1);
			if (Id.IsEmpty())
			{
				Report.Error(TableName, Where, TEXT("the id (first column) is empty"));
				continue;
			}

			// FName ignores case, so "Zafir" and "zafir" would be the same row
			const FString Key = Id.ToLower();
			if (const int32* FirstLine = FirstLineOfId.Find(Key))
			{
				Report.Error(TableName, Id, FString::Printf(TEXT("duplicate id: CSV rows %d and %d"), *FirstLine, RowIndex + 1));
			}
			else
			{
				FirstLineOfId.Add(Key, RowIndex + 1);
			}
		}
		return DataRows;
	}
}

// ---------------------------------------------------------------------------------------------
// Checks
// ---------------------------------------------------------------------------------------------

namespace
{
	/** Dance lanes (4 directions, GDD dance minigame) */
	constexpr int32 GDanceLaneCount = 4;

	/** Longest reply the validator lets through (same limit as UResponseValidator: GDD 8.1) */
	constexpr int32 GMaxLineWords = 80;

	template <typename RowType>
	const RowType* FindRow(const UDataTable* Table, FName RowName)
	{
		if (!Table || RowName.IsNone())
		{
			return nullptr;
		}
		uint8* const* Found = Table->GetRowMap().Find(RowName);
		return Found ? reinterpret_cast<const RowType*>(*Found) : nullptr;
	}

	template <typename RowType, typename FuncType>
	void ForEachRow(const UDataTable* Table, FuncType&& Func)
	{
		for (const TPair<FName, uint8*>& Pair : Table->GetRowMap())
		{
			Func(Pair.Key.ToString(), *reinterpret_cast<const RowType*>(Pair.Value));
		}
	}

	bool IsBlank(const FString& Text)
	{
		return Text.TrimStartAndEnd().IsEmpty();
	}

	int32 CountWords(const FString& Text)
	{
		TArray<FString> Words;
		Text.ParseIntoArrayWS(Words);
		return Words.Num();
	}

	bool HasBeat(const FSceneCardRow& Scene, FName BeatId)
	{
		return Scene.Beats.ContainsByPredicate([BeatId](const FBeatDef& Beat) { return Beat.Id == BeatId; });
	}

	bool HasValue(const FCharacterCardRow& Card, FName ValueName)
	{
		return Card.Values.ContainsByPredicate([ValueName](const FRelationshipValueDef& Value) { return Value.Name == ValueName; });
	}

	struct FChecker
	{
		FAshesDataTables T;
		FAshesDataReport& Report;

		explicit FChecker(FAshesDataReport& InReport) : Report(InReport) {}

		/** Drops tables with the wrong row struct (their rows cannot be read safely) and records row counts */
		void Prepare(const FAshesDataTables& Tables)
		{
			T = Tables;
			for (const FAshesTableDef& Def : AshesData::GetTableDefs())
			{
				const UDataTable*& Table = T.*(Def.Slot);
				if (!Table)
				{
					Report.Info(Def.Name, FString(), TEXT("table not found, skipped"));
					continue;
				}
				if (Table->GetRowStruct() != Def.GetRowStruct())
				{
					Report.Error(Def.Name, FString(), FString::Printf(TEXT("wrong row struct %s, expected %s"),
						*GetNameSafe(Table->GetRowStruct()), *Def.GetRowStruct()->GetName()));
					Table = nullptr;
					continue;
				}
				Report.RowCounts.Emplace(Def.Name, Table->GetRowMap().Num());
			}
		}

		/** A reference to a clue row; skipped while DT_Clues itself is missing */
		void CheckClueRef(const TCHAR* Table, const FString& Row, FName ClueId, const TCHAR* Field)
		{
			if (!ClueId.IsNone() && T.Clues && !FindRow<FClueRow>(T.Clues, ClueId))
			{
				Report.Error(Table, Row, FString::Printf(TEXT("%s: clue \"%s\" is not in DT_Clues"), Field, *ClueId.ToString()));
			}
		}

		void CheckLineLength(const TCHAR* Table, const FString& Row, const FString& Line, const TCHAR* Field)
		{
			const int32 Words = CountWords(Line);
			if (Words > GMaxLineWords)
			{
				Report.Warning(Table, Row, FString::Printf(TEXT("%s has %d words (limit %d)"), Field, Words, GMaxLineWords));
			}
		}

		void CheckCharacters()
		{
			const TCHAR* Table = TEXT("DT_Characters");
			ForEachRow<FCharacterCardRow>(T.Characters, [&](const FString& Row, const FCharacterCardRow& Card)
			{
				if (IsBlank(Card.DisplayName)) { Report.Error(Table, Row, TEXT("DisplayName is empty")); }
				if (Card.TurnLimit <= 0) { Report.Error(Table, Row, TEXT("TurnLimit must be above 0")); }

				if (Card.bUsesLLM)
				{
					if (Card.Emotions.Num() == 0) { Report.Error(Table, Row, TEXT("Emotions is empty")); }
					if (IsBlank(Card.Voice)) { Report.Warning(Table, Row, TEXT("Voice is empty")); }
					if (IsBlank(Card.SpeechRules)) { Report.Warning(Table, Row, TEXT("SpeechRules is empty")); }
				}
				if (Card.Emotions.Num() > 0 && !Card.Emotions.Contains(Card.FallbackEmotion))
				{
					Report.Error(Table, Row, FString::Printf(TEXT("FallbackEmotion \"%s\" is not in Emotions"), *Card.FallbackEmotion.ToString()));
				}

				TSet<FName> ValueNames;
				for (const FRelationshipValueDef& Value : Card.Values)
				{
					if (Value.Name.IsNone())
					{
						Report.Error(Table, Row, TEXT("a relationship value has no Name"));
						continue;
					}
					bool bAlreadyThere = false;
					ValueNames.Add(Value.Name, &bAlreadyThere);
					if (bAlreadyThere)
					{
						Report.Error(Table, Row, FString::Printf(TEXT("relationship value \"%s\" is listed twice"), *Value.Name.ToString()));
					}
					if (Value.Start < 0 || Value.Start > 100)
					{
						Report.Error(Table, Row, FString::Printf(TEXT("value \"%s\": Start %d is outside 0-100"), *Value.Name.ToString(), Value.Start));
					}
				}

				for (const FChapterSecret& Secret : Card.Secrets)
				{
					if (IsBlank(Secret.Text)) { Report.Error(Table, Row, TEXT("a secret has no Text")); }
				}
			});
		}

		void CheckScenes()
		{
			const TCHAR* Table = TEXT("DT_Scenes");
			ForEachRow<FSceneCardRow>(T.Scenes, [&](const FString& Row, const FSceneCardRow& Scene)
			{
				if (Scene.TurnLimit <= 0) { Report.Error(Table, Row, TEXT("TurnLimit must be above 0")); }
				if (IsBlank(Scene.GoalText)) { Report.Warning(Table, Row, TEXT("GoalText is empty")); }
				if (IsBlank(Scene.Opener)) { Report.Warning(Table, Row, TEXT("Opener is empty")); }

				if (Scene.Characters.Num() == 0) { Report.Error(Table, Row, TEXT("Characters is empty")); }
				for (const FName Character : Scene.Characters)
				{
					if (T.Characters && !FindRow<FCharacterCardRow>(T.Characters, Character))
					{
						Report.Error(Table, Row, FString::Printf(TEXT("character \"%s\" is not in DT_Characters"), *Character.ToString()));
					}
				}

				TSet<FName> BeatIds;
				for (const FBeatDef& Beat : Scene.Beats)
				{
					if (Beat.Id.IsNone())
					{
						Report.Error(Table, Row, TEXT("a beat has no Id"));
						continue;
					}
					bool bAlreadyThere = false;
					BeatIds.Add(Beat.Id, &bAlreadyThere);
					if (bAlreadyThere)
					{
						Report.Error(Table, Row, FString::Printf(TEXT("beat \"%s\" is listed twice"), *Beat.Id.ToString()));
					}
					if (IsBlank(Beat.Rule))
					{
						Report.Warning(Table, Row, FString::Printf(TEXT("beat \"%s\" has no Rule"), *Beat.Id.ToString()));
					}
				}
			});
		}

		void CheckClues()
		{
			const TCHAR* Table = TEXT("DT_Clues");
			ForEachRow<FClueRow>(T.Clues, [&](const FString& Row, const FClueRow& Clue)
			{
				if (Clue.Chapter < 1) { Report.Error(Table, Row, TEXT("Chapter must be 1 or above")); }
				if (IsBlank(Clue.JournalText)) { Report.Error(Table, Row, TEXT("JournalText is empty")); }
				if (IsBlank(Clue.Holder)) { Report.Error(Table, Row, TEXT("Holder is empty")); }
				if (IsBlank(Clue.TopicText)) { Report.Warning(Table, Row, TEXT("TopicText is empty")); }

				if (Clue.Source == EClueSource::Talk)
				{
					if (!IsBlank(Clue.Holder) && T.Characters && !FindRow<FCharacterCardRow>(T.Characters, FName(*Clue.Holder.TrimStartAndEnd())))
					{
						Report.Error(Table, Row, FString::Printf(TEXT("talk clue: holder \"%s\" is not in DT_Characters"), *Clue.Holder));
					}
					if (IsBlank(Clue.PromptHint)) { Report.Warning(Table, Row, TEXT("talk clue without PromptHint")); }
				}

				if (!Clue.RequiresClue.IsNone())
				{
					if (Clue.RequiresClue.ToString().Equals(Row, ESearchCase::IgnoreCase))
					{
						Report.Error(Table, Row, TEXT("RequiresClue points at itself"));
					}
					else
					{
						CheckClueRef(Table, Row, Clue.RequiresClue, TEXT("RequiresClue"));
					}
				}
			});
		}

		void CheckFallbackLines()
		{
			const TCHAR* Table = TEXT("DT_FallbackLines");
			ForEachRow<FFallbackLineRow>(T.FallbackLines, [&](const FString& Row, const FFallbackLineRow& Line)
			{
				if (IsBlank(Line.Line)) { Report.Error(Table, Row, TEXT("Line is empty")); }
				CheckLineLength(Table, Row, Line.Line, TEXT("Line"));

				const FCharacterCardRow* Card = FindRow<FCharacterCardRow>(T.Characters, Line.Character);
				if (Line.Character.IsNone())
				{
					Report.Error(Table, Row, TEXT("Character is empty"));
				}
				else if (T.Characters && !Card)
				{
					Report.Error(Table, Row, FString::Printf(TEXT("character \"%s\" is not in DT_Characters"), *Line.Character.ToString()));
				}

				const bool bAnyScene = AshesData::IsAny(Line.Scene);
				const FSceneCardRow* Scene = bAnyScene ? nullptr : FindRow<FSceneCardRow>(T.Scenes, Line.Scene);
				if (!bAnyScene && T.Scenes)
				{
					if (!Scene)
					{
						Report.Error(Table, Row, FString::Printf(TEXT("scene \"%s\" is not in DT_Scenes"), *Line.Scene.ToString()));
					}
					else if (!Line.Character.IsNone() && !Scene->Characters.Contains(Line.Character))
					{
						Report.Warning(Table, Row, FString::Printf(TEXT("character \"%s\" is not listed in scene \"%s\""), *Line.Character.ToString(), *Line.Scene.ToString()));
					}
				}

				if (!AshesData::IsAny(Line.Beat))
				{
					if (bAnyScene)
					{
						Report.Error(Table, Row, FString::Printf(TEXT("beat \"%s\" needs a Scene"), *Line.Beat.ToString()));
					}
					else if (Scene && !HasBeat(*Scene, Line.Beat))
					{
						Report.Error(Table, Row, FString::Printf(TEXT("beat \"%s\" is not in scene \"%s\""), *Line.Beat.ToString(), *Line.Scene.ToString()));
					}
				}

				if (!AshesData::IsAny(Line.Emotion) && Card && !Card->Emotions.Contains(Line.Emotion))
				{
					Report.Error(Table, Row, FString::Printf(TEXT("emotion \"%s\" is not one of %s's emotions"), *Line.Emotion.ToString(), *Line.Character.ToString()));
				}

				CheckClueRef(Table, Row, Line.Clue, TEXT("Clue"));
			});
		}

		/** Changes text of an offline row: must parse and use only the character's own values */
		void CheckChanges(const TCHAR* Table, const FString& Row, const FString& Changes, const FCharacterCardRow* Card, const TCHAR* Field)
		{
			TMap<FName, int32> Parsed;
			FString ParseError;
			if (!AshesData::ParseChanges(Changes, Parsed, &ParseError))
			{
				Report.Error(Table, Row, FString::Printf(TEXT("%s: %s"), Field, *ParseError));
				return;
			}
			if (Card)
			{
				for (const TPair<FName, int32>& Change : Parsed)
				{
					if (!HasValue(*Card, Change.Key))
					{
						Report.Error(Table, Row, FString::Printf(TEXT("%s: \"%s\" is not a relationship value of this character"), Field, *Change.Key.ToString()));
					}
				}
			}
		}

		/** Beats text of an offline row: only beat ids of the scene (side-character talks have no scene row and no beats) */
		void CheckBeats(const TCHAR* Table, const FString& Row, const FString& Beats, FName SceneName, const FSceneCardRow* Scene, const TCHAR* Field)
		{
			for (const FName BeatId : AshesData::SplitIds(Beats))
			{
				if (Scene ? !HasBeat(*Scene, BeatId) : (T.Scenes != nullptr))
				{
					Report.Error(Table, Row, FString::Printf(TEXT("%s: beat \"%s\" is not in scene \"%s\""), Field, *BeatId.ToString(), *SceneName.ToString()));
				}
			}
		}

		void CheckOfflineTrees()
		{
			const TCHAR* Table = TEXT("DT_OfflineTrees");

			// Nodes per scene and the nodes each node leads to
			TMap<FName, TMap<int32, int32>> RepliesPerNode;
			TMap<FName, TMultiMap<int32, int32>> Links;
			ForEachRow<FOfflineNodeRow>(T.OfflineTrees, [&](const FString& Row, const FOfflineNodeRow& Node)
			{
				if (Node.Scene.IsNone())
				{
					return;
				}
				RepliesPerNode.FindOrAdd(Node.Scene).FindOrAdd(Node.Node)++;
				TMultiMap<int32, int32>& SceneLinks = Links.FindOrAdd(Node.Scene);
				if (Node.NextNode > 0) { SceneLinks.Add(Node.Node, Node.NextNode); }
				if (!IsBlank(Node.StandInLine) && Node.StandInNextNode > 0) { SceneLinks.Add(Node.Node, Node.StandInNextNode); }
			});

			ForEachRow<FOfflineNodeRow>(T.OfflineTrees, [&](const FString& Row, const FOfflineNodeRow& Node)
			{
				if (Node.Scene.IsNone())
				{
					Report.Error(Table, Row, TEXT("Scene is empty"));
					return;
				}
				if (Node.Node < 1) { Report.Error(Table, Row, TEXT("Node must be 1 or above")); }
				if (IsBlank(Node.PlayerLine)) { Report.Error(Table, Row, TEXT("PlayerLine is empty")); }
				if (IsBlank(Node.CharacterLine)) { Report.Error(Table, Row, TEXT("CharacterLine is empty")); }
				CheckLineLength(Table, Row, Node.CharacterLine, TEXT("CharacterLine"));
				CheckLineLength(Table, Row, Node.StandInLine, TEXT("StandInLine"));

				const FCharacterCardRow* Card = FindRow<FCharacterCardRow>(T.Characters, Node.Character);
				if (Node.Character.IsNone())
				{
					Report.Error(Table, Row, TEXT("Character is empty"));
				}
				else if (T.Characters && !Card)
				{
					Report.Error(Table, Row, FString::Printf(TEXT("character \"%s\" is not in DT_Characters"), *Node.Character.ToString()));
				}

				// A scene name that is not in DT_Scenes is a side-character talk; it needs its own chapter
				const FSceneCardRow* Scene = FindRow<FSceneCardRow>(T.Scenes, Node.Scene);
				if (!Scene && T.Scenes && Node.Chapter < 1)
				{
					Report.Error(Table, Row, FString::Printf(TEXT("\"%s\" is not in DT_Scenes; a side-character talk needs Chapter 1 or above"), *Node.Scene.ToString()));
				}

				if (Node.Emotion.IsNone())
				{
					Report.Error(Table, Row, TEXT("Emotion is empty"));
				}
				else if (Card && !Card->Emotions.Contains(Node.Emotion))
				{
					Report.Error(Table, Row, FString::Printf(TEXT("emotion \"%s\" is not one of %s's emotions"), *Node.Emotion.ToString(), *Node.Character.ToString()));
				}

				CheckChanges(Table, Row, Node.Changes, Card, TEXT("Changes"));
				CheckBeats(Table, Row, Node.Beats, Node.Scene, Scene, TEXT("Beats"));
				for (const FName ClueId : AshesData::SplitIds(Node.Clues))
				{
					CheckClueRef(Table, Row, ClueId, TEXT("Clues"));
				}

				const TMap<int32, int32>& SceneNodes = RepliesPerNode.FindChecked(Node.Scene);
				if (Node.NextNode < 0 || (Node.NextNode > 0 && !SceneNodes.Contains(Node.NextNode)))
				{
					Report.Error(Table, Row, FString::Printf(TEXT("NextNode %d does not exist in \"%s\""), Node.NextNode, *Node.Scene.ToString()));
				}

				// Rule + stand-in
				const bool bHasRule = !Node.RequiresValue.IsNone() || !Node.RequiresClue.IsNone();
				const bool bHasStandIn = !IsBlank(Node.StandInLine);
				if (bHasRule && !bHasStandIn) { Report.Error(Table, Row, TEXT("a rule (RequiresValue / RequiresClue) needs a StandInLine")); }
				if (!bHasRule && bHasStandIn) { Report.Warning(Table, Row, TEXT("StandInLine without a rule is never used")); }
				if (!Node.RequiresValue.IsNone() && Card && !HasValue(*Card, Node.RequiresValue))
				{
					Report.Error(Table, Row, FString::Printf(TEXT("RequiresValue \"%s\" is not a relationship value of this character"), *Node.RequiresValue.ToString()));
				}
				CheckClueRef(Table, Row, Node.RequiresClue, TEXT("RequiresClue"));

				if (bHasStandIn)
				{
					if (!Node.StandInEmotion.IsNone() && Card && !Card->Emotions.Contains(Node.StandInEmotion))
					{
						Report.Error(Table, Row, FString::Printf(TEXT("StandInEmotion \"%s\" is not one of %s's emotions"), *Node.StandInEmotion.ToString(), *Node.Character.ToString()));
					}
					CheckChanges(Table, Row, Node.StandInChanges, Card, TEXT("StandInChanges"));
					CheckBeats(Table, Row, Node.StandInBeats, Node.Scene, Scene, TEXT("StandInBeats"));
					if (Node.StandInNextNode < 0 || (Node.StandInNextNode > 0 && !SceneNodes.Contains(Node.StandInNextNode)))
					{
						Report.Error(Table, Row, FString::Printf(TEXT("StandInNextNode %d does not exist in \"%s\""), Node.StandInNextNode, *Node.Scene.ToString()));
					}
				}
			});

			// Per scene: a start node, at most 3 replies per node (3 reply buttons), every node reachable from node 1
			for (const TPair<FName, TMap<int32, int32>>& SceneNodes : RepliesPerNode)
			{
				const FString SceneName = SceneNodes.Key.ToString();
				if (!SceneNodes.Value.Contains(1))
				{
					Report.Error(Table, SceneName, TEXT("the tree has no node 1 (start node)"));
					continue;
				}

				TSet<int32> Reached;
				TArray<int32> Open = { 1 };
				const TMultiMap<int32, int32>& SceneLinks = Links.FindChecked(SceneNodes.Key);
				while (Open.Num() > 0)
				{
					const int32 Current = Open.Pop();
					bool bAlreadyThere = false;
					Reached.Add(Current, &bAlreadyThere);
					if (!bAlreadyThere)
					{
						SceneLinks.MultiFind(Current, Open);
					}
				}

				for (const TPair<int32, int32>& NodeReplies : SceneNodes.Value)
				{
					if (NodeReplies.Value > 3)
					{
						Report.Warning(Table, SceneName, FString::Printf(TEXT("node %d has %d replies (the screen shows 3)"), NodeReplies.Key, NodeReplies.Value));
					}
					if (!Reached.Contains(NodeReplies.Key))
					{
						Report.Warning(Table, SceneName, FString::Printf(TEXT("node %d cannot be reached from node 1"), NodeReplies.Key));
					}
				}
			}
		}

		void CheckBarks()
		{
			const TCHAR* Table = TEXT("DT_Barks");
			ForEachRow<FBarkRow>(T.Barks, [&](const FString& Row, const FBarkRow& Bark)
			{
				if (IsBlank(Bark.Line)) { Report.Error(Table, Row, TEXT("Line is empty")); }
				if (IsBlank(Bark.Speaker)) { Report.Warning(Table, Row, TEXT("Speaker is empty")); }
				if (Bark.Chapter < 1) { Report.Error(Table, Row, TEXT("Chapter must be 1 or above")); }

				const bool bGivesClue = Bark.Kind == EBarkKind::Eavesdrop || Bark.Kind == EBarkKind::DanceGossip;
				if (bGivesClue && Bark.Clue.IsNone()) { Report.Warning(Table, Row, TEXT("eavesdrop / dance gossip without a Clue")); }
				CheckClueRef(Table, Row, Bark.Clue, TEXT("Clue"));
			});
		}

		void CheckDanceCharts()
		{
			const TCHAR* Table = TEXT("DT_DanceCharts");
			TMap<int32, FString> RowOfNight;
			ForEachRow<FDanceChartRow>(T.DanceCharts, [&](const FString& Row, const FDanceChartRow& Chart)
			{
				if (const FString* Other = RowOfNight.Find(Chart.Night))
				{
					Report.Error(Table, Row, FString::Printf(TEXT("Night %d is already used by row \"%s\""), Chart.Night, **Other));
				}
				else
				{
					RowOfNight.Add(Chart.Night, Row);
				}

				if (Chart.BPM <= 0.f || Chart.TempoMultiplier <= 0.f || Chart.LengthSeconds <= 0.f)
				{
					Report.Error(Table, Row, TEXT("BPM, TempoMultiplier and LengthSeconds must be above 0"));
					return;
				}
				if (Chart.Song.IsNull()) { Report.Warning(Table, Row, TEXT("Song is empty")); }
				CheckClueRef(Table, Row, Chart.RewardClue, TEXT("RewardClue"));

				TArray<TPair<float, int32>> Notes;
				FString ParseError;
				if (!AshesData::ParseNotes(Chart.Notes, Notes, &ParseError))
				{
					Report.Error(Table, Row, FString::Printf(TEXT("Notes: %s"), *ParseError));
					return;
				}
				if (Notes.Num() == 0) { Report.Error(Table, Row, TEXT("Notes is empty")); }

				const float SecondsPerBeat = 60.f / (Chart.BPM * Chart.TempoMultiplier);
				for (const TPair<float, int32>& Note : Notes)
				{
					if (Note.Value < 1 || Note.Value > GDanceLaneCount)
					{
						Report.Error(Table, Row, FString::Printf(TEXT("Notes: lane %d is outside 1-%d"), Note.Value, GDanceLaneCount));
					}
					if (Note.Key < 0.f || Note.Key * SecondsPerBeat > Chart.LengthSeconds)
					{
						Report.Warning(Table, Row, FString::Printf(TEXT("Notes: beat %g is outside the %g s song"), Note.Key, Chart.LengthSeconds));
					}
				}
			});
		}

		void CheckCutscenes()
		{
			const TCHAR* Table = TEXT("DT_Cutscenes");
			TMap<FString, FString> RowOfSlot;
			ForEachRow<FCutsceneLineRow>(T.Cutscenes, [&](const FString& Row, const FCutsceneLineRow& Line)
			{
				if (Line.Part.IsNone()) { Report.Error(Table, Row, TEXT("Part is empty")); }
				if (IsBlank(Line.Line)) { Report.Error(Table, Row, TEXT("Line is empty")); }

				const FString Route = AshesData::IsAny(Line.Route) ? FString(TEXT("any")) : Line.Route.ToString().ToLower();
				if (Route != TEXT("any") && Route != TEXT("romance") && Route != TEXT("comrade"))
				{
					Report.Error(Table, Row, FString::Printf(TEXT("Route \"%s\" must be empty, any, romance or comrade"), *Line.Route.ToString()));
				}

				const FString Slot = FString::Printf(TEXT("%s|%d|%s"), *Line.Part.ToString().ToLower(), Line.Order, *Route);
				if (const FString* Other = RowOfSlot.Find(Slot))
				{
					Report.Warning(Table, Row, FString::Printf(TEXT("same Part, Order and Route as row \"%s\""), **Other));
				}
				else
				{
					RowOfSlot.Add(Slot, Row);
				}
			});
		}
	};
}

void AshesData::Validate(const FAshesDataTables& Tables, FAshesDataReport& Report)
{
	FChecker Checker(Report);
	Checker.Prepare(Tables);

	if (Checker.T.Characters) { Checker.CheckCharacters(); }
	if (Checker.T.Scenes) { Checker.CheckScenes(); }
	if (Checker.T.Clues) { Checker.CheckClues(); }
	if (Checker.T.FallbackLines) { Checker.CheckFallbackLines(); }
	if (Checker.T.OfflineTrees) { Checker.CheckOfflineTrees(); }
	if (Checker.T.Barks) { Checker.CheckBarks(); }
	if (Checker.T.DanceCharts) { Checker.CheckDanceCharts(); }
	if (Checker.T.Cutscenes) { Checker.CheckCutscenes(); }
}
