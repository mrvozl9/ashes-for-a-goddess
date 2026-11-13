// AI-assisted: Claude Code, 10.11.2026, project settings for the built-in AI (Phase 3); editor-only timeout (11.11.2026); soft / hard limits (12.11.2026); 10 s thinking limit to the first words (13.11.2026). Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "AshesAISettings.generated.h"

/**
 *  Project Settings > Game > Ashes AI. Stored in Config/DefaultGame.ini (no secrets here, ever).
 *  The built-in AI is llama.cpp's llama-server + one GGUF model, shipped in <Game>/ThirdParty/LLM.
 *  In development the folder can live elsewhere: set the environment variable ASHES_LLM_DIR.
 */
UCLASS(config=Game, defaultconfig, meta=(DisplayName="Ashes AI"))
class ASHES_API UAshesAISettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	UAshesAISettings();

	/** Start the built-in model when the game starts (and on the first request in the editor) */
	UPROPERTY(config, EditAnywhere, Category="Built-in model")
	bool bAutoStartLocalModel = true;

	/** Folder with llama-server and models, relative to the project / game folder. ASHES_LLM_DIR overrides it. */
	UPROPERTY(config, EditAnywhere, Category="Built-in model")
	FString LocalModelDirectory = TEXT("ThirdParty/LLM");

	/** Server executable inside LocalModelDirectory */
	UPROPERTY(config, EditAnywhere, Category="Built-in model")
	FString ServerExecutable = TEXT("llama-server/llama-server.exe");

	/** Model file inside LocalModelDirectory */
	UPROPERTY(config, EditAnywhere, Category="Built-in model")
	FString ModelFile = TEXT("models/Qwen3.5-4B-Q4_K_M.gguf");

	/** First port tried on 127.0.0.1; the next free one is used if it is taken */
	UPROPERTY(config, EditAnywhere, Category="Built-in model", meta=(ClampMin="1024", ClampMax="65000"))
	int32 PreferredPort = 8731;

	/** Context size in tokens (prompt + reply) */
	UPROPERTY(config, EditAnywhere, Category="Built-in model", meta=(ClampMin="1024"))
	int32 ContextSize = 4096;

	/** llama.cpp --device value: empty = llama.cpp default, "none" = CPU only, "Vulkan1" = a specific GPU */
	UPROPERTY(config, EditAnywhere, Category="Built-in model")
	FString Device;

	/** Layers on the GPU: -1 = let llama.cpp fit them into free VRAM */
	UPROPERTY(config, EditAnywhere, Category="Built-in model", meta=(ClampMin="-1"))
	int32 GpuLayers = -1;

	/** Seconds to wait for the model to load before giving up (fallback lines are used meanwhile) */
	UPROPERTY(config, EditAnywhere, Category="Built-in model", meta=(ClampMin="5", Units="s"))
	float StartupTimeoutSeconds = 90.f;

	/** Soft limit (GDD: 6 s): after it Zafir visibly hesitates, but the game keeps waiting for the real answer */
	UPROPERTY(config, EditAnywhere, Category="Requests", meta=(ClampMin="1", Units="s"))
	float RequestTimeoutSeconds = 6.f;

	/** Hard limit in the game: no answer by then -> a hesitate line, the turn does not count. Packaged builds always use this. */
	UPROPERTY(config, EditAnywhere, Category="Requests", meta=(ClampMin="1", Units="s"))
	float HardTimeoutSeconds = 20.f;

	/** Hard limit in the editor and Play-in-Editor only (slow development PCs); never used by packaged builds */
	UPROPERTY(config, EditAnywhere, Category="Requests", meta=(ClampMin="1", Units="s"))
	float EditorHardTimeoutSeconds = 60.f;

	/**
	 *  Thinking limit (Baha, 13.11.2026): if none of the reply's words are on screen this long after the player's line,
	 *  the model is stopped and the character dodges (the turn does not count). Once words appear, the reply may finish
	 *  up to the hard limit. Same value in the editor and the game.
	 */
	UPROPERTY(config, EditAnywhere, Category="Requests", meta=(ClampMin="1", Units="s"))
	float FirstWordsTimeoutSeconds = 10.f;

	/** Hard limit: EditorHardTimeoutSeconds in the editor (incl. PIE), HardTimeoutSeconds in the game */
	float GetEffectiveRequestTimeout() const;

	/** Soft limit: when the hesitation shows (RequestTimeoutSeconds) */
	float GetHesitateAfterSeconds() const { return RequestTimeoutSeconds; }

	/** Model name sent in requests (llama-server alias) */
	UPROPERTY(config, EditAnywhere, Category="Requests")
	FString ModelAlias = TEXT("ashes-local");

	/** Optional external OpenAI-compatible endpoint (e.g. a local Ollama "http://127.0.0.1:11434/v1"); empty = built-in model */
	UPROPERTY(config, EditAnywhere, Category="External endpoint (optional)")
	FString ExternalBaseUrl;

	UPROPERTY(config, EditAnywhere, Category="External endpoint (optional)")
	FString ExternalModel;

	/** Resolved folder (ASHES_LLM_DIR, else LocalModelDirectory under the project / game folder) */
	FString ResolveLocalModelDirectory() const;
};
