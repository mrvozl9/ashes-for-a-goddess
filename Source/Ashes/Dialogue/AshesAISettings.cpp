// AI-assisted: Claude Code, 10.11.2026, project settings for the built-in AI (Phase 3); editor-only timeout (11.11.2026); soft / hard limits (12.11.2026). Reviewed by <pending>.

#include "AshesAISettings.h"
#include "Misc/Paths.h"
#include "HAL/PlatformMisc.h"

UAshesAISettings::UAshesAISettings()
{
	CategoryName = TEXT("Game");
	SectionName = TEXT("Ashes AI");
}

float UAshesAISettings::GetEffectiveRequestTimeout() const
{
	return GIsEditor ? EditorHardTimeoutSeconds : HardTimeoutSeconds;
}

FString UAshesAISettings::ResolveLocalModelDirectory() const
{
	const FString FromEnv = FPlatformMisc::GetEnvironmentVariable(TEXT("ASHES_LLM_DIR"));
	if (!FromEnv.IsEmpty())
	{
		return FPaths::ConvertRelativePathToFull(FromEnv);
	}
	return FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), LocalModelDirectory));
}
