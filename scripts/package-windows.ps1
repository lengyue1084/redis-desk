[CmdletBinding()]
param(
    [string]$Version,
    [string]$QtDir = $env:QTDIR,
    [string]$MinGwDir,
    [string]$BuildDir,
    [string]$OutputRoot,
    [string]$InnoSetupPath,
    [string]$CertificatePath,
    [string]$TimestampUrl = "http://timestamp.digicert.com"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = Split-Path -Parent $PSScriptRoot
$cmakeFile = Join-Path $repoRoot "CMakeLists.txt"
$cmakeText = Get-Content -LiteralPath $cmakeFile -Raw
$versionMatch = [regex]::Match($cmakeText, 'project\s*\(\s*RedisDesk\s+VERSION\s+([0-9]+\.[0-9]+\.[0-9]+)', 'IgnoreCase')
if (-not $versionMatch.Success) {
    throw "Unable to read the RedisDesk version from CMakeLists.txt."
}

$projectVersion = $versionMatch.Groups[1].Value
if ([string]::IsNullOrWhiteSpace($Version)) {
    $Version = $projectVersion
}
if ($Version -notmatch '^\d+\.\d+\.\d+$') {
    throw "Version '$Version' must use the X.Y.Z format."
}
if ($Version -ne $projectVersion) {
    throw "Package version '$Version' does not match CMake project version '$projectVersion'."
}

function Resolve-Tool {
    param(
        [Parameter(Mandatory = $true)][string]$Name,
        [string[]]$Candidates = @()
    )

    foreach ($candidate in $Candidates) {
        if ($candidate -and (Test-Path -LiteralPath $candidate -PathType Leaf)) {
            return (Resolve-Path -LiteralPath $candidate).Path
        }
    }
    $command = Get-Command $Name -ErrorAction SilentlyContinue
    if ($command) {
        return $command.Source
    }
    throw "Required tool '$Name' was not found."
}

if ([string]::IsNullOrWhiteSpace($QtDir)) {
    $QtDir = "D:\Qt\6.5.3\mingw_64"
}
$QtDir = (Resolve-Path -LiteralPath $QtDir).Path
$qtRoot = Split-Path -Parent (Split-Path -Parent $QtDir)

if ([string]::IsNullOrWhiteSpace($MinGwDir)) {
    $preferredToolchain = Join-Path $qtRoot "Tools\mingw1120_64\bin"
    if (Test-Path -LiteralPath (Join-Path $preferredToolchain "g++.exe") -PathType Leaf) {
        $MinGwDir = $preferredToolchain
    } else {
        $pathCompiler = Get-Command "g++.exe" -ErrorAction SilentlyContinue
        if ($pathCompiler) {
            $MinGwDir = Split-Path -Parent $pathCompiler.Source
        } else {
            $toolchains = Get-ChildItem -LiteralPath (Join-Path $qtRoot "Tools") -Directory -Filter "mingw*_64" -ErrorAction SilentlyContinue |
                Where-Object { Test-Path -LiteralPath (Join-Path $_.FullName "bin\g++.exe") } |
                Sort-Object LastWriteTime -Descending
            if (-not $toolchains) {
                throw "A 64-bit MinGW toolchain was not found under '$qtRoot\Tools'."
            }
            $MinGwDir = Join-Path $toolchains[0].FullName "bin"
        }
    }
}
$MinGwDir = (Resolve-Path -LiteralPath $MinGwDir).Path

$cmake = Resolve-Tool "cmake.exe" @(
    (Join-Path $qtRoot "Tools\CMake_64\bin\cmake.exe")
)
$ninja = Resolve-Tool "ninja.exe" @(
    (Join-Path $qtRoot "Tools\Ninja\ninja.exe")
)
$compiler = Resolve-Tool "g++.exe" @(
    (Join-Path $MinGwDir "g++.exe")
)
$windDeployQt = Resolve-Tool "windeployqt.exe" @(
    (Join-Path $QtDir "bin\windeployqt.exe")
)
$env:PATH = "$(Join-Path $QtDir 'bin');$MinGwDir;$env:PATH"
$env:QT_PLUGIN_PATH = Join-Path $QtDir "plugins"

if ([string]::IsNullOrWhiteSpace($BuildDir)) {
    $BuildDir = Join-Path $repoRoot "out\build\windows-release"
}
if ([string]::IsNullOrWhiteSpace($OutputRoot)) {
    $OutputRoot = Join-Path $repoRoot "out"
}
$BuildDir = [System.IO.Path]::GetFullPath($BuildDir)
$OutputRoot = [System.IO.Path]::GetFullPath($OutputRoot)
$stageDir = Join-Path $OutputRoot "package\RedisDesk-v$Version-windows-x64"
$installerDir = Join-Path $OutputRoot "installer"

Write-Host "Building RedisDesk $Version"
& $cmake --fresh -S $repoRoot -B $BuildDir -G Ninja `
    "-DCMAKE_BUILD_TYPE=Release" `
    "-DCMAKE_PREFIX_PATH=$QtDir" `
    "-DCMAKE_CXX_COMPILER=$compiler" `
    "-DCMAKE_MAKE_PROGRAM=$ninja"
if ($LASTEXITCODE -ne 0) { throw "CMake configuration failed." }

& $cmake --build $BuildDir --config Release
if ($LASTEXITCODE -ne 0) { throw "RedisDesk build failed." }

$builtExe = Join-Path $BuildDir "bin\RedisDesk.exe"
if (-not (Test-Path -LiteralPath $builtExe -PathType Leaf)) {
    throw "Built executable was not found at '$builtExe'."
}

if (Test-Path -LiteralPath $stageDir) {
    Remove-Item -LiteralPath $stageDir -Recurse -Force
}
New-Item -ItemType Directory -Path $stageDir -Force | Out-Null
New-Item -ItemType Directory -Path $installerDir -Force | Out-Null
Copy-Item -LiteralPath $builtExe -Destination $stageDir

& $windDeployQt --compiler-runtime --no-system-d3d-compiler --no-translations --dir $stageDir (Join-Path $stageDir "RedisDesk.exe")
if ($LASTEXITCODE -ne 0) { throw "windeployqt failed." }

Copy-Item -LiteralPath (Join-Path $repoRoot "LICENSE") -Destination $stageDir
Copy-Item -LiteralPath (Join-Path $repoRoot "README.md") -Destination $stageDir
Copy-Item -LiteralPath (Join-Path $repoRoot "README.en.md") -Destination $stageDir
Set-Content -LiteralPath (Join-Path $stageDir "qt.conf") -Value "[Paths]`r`nPlugins=." -Encoding Ascii

function Resolve-SignTool {
    $command = Get-Command "signtool.exe" -ErrorAction SilentlyContinue
    if ($command) { return $command.Source }

    $kitsRoot = Join-Path ${env:ProgramFiles(x86)} "Windows Kits\10\bin"
    $candidate = Get-ChildItem -LiteralPath $kitsRoot -Recurse -Filter "signtool.exe" -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -match '\\x64\\signtool\.exe$' } |
        Sort-Object FullName -Descending |
        Select-Object -First 1
    if ($candidate) { return $candidate.FullName }
    throw "signtool.exe was not found. Install the Windows SDK or omit CertificatePath."
}

function Invoke-CodeSigning {
    param([Parameter(Mandatory = $true)][string]$Path)

    $password = $env:REDISDESK_CERT_PASSWORD
    if ([string]::IsNullOrWhiteSpace($password)) {
        throw "REDISDESK_CERT_PASSWORD is required when CertificatePath is provided."
    }
    $signTool = Resolve-SignTool
    & $signTool sign /fd SHA256 /td SHA256 /tr $TimestampUrl /f $CertificatePath /p $password $Path
    if ($LASTEXITCODE -ne 0) { throw "Code signing failed for '$Path'." }
}

if (-not [string]::IsNullOrWhiteSpace($CertificatePath)) {
    $CertificatePath = (Resolve-Path -LiteralPath $CertificatePath).Path
    Invoke-CodeSigning (Join-Path $stageDir "RedisDesk.exe")
} else {
    Write-Warning "No signing certificate supplied; creating an unsigned package."
}

if ([string]::IsNullOrWhiteSpace($InnoSetupPath)) {
    $knownInnoPaths = @(
        (Join-Path ${env:ProgramFiles(x86)} "Inno Setup 6\ISCC.exe"),
        (Join-Path $env:ProgramFiles "Inno Setup 6\ISCC.exe"),
        (Join-Path $env:LOCALAPPDATA "Programs\Inno Setup 6\ISCC.exe")
    )
    $InnoSetupPath = Resolve-Tool "ISCC.exe" $knownInnoPaths
} else {
    $InnoSetupPath = (Resolve-Path -LiteralPath $InnoSetupPath).Path
}

$innoScript = Join-Path $repoRoot "packaging\windows\RedisDesk.iss"
& $InnoSetupPath `
    "/DAppVersion=$Version" `
    "/DSourceDir=$stageDir" `
    "/DOutputDir=$installerDir" `
    "/DRepoRoot=$repoRoot" `
    $innoScript
if ($LASTEXITCODE -ne 0) { throw "Inno Setup compilation failed." }

$installerPath = Join-Path $installerDir "RedisDesk-Setup-v$Version-x64.exe"
if (-not (Test-Path -LiteralPath $installerPath -PathType Leaf)) {
    throw "Installer was not found at '$installerPath'."
}
if (-not [string]::IsNullOrWhiteSpace($CertificatePath)) {
    Invoke-CodeSigning $installerPath
}

$hash = (Get-FileHash -LiteralPath $installerPath -Algorithm SHA256).Hash.ToLowerInvariant()
$hashPath = "$installerPath.sha256"
Set-Content -LiteralPath $hashPath -Value "$hash  $([System.IO.Path]::GetFileName($installerPath))" -Encoding Ascii

Write-Host "Installer: $installerPath"
Write-Host "SHA-256:  $hashPath"
