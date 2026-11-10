# AI-assisted: Claude Code, 10.11.2026, downloads the pinned built-in AI (llama.cpp server + model) and checks SHA-256 (Phase 3). Reviewed by <pending>.
#
# Usage (PowerShell, from the repo root):
#   powershell -ExecutionPolicy Bypass -File Tools\GetLocalLLM.ps1                     # into ThirdParty\LLM (git-ignored)
#   powershell -ExecutionPolicy Bypass -File Tools\GetLocalLLM.ps1 -Target D:\AshesLLM # anywhere else; then set ASHES_LLM_DIR=D:\AshesLLM
#
# The files are NOT in git (too big). The packaged game ships them in <Game>\ThirdParty\LLM.

param(
    [string]$Target = (Join-Path $PSScriptRoot "..\ThirdParty\LLM")
)

$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"

# Pinned versions: change these together with technical_design.md section 12 and Docs/Licenses.md
$Downloads = @(
    @{
        Name   = "llama.cpp b12138 (Windows x64, Vulkan) - MIT license"
        Url    = "https://github.com/ggml-org/llama.cpp/releases/download/b12138/llama-b12138-bin-win-vulkan-x64.zip"
        File   = "llama-b12138-bin-win-vulkan-x64.zip"
        Sha256 = "5fd9523dc1d4ce634a697d760addf5722ec8f7f2dc722234c75124d506d3bb41"
        Unzip  = "llama-server"
    },
    @{
        Name   = "Qwen3.5-4B Q4_K_M GGUF (unsloth, commit e87f176) - Apache 2.0 license"
        Url    = "https://huggingface.co/unsloth/Qwen3.5-4B-GGUF/resolve/e87f176479d0855a907a41277aca2f8ee7a09523/Qwen3.5-4B-Q4_K_M.gguf"
        File   = "models\Qwen3.5-4B-Q4_K_M.gguf"
        Sha256 = "00fe7986ff5f6b463e62455821146049db6f9313603938a70800d1fb69ef11a4"
        Unzip  = $null
    }
)

New-Item -ItemType Directory -Force -Path $Target | Out-Null
$Target = (Resolve-Path $Target).Path

foreach ($d in $Downloads) {
    $out = Join-Path $Target $d.File
    New-Item -ItemType Directory -Force -Path (Split-Path $out) | Out-Null

    $ok = (Test-Path $out) -and ((Get-FileHash $out -Algorithm SHA256).Hash -eq $d.Sha256.ToUpper())
    if (-not $ok) {
        Write-Host "Downloading $($d.Name) ..."
        Invoke-WebRequest -UseBasicParsing -Uri $d.Url -OutFile "$out.part"
        $hash = (Get-FileHash "$out.part" -Algorithm SHA256).Hash
        if ($hash -ne $d.Sha256.ToUpper()) {
            Remove-Item "$out.part"
            throw "SHA-256 mismatch for $($d.File): got $hash"
        }
        Move-Item -Force "$out.part" $out
    }
    Write-Host "OK  $($d.File)  (sha256 verified)"

    if ($d.Unzip) {
        $dest = Join-Path $Target $d.Unzip
        if (-not (Test-Path (Join-Path $dest "llama-server.exe"))) {
            Expand-Archive -Path $out -DestinationPath $dest -Force
        }
        Write-Host "OK  $($d.Unzip)\llama-server.exe"
    }
}

Write-Host ""
Write-Host "Done: $Target"
Write-Host "If this is not <repo>\ThirdParty\LLM, set the environment variable ASHES_LLM_DIR to it."
