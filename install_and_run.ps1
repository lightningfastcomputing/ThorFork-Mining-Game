$ErrorActionPreference = "Stop"

$RepoUrl = "https://github.com/lightningfastcomputing/ThorFork-Mining-Game.git"
$Branch = "deep-systems-edition"
$InstallDir = Join-Path $HOME "ThorFork-Mining-Game"
$MsysRoot = "C:\msys64"
$Bash = Join-Path $MsysRoot "usr\bin\bash.exe"
$TempScript = Join-Path $env:TEMP "thorfork_install_and_run.sh"

function Write-Step {
    param([string]$Message)
    Write-Host "`n==> $Message" -ForegroundColor Cyan
}

Write-Step "Checking for MSYS2"

if (-not (Test-Path $Bash)) {
    $winget = Get-Command winget.exe -ErrorAction SilentlyContinue
    if (-not $winget) {
        throw "winget is unavailable. Install App Installer from Microsoft Store, then retry."
    }

    Write-Step "Installing MSYS2"
    & winget.exe install --id MSYS2.MSYS2 -e `
        --accept-package-agreements `
        --accept-source-agreements

    if ($LASTEXITCODE -ne 0 -and -not (Test-Path $Bash)) {
        throw "MSYS2 installation failed with exit code $LASTEXITCODE."
    }
}

if (-not (Test-Path $Bash)) {
    throw "MSYS2 bash was not found at $Bash."
}

$bashScript = @'
#!/usr/bin/env bash
set -euo pipefail

echo
echo "==> Updating MSYS2 and installing dependencies"
pacman -Syu --noconfirm
pacman -S --needed --noconfirm \
  git \
  make \
  mingw-w64-x86_64-gcc \
  mingw-w64-x86_64-SDL2 \
  mingw-w64-x86_64-SDL2_image \
  mingw-w64-x86_64-SDL2_ttf

repo="$(cygpath -u "$THOR_INSTALL_DIR")"

echo
echo "==> Cloning or updating ThorFork Mining Game"
if [ -d "$repo/.git" ]; then
    git -C "$repo" fetch origin
    git -C "$repo" checkout "$THOR_BRANCH"
    git -C "$repo" pull --ff-only origin "$THOR_BRANCH"
else
    git clone --branch "$THOR_BRANCH" --single-branch "$THOR_REPO_URL" "$repo"
fi

echo
echo "==> Building"
cd "$repo"
make clean-native
make -j1

echo
echo "==> Launching"
export PATH="/mingw64/bin:$PATH"
./Main.exe
'@

Set-Content -Path $TempScript -Value $bashScript -Encoding utf8

$env:MSYSTEM = "MINGW64"
$env:CHERE_INVOKING = "1"
$env:THOR_REPO_URL = $RepoUrl
$env:THOR_BRANCH = $Branch
$env:THOR_INSTALL_DIR = $InstallDir

$TempScriptMsys = (& $Bash -lc "cygpath -u '$TempScript'").Trim()

Write-Step "Running installer"
& $Bash $TempScriptMsys

if ($LASTEXITCODE -ne 0) {
    throw "Installer failed with exit code $LASTEXITCODE."
}
