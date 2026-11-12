// AI-assisted: Claude Code, 12.11.2026, console commands Ashes.Data.Import / Validate / ExportTemplates (CSV -> DataTable assets + data report). Reviewed by Baha.
//
// Workflow: the CSV files live in Content/Ashes/Data/CSV/<TableName>.csv (first column = row id).
// After changing a CSV, run "Ashes.Data.Import" in the editor console: every table that has a CSV is rebuilt from it,
// saved, and all tables are checked. The report is printed to the Output Log and saved in Saved/DataReports.
// No code changes are needed for new or changed rows.

#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"
#include "HAL/FileManager.h"
#include "Engine/DataTable.h"
#include "Misc/FileHelper.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Ashes.h"
#include "AshesDataValidator.h"

#if WITH_EDITOR
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#endif

namespace AshesDataCommands
{
	/** Prints the report and saves it as Saved/DataReports/<Prefix>_<time>.txt */
	static void Publish(const FAshesDataReport& Report, const TCHAR* Prefix)
	{
		const FString Text = Report.ToText();

		TArray<FString> Lines;
		Text.ParseIntoArrayLines(Lines);
		for (const FString& Line : Lines)
		{
			if (Line.StartsWith(TEXT("ERROR")))
			{
				UE_LOG(LogAshes, Error, TEXT("%s"), *Line);
			}
			else if (Line.StartsWith(TEXT("WARNING")))
			{
				UE_LOG(LogAshes, Warning, TEXT("%s"), *Line);
			}
			else
			{
				UE_LOG(LogAshes, Display, TEXT("%s"), *Line);
			}
		}

		const FString Path = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("DataReports"),
			FString::Printf(TEXT("%s_%s.txt"), Prefix, *FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M%S"))));
		if (FFileHelper::SaveStringToFile(Text, *Path, FFileHelper::EEncodingOptions::ForceUTF8))
		{
			UE_LOG(LogAshes, Display, TEXT("Data report saved: %s"), *FPaths::ConvertRelativePathToFull(Path));
		}
	}

	static void Validate()
	{
		FAshesDataReport Report;
		AshesData::Validate(AshesData::LoadTables(), Report);
		Publish(Report, TEXT("DataValidate"));
	}

#if WITH_EDITOR

	static FString GetCsvPath(const FAshesTableDef& Def)
	{
		return FPaths::Combine(AshesData::GetCsvFolder(), FString(Def.Name) + TEXT(".csv"));
	}

	/** Rebuilds one table asset from its CSV. The asset is only touched when the CSV can be read completely. */
	static void ImportTable(const FAshesTableDef& Def, FAshesDataReport& Report)
	{
		const FString TableName = Def.Name;
		const FString CsvPath = GetCsvPath(Def);
		if (!FPaths::FileExists(CsvPath))
		{
			Report.Info(TableName, FString(), TEXT("no CSV file, table left as it is"));
			return;
		}

		FString Csv;
		if (!FFileHelper::LoadFileToString(Csv, *CsvPath))
		{
			Report.Error(TableName, FString(), FString::Printf(TEXT("cannot read %s"), *CsvPath));
			return;
		}

		// 1. Ids: stop before the import would silently drop rows
		const int32 ErrorsBefore = Report.NumErrors();
		const int32 DataRows = AshesData::CheckCsvIds(Csv, TableName, Report);
		if (DataRows == 0)
		{
			Report.Info(TableName, FString(), TEXT("the CSV has no data rows, table left as it is"));
			return;
		}
		if (Report.NumErrors() > ErrorsBefore)
		{
			Report.Error(TableName, FString(), TEXT("NOT imported: fix the id problems above first"));
			return;
		}

		// 2. Trial import into a throw-away table, so a broken CSV never empties the real one
		UDataTable* Trial = NewObject<UDataTable>(GetTransientPackage());
		Trial->RowStruct = Def.GetRowStruct();
		for (const FString& Problem : Trial->CreateTableFromCSVString(Csv))
		{
			Report.Warning(TableName, FString(), FString::Printf(TEXT("import: %s"), *Problem));
		}
		if (Trial->GetRowMap().Num() != DataRows)
		{
			Report.Error(TableName, FString(), FString::Printf(TEXT("NOT imported: %d of %d CSV rows could be read"), Trial->GetRowMap().Num(), DataRows));
			return;
		}

		// 3. The real asset (created when it does not exist yet)
		const FString PackageName = FString::Printf(TEXT("%s/%s"), AshesData::GetAssetFolder(), Def.Name);
		const FString Filename = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
		const bool bExists = FPackageName::DoesPackageExist(PackageName);
		if (bExists && IFileManager::Get().IsReadOnly(*Filename))
		{
			Report.Error(TableName, FString(), FString::Printf(
				TEXT("NOT imported: the asset is read-only. Lock it first: git lfs lock Content/Ashes/Data/%s.uasset"), Def.Name));
			return;
		}

		UDataTable* Table = nullptr;
		UPackage* Package = nullptr;
		if (bExists)
		{
			Table = LoadObject<UDataTable>(nullptr, *FString::Printf(TEXT("%s.%s"), *PackageName, Def.Name));
			if (!Table || Table->GetRowStruct() != Def.GetRowStruct())
			{
				Report.Error(TableName, FString(), TEXT("NOT imported: the asset is missing or has the wrong row struct"));
				return;
			}
			Package = Table->GetPackage();
		}
		else
		{
			Package = CreatePackage(*PackageName);
			Table = NewObject<UDataTable>(Package, FName(Def.Name), RF_Public | RF_Standalone);
			Table->RowStruct = Def.GetRowStruct();
			FAssetRegistryModule::AssetCreated(Table);
		}

		Table->Modify();
		Table->CreateTableFromCSVString(Csv);
		Table->MarkPackageDirty();

		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError;
		SaveArgs.bSlowTask = false;
		if (UPackage::SavePackage(Package, Table, *Filename, SaveArgs))
		{
			Report.Info(TableName, FString(), FString::Printf(TEXT("%s %d rows from CSV"), bExists ? TEXT("reimported") : TEXT("created with"), DataRows));
		}
		else
		{
			Report.Error(TableName, FString(), FString::Printf(TEXT("imported in memory but could not save %s"), *Filename));
		}
	}

	static void Import(const TArray<FString>& Args)
	{
		FAshesDataReport Report;
		for (const FAshesTableDef& Def : AshesData::GetTableDefs())
		{
			// Optional argument: only this table (e.g. Ashes.Data.Import DT_Clues)
			if (Args.Num() == 0 || Args.Contains(FString(Def.Name)))
			{
				ImportTable(Def, Report);
			}
		}
		AshesData::Validate(AshesData::LoadTables(), Report);
		Publish(Report, TEXT("DataImport"));
	}

	/** Writes a CSV for every table that has none yet: the current rows of the asset, or only the header row */
	static void ExportTemplates()
	{
		for (const FAshesTableDef& Def : AshesData::GetTableDefs())
		{
			const FString CsvPath = GetCsvPath(Def);
			if (FPaths::FileExists(CsvPath))
			{
				UE_LOG(LogAshes, Display, TEXT("%s: CSV already exists, not overwritten"), Def.Name);
				continue;
			}

			const FString PackageName = FString::Printf(TEXT("%s/%s"), AshesData::GetAssetFolder(), Def.Name);
			const UDataTable* Table = FPackageName::DoesPackageExist(PackageName)
				? LoadObject<UDataTable>(nullptr, *FString::Printf(TEXT("%s.%s"), *PackageName, Def.Name))
				: nullptr;
			if (!Table || Table->GetRowStruct() != Def.GetRowStruct())
			{
				UDataTable* Empty = NewObject<UDataTable>(GetTransientPackage());
				Empty->RowStruct = Def.GetRowStruct();
				Table = Empty;
			}

			if (FFileHelper::SaveStringToFile(Table->GetTableAsCSV(), *CsvPath, FFileHelper::EEncodingOptions::ForceUTF8))
			{
				UE_LOG(LogAshes, Display, TEXT("%s: wrote %s (%d rows)"), Def.Name, *CsvPath, Table->GetRowMap().Num());
			}
			else
			{
				UE_LOG(LogAshes, Error, TEXT("%s: cannot write %s"), Def.Name, *CsvPath);
			}
		}
	}

#endif // WITH_EDITOR
}

static FAutoConsoleCommand GAshesDataValidate(
	TEXT("Ashes.Data.Validate"),
	TEXT("Check the DataTables in /Game/Ashes/Data (empty required fields, references to missing rows) and write a report"),
	FConsoleCommandDelegate::CreateStatic(&AshesDataCommands::Validate));

#if WITH_EDITOR

static FAutoConsoleCommand GAshesDataImport(
	TEXT("Ashes.Data.Import"),
	TEXT("Rebuild the DataTables from Content/Ashes/Data/CSV/<Table>.csv, save them and write a report. Optional: table names (e.g. Ashes.Data.Import DT_Clues)"),
	FConsoleCommandWithArgsDelegate::CreateStatic(&AshesDataCommands::Import));

static FAutoConsoleCommand GAshesDataExportTemplates(
	TEXT("Ashes.Data.ExportTemplates"),
	TEXT("Write a CSV (current rows or just the header) for every table that has no CSV yet in Content/Ashes/Data/CSV"),
	FConsoleCommandDelegate::CreateStatic(&AshesDataCommands::ExportTemplates));

#endif // WITH_EDITOR
