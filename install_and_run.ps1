$ErrorActionPreference = "Stop"

$ScriptVersion = "2.0.0"
$RepoUrl = "https://github.com/lightningfastcomputing/ThorFork-Mining-Game.git"
$Branch = "deep-systems-edition"
$MsysRoot = "C:\msys64"
$Bash = Join-Path $MsysRoot "usr\bin\bash.exe"

Write-Host "ThorFork installer v$ScriptVersion" -ForegroundColor Green

function Step([string]$Message) {
    Write-Host "`n==> $Message" -ForegroundColor Cyan
}

Step "Checking for MSYS2"

if (-not (Test-Path $Bash)) {
    if (-not (Get-Command winget.exe -ErrorAction SilentlyContinue)) {
        throw "winget is unavailable. Install Microsoft App Installer, then retry."
    }

    Step "Installing MSYS2"
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

$env:MSYSTEM = "MINGW64"
$env:CHERE_INVOKING = "1"

# Use the MSYS2 home directory directly to avoid Windows/Bash path conversion bugs.
$RepoPosix = "/home/$env:USERNAME/ThorFork-Mining-Game"

Step "Updating MSYS2"
& $Bash -lc "pacman -Syu --noconfirm"
if ($LASTEXITCODE -ne 0) {
    Write-Warning "MSYS2 update returned exit code $LASTEXITCODE; continuing with dependency installation."
}

Step "Installing compiler and SDL dependencies"
& $Bash -lc "pacman -S --needed --noconfirm git make mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_ttf"
if ($LASTEXITCODE -ne 0) {
    throw "Dependency installation failed with exit code $LASTEXITCODE."
}

Step "Cloning or updating ThorFork Mining Game"

$RepoExists = & $Bash -lc "test -d '$RepoPosix/.git'"
$RepoExistsCode = $LASTEXITCODE

if ($RepoExistsCode -eq 0) {
    & $Bash -lc "git -C '$RepoPosix' fetch origin"
    if ($LASTEXITCODE -ne 0) { throw "git fetch failed." }

    & $Bash -lc "git -C '$RepoPosix' checkout '$Branch'"
    if ($LASTEXITCODE -ne 0) { throw "git checkout failed." }

    & $Bash -lc "git -C '$RepoPosix' pull --ff-only origin '$Branch'"
    if ($LASTEXITCODE -ne 0) { throw "git pull failed." }
}
else {
    & $Bash -lc "git clone --branch '$Branch' --single-branch '$RepoUrl' '$RepoPosix'"
    if ($LASTEXITCODE -ne 0) { throw "git clone failed." }
}

Step "Building"
& $Bash -lc "cd '$RepoPosix' && make clean-native && make -j1"
if ($LASTEXITCODE -ne 0) {
    throw "Build failed with exit code $LASTEXITCODE."
}

Step "Launching"
& $Bash -lc "export PATH=/mingw64/bin:`$PATH; cd '$RepoPosix' && ./Main.exe"
if ($LASTEXITCODE -ne 0) {
    throw "The game exited with code $LASTEXITCODE."
}
