$ErrorActionPreference = "Stop"

$RepoUrl = "https://github.com/lightningfastcomputing/ThorFork-Mining-Game.git"
$Branch = "deep-systems-edition"
$InstallDir = Join-Path $HOME "ThorFork-Mining-Game"
$MsysRoot = "C:\msys64"
$Bash = Join-Path $MsysRoot "usr\bin\bash.exe"

function Write-Step {
    param([string]$Message)
    Write-Host "`n==> $Message" -ForegroundColor Cyan
}

Write-Step "Checking for MSYS2"

if (-not (Test-Path $Bash)) {
    $winget = Get-Command winget.exe -ErrorAction SilentlyContinue
    if (-not $winget) {
        throw "winget is not available. Install App Installer from Microsoft Store, then run this command again."
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
    throw "MSYS2 bash was not found at $Bash after installation."
}

$env:MSYSTEM = "MINGW64"
$env:CHERE_INVOKING = "1"
$env:THOR_REPO_URL = $RepoUrl
$env:THOR_BRANCH = $Branch
$env:THOR_INSTALL_DIR = $InstallDir

Write-Step "Updating MSYS2 package metadata"
& $Bash -lc 'pacman -Syu --noconfirm'
if ($LASTEXITCODE -ne 0) {
    Write-Warning "The first MSYS2 update pass returned exit code $LASTEXITCODE. Retrying in a fresh shell."
}

Write-Step "Installing compiler and SDL dependencies"
& $Bash -lc @'
pacman -Syu --noconfirm
pacman -S --needed --noconfirm \
  git \
  make \
  mingw-w64-x86_64-gcc \
  mingw-w64-x86_64-SDL2 \
  mingw-w64-x86_64-SDL2_image \
  mingw-w64-x86_64-SDL2_ttf
'@

if ($LASTEXITCODE -ne 0) {
    throw "Dependency installation failed with exit code $LASTEXITCODE."
}

Write-Step "Cloning or updating ThorFork Mining Game"
& $Bash -lc @'
set -e

repo="$(cygpath -u "$THOR_INSTALL_DIR")"

if [ -d "$repo/.git" ]; then
    git -C "$repo" fetch origin
    git -C "$repo" checkout "$THOR_BRANCH"
    git -C "$repo" pull --ff-only origin "$THOR_BRANCH"
else
    git clone --branch "$THOR_BRANCH" --single-branch "$THOR_REPO_URL" "$repo"
fi
'@

if ($LASTEXITCODE -ne 0) {
    throw "Repository setup failed with exit code $LASTEXITCODE."
}

Write-Step "Building the game"
& $Bash -lc @'
set -e

repo="$(cygpath -u "$THOR_INSTALL_DIR")"
cd "$repo"

make clean-native
make -j1
'@

if ($LASTEXITCODE -ne 0) {
    throw "Build failed with exit code $LASTEXITCODE."
}

Write-Step "Launching ThorFork Mining Game"
& $Bash -lc @'
set -e

repo="$(cygpath -u "$THOR_INSTALL_DIR")"
cd "$repo"

export PATH="/mingw64/bin:$PATH"
./Main.exe
'@
