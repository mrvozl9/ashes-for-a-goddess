// AI-assisted: Claude Code, 10.11.2026, starts / stops the built-in llama-server and reports when the model is ready (Phase 3). Reviewed by <pending>.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "Containers/Ticker.h"
#include "HAL/PlatformProcess.h"
#include "LocalModelSubsystem.generated.h"

UENUM(BlueprintType)
enum class ELocalModelState : uint8
{
	Stopped,
	Starting,   // process launched, model loading
	Ready,      // /health answered 200
	Failed      // files missing, process exited or startup timeout
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLocalModelStateChanged, ELocalModelState /*NewState*/);

/**
 *  Runs the built-in AI: llama.cpp's llama-server as a hidden child process on 127.0.0.1.
 *  Engine subsystem: in the editor the server survives between PIE sessions (fast iteration);
 *  in the packaged game it lives as long as the game. On Windows the process is put in a job object,
 *  so it also dies if the game crashes.
 */
UCLASS()
class ASHES_API ULocalModelSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Launches the server if it is not running; safe to call many times */
	void StartServer();

	/** Kills the server and frees the memory */
	void StopServer();

	ELocalModelState GetState() const { return State; }
	bool IsReady() const { return State == ELocalModelState::Ready; }

	/** "http://127.0.0.1:<port>/v1" while running */
	FString GetBaseUrl() const;

	/** One line for logs / the settings status ("AI: built-in model ready", ...) */
	FString GetStatusText() const;

	FOnLocalModelStateChanged OnStateChanged;

private:

	bool PollHealth(float DeltaTime);
	void SetState(ELocalModelState NewState, const FString& Reason = FString());
	static int32 FindFreePort(int32 FirstPort);

	ELocalModelState State = ELocalModelState::Stopped;
	FString LastError;
	FProcHandle ServerProcess;
	int32 Port = 0;
	double StartTime = 0.0;
	bool bHealthRequestInFlight = false;
	FTSTicker::FDelegateHandle HealthTicker;

#if PLATFORM_WINDOWS
	void* JobHandle = nullptr;
#endif
};
