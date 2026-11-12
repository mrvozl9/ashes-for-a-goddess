// AI-assisted: Claude Code, 12.11.2026, data report and checks for the CSV-imported DataTables (duplicate ids, missing references, empty required fields). Reviewed by Baha.

#pragma once

#include "CoreMinimal.h"
#include "AshesTypes.h"

class UDataTable;
class UScriptStruct;

enum class EAshesDataSeverity : uint8
{
	/** The data is wrong; the game would misbehave */
	Error,
	/** Probably a mistake; the game still runs */
	Warning,
	/** Only a note (e.g. a table has no CSV yet) */
	Info
};

struct ASHES_API FAshesDataIssue
{
	EAshesDataSeverity Severity = EAshesDataSeverity::Error;
	FString Table;
	FString Row;
	FString Message;
};

/** Everything the import and the checks found; printed to the Output Log and saved under Saved/DataReports */
struct ASHES_API FAshesDataReport
{
	TArray<FAshesDataIssue> Issues;

	/** Rows per table, in the order the tables were checked */
	TArray<TPair<FString, int32>> RowCounts;

	void Add(EAshesDataSeverity Severity, const FString& Table, const FString& Row, const FString& Message);
	void Error(const FString& Table, const FString& Row, const FString& Message) { Add(EAshesDataSeverity::Error, Table, Row, Message); }
	void Warning(const FString& Table, const FString& Row, const FString& Message) { Add(EAshesDataSeverity::Warning, Table, Row, Message); }
	void Info(const FString& Table, const FString& Row, const FString& Message) { Add(EAshesDataSeverity::Info, Table, Row, Message); }

	int32 Num(EAshesDataSeverity Severity) const;
	int32 NumErrors() const { return Num(EAshesDataSeverity::Error); }
	int32 NumWarnings() const { return Num(EAshesDataSeverity::Warning); }

	/** True if an issue of this severity exists for the table and row whose message contains MessagePart (tests) */
	bool Has(EAshesDataSeverity Severity, const FString& Table, const FString& Row, const FString& MessagePart) const;

	/** One line per issue, errors first, then the row counts */
	FString ToText() const;
};

/** The tables the checks read; any pointer may be null (that table is then skipped) */
struct ASHES_API FAshesDataTables
{
	const UDataTable* Characters = nullptr;
	const UDataTable* Scenes = nullptr;
	const UDataTable* Clues = nullptr;
	const UDataTable* FallbackLines = nullptr;
	const UDataTable* OfflineTrees = nullptr;
	const UDataTable* Barks = nullptr;
	const UDataTable* DanceCharts = nullptr;
	const UDataTable* Cutscenes = nullptr;
};

/** One imported table: asset name (also the CSV file name) and its row struct */
struct ASHES_API FAshesTableDef
{
	const TCHAR* Name = nullptr;
	UScriptStruct* (*GetRowStruct)() = nullptr;
	const UDataTable* FAshesDataTables::* Slot = nullptr;
};

namespace AshesData
{
	/** Content folder of the DataTable assets: /Game/Ashes/Data */
	ASHES_API const TCHAR* GetAssetFolder();

	/** Folder of the CSV sources: <Project>/Content/Ashes/Data/CSV */
	ASHES_API FString GetCsvFolder();

	/** The eight imported tables, in import order (characters and scenes first, they are referenced by the rest) */
	ASHES_API TConstArrayView<FAshesTableDef> GetTableDefs();

	/** Loads every table asset that exists in /Game/Ashes/Data; missing ones stay null */
	ASHES_API FAshesDataTables LoadTables();

	/** Empty or "any" (any case) */
	ASHES_API bool IsAny(FName Name);

	/** "a; b;c" -> a, b, c (trimmed, empty parts dropped) */
	ASHES_API TArray<FName> SplitIds(const FString& Text);

	/** "trust:2; affection:-3" or "trust:+2" -> map. False (with OutError) when a part is not name:whole number */
	ASHES_API bool ParseChanges(const FString& Text, TMap<FName, int32>& OutChanges, FString* OutError = nullptr);

	/** "4:1;6.5:3" -> (beat, lane) pairs. False (with OutError) when a part is not beat:lane */
	ASHES_API bool ParseNotes(const FString& Text, TArray<TPair<float, int32>>& OutNotes, FString* OutError = nullptr);

	/**
	 * Checks the id column (first column) of a CSV text before it is imported: empty ids and ids used twice.
	 * Returns the number of data rows (the header and blank lines are not counted).
	 */
	ASHES_API int32 CheckCsvIds(const FString& Csv, const FString& TableName, FAshesDataReport& Report);

	/** Checks every table that is set: wrong row struct, empty required fields, references to rows that do not exist */
	ASHES_API void Validate(const FAshesDataTables& Tables, FAshesDataReport& Report);
}
