# AI-assisted: Codex, 12.11.2026, launch this checkout on its Codex MCP port. Reviewed by <pending>.
param(
    [string]$EngineRoot = "C:\Program Files\Epic Games\UE_5.8"
)
$ErrorActionPreference = "Stop"
$projectFile = (Resolve-Path (Join-Path $PSScriptRoot "..\Ashes.uproject")).Path
$editorExe = Join-Path $EngineRoot "Engine\Binaries\Win64\UnrealEditor.exe"
if (-not (Test-Path -LiteralPath $editorExe)) {
    throw "UE 5.8 editor not found. Supply -EngineRoot with your engine installation folder."
}
# Never restart an existing editor or discard unsaved work.
$existing = @(Get-CimInstance Win32_Process -Filter "Name = 'UnrealEditor.exe'" | Where-Object {
    $_.CommandLine -and $_.CommandLine.Replace('/', '\').Contains($projectFile)
})
if ($existing.Count -gt 0) {
    Write-Host "This Ashes checkout is already open (PID $($existing.ProcessId -join ', '))."
    Write-Host "Check Tools/Test-UnrealMCP.py; if its port differs, close Ashes normally before relaunching."
    exit 0
}
$listener = @(Get-NetTCPConnection -State Listen -LocalPort 8001 -ErrorAction SilentlyContinue)
if ($listener.Count -gt 0) {
    throw "Port 8001 is already occupied. Verify its project before changing any connection settings."
}
$editorArgs = @('"' + $projectFile + '"', '-ModelContextProtocolPort=8001', '-ModelContextProtocolStartServer')
$process = Start-Process -FilePath $editorExe -ArgumentList $editorArgs -WindowStyle Hidden -PassThru
Write-Host "Started Ashes editor PID $($process.Id); MCP: http://127.0.0.1:8001/mcp"
Write-Host "Wait for L_Test to load, then run: python Tools/Test-UnrealMCP.py"
