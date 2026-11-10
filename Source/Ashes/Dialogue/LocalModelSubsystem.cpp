// AI-assisted: Claude Code, 10.11.2026, starts / stops the built-in llama-server and reports when the model is ready (Phase 3). Reviewed by <pending>.

#include "LocalModelSubsystem.h"
#include "AshesAISettings.h"
#include "Ashes.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Misc/Paths.h"
#include "HAL/PlatformTime.h"
#include "SocketSubsystem.h"
#include "Sockets.h"
#include "IPAddress.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

void ULocalModelSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// In the packaged game the model loads right away, while the title screen shows.
	// In the editor it starts on demand (PIE start or Ashes.LLM.Start), so the editor itself stays light.
	if (!GIsEditor && GetDefault<UAshesAISettings>()->bAutoStartLocalModel && GetDefault<UAshesAISettings>()->ExternalBaseUrl.IsEmpty())
	{
		StartServer();
	}
}

void ULocalModelSubsystem::Deinitialize()
{
	StopServer();
	Super::Deinitialize();
}

FString ULocalModelSubsystem::GetBaseUrl() const
{
	return Port > 0 ? FString::Printf(TEXT("http://127.0.0.1:%d/v1"), Port) : FString();
}

FString ULocalModelSubsystem::GetStatusText() const
{
	switch (State)
	{
	case ELocalModelState::Ready:    return TEXT("AI: built-in model ready");
	case ELocalModelState::Starting: return TEXT("AI: built-in model loading...");
	case ELocalModelState::Failed:   return FString::Printf(TEXT("AI: built-in model unavailable (%s), pre-written lines"), *LastError);
	default:                         return TEXT("AI: built-in model stopped");
	}
}

int32 ULocalModelSubsystem::FindFreePort(int32 FirstPort)
{
	ISocketSubsystem* Sockets = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	if (!Sockets)
	{
		return FirstPort;
	}

	for (int32 Candidate = FirstPort; Candidate < FirstPort + 50; ++Candidate)
	{
		FSocket* Socket = Sockets->CreateSocket(NAME_Stream, TEXT("AshesPortProbe"), false);
		if (!Socket)
		{
			break;
		}
		TSharedRef<FInternetAddr> Addr = Sockets->CreateInternetAddr();
		bool bValidIp = false;
		Addr->SetIp(TEXT("127.0.0.1"), bValidIp);
		Addr->SetPort(Candidate);
		const bool bFree = Socket->Bind(*Addr);
		Socket->Close();
		Sockets->DestroySocket(Socket);
		if (bFree)
		{
			return Candidate;
		}
	}
	return FirstPort;
}

void ULocalModelSubsystem::StartServer()
{
	if (State == ELocalModelState::Starting || State == ELocalModelState::Ready)
	{
		return;
	}

	const UAshesAISettings* Settings = GetDefault<UAshesAISettings>();
	const FString Dir = Settings->ResolveLocalModelDirectory();
	const FString Exe = FPaths::Combine(Dir, Settings->ServerExecutable);
	const FString ModelPath = FPaths::Combine(Dir, Settings->ModelFile);

	if (!FPaths::FileExists(Exe) || !FPaths::FileExists(ModelPath))
	{
		SetState(ELocalModelState::Failed, TEXT("files not found"));
		UE_LOG(LogAshes, Warning, TEXT("Built-in AI files missing. Expected '%s' and '%s'. Run Tools/GetLocalLLM.ps1 or set ASHES_LLM_DIR."), *Exe, *ModelPath);
		return;
	}

	Port = FindFreePort(Settings->PreferredPort);

	FString Args = FString::Printf(
		TEXT("-m \"%s\" --host 127.0.0.1 --port %d -c %d -np 1 --no-webui --reasoning off -a %s"),
		*ModelPath, Port, Settings->ContextSize, *Settings->ModelAlias);
	if (!Settings->Device.IsEmpty())
	{
		Args += FString::Printf(TEXT(" --device %s"), *Settings->Device);
	}
	if (Settings->GpuLayers >= 0)
	{
		Args += FString::Printf(TEXT(" -ngl %d"), Settings->GpuLayers);
	}

	// Hidden window; working directory = the server folder so its DLLs load
	ServerProcess = FPlatformProcess::CreateProc(*Exe, *Args, false, true, true, nullptr, 0, *FPaths::GetPath(Exe), nullptr);
	if (!ServerProcess.IsValid())
	{
		SetState(ELocalModelState::Failed, TEXT("could not launch"));
		return;
	}

#if PLATFORM_WINDOWS
	// Kill the server together with the game, even after a crash
	HANDLE Job = CreateJobObjectW(nullptr, nullptr);
	if (Job)
	{
		JOBOBJECT_EXTENDED_LIMIT_INFORMATION Info = {};
		Info.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
		SetInformationJobObject(Job, JobObjectExtendedLimitInformation, &Info, sizeof(Info));
		AssignProcessToJobObject(Job, ServerProcess.Get());
		JobHandle = Job;
	}
#endif

	StartTime = FPlatformTime::Seconds();
	SetState(ELocalModelState::Starting);
	UE_LOG(LogAshes, Log, TEXT("Built-in AI starting on port %d: %s"), Port, *FPaths::GetCleanFilename(ModelPath));

	HealthTicker = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ULocalModelSubsystem::PollHealth), 0.5f);
}

bool ULocalModelSubsystem::PollHealth(float DeltaTime)
{
	if (State != ELocalModelState::Starting)
	{
		return false; // stop ticking
	}

	if (!FPlatformProcess::IsProcRunning(ServerProcess))
	{
		StopServer();
		SetState(ELocalModelState::Failed, TEXT("server exited"));
		return false;
	}

	if (FPlatformTime::Seconds() - StartTime > GetDefault<UAshesAISettings>()->StartupTimeoutSeconds)
	{
		StopServer();
		SetState(ELocalModelState::Failed, TEXT("startup timeout"));
		return false;
	}

	if (!bHealthRequestInFlight)
	{
		bHealthRequestInFlight = true;
		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Http = FHttpModule::Get().CreateRequest();
		Http->SetURL(FString::Printf(TEXT("http://127.0.0.1:%d/health"), Port));
		Http->SetVerb(TEXT("GET"));
		Http->SetTimeout(2.f);
		TWeakObjectPtr<ULocalModelSubsystem> WeakThis(this);
		Http->OnProcessRequestComplete().BindLambda([WeakThis](FHttpRequestPtr, FHttpResponsePtr Response, bool bConnected)
		{
			if (ULocalModelSubsystem* Self = WeakThis.Get())
			{
				Self->bHealthRequestInFlight = false;
				if (bConnected && Response.IsValid() && Response->GetResponseCode() == 200 && Self->State == ELocalModelState::Starting)
				{
					UE_LOG(LogAshes, Log, TEXT("Built-in AI ready after %.1f s"), FPlatformTime::Seconds() - Self->StartTime);
					Self->SetState(ELocalModelState::Ready);
				}
			}
		});
		Http->ProcessRequest();
	}
	return true;
}

void ULocalModelSubsystem::StopServer()
{
	if (HealthTicker.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(HealthTicker);
		HealthTicker.Reset();
	}

	if (ServerProcess.IsValid())
	{
		if (FPlatformProcess::IsProcRunning(ServerProcess))
		{
			FPlatformProcess::TerminateProc(ServerProcess, true);
		}
		FPlatformProcess::CloseProc(ServerProcess);
		ServerProcess.Reset();
	}

#if PLATFORM_WINDOWS
	if (JobHandle)
	{
		CloseHandle(static_cast<HANDLE>(JobHandle));
		JobHandle = nullptr;
	}
#endif

	bHealthRequestInFlight = false;
	if (State == ELocalModelState::Starting || State == ELocalModelState::Ready)
	{
		SetState(ELocalModelState::Stopped);
	}
	Port = 0;
}

void ULocalModelSubsystem::SetState(ELocalModelState NewState, const FString& Reason)
{
	State = NewState;
	LastError = Reason;
	if (NewState == ELocalModelState::Failed)
	{
		UE_LOG(LogAshes, Warning, TEXT("Built-in AI failed: %s (pre-written lines will be used)"), *Reason);
	}
	OnStateChanged.Broadcast(NewState);
}
