#!/usr/bin/env pwsh
# Compile and run a single exercise, with ASan + UBSan when the toolchain
# supports them.
# Usage: .\build.ps1 3-zivotni-vek-i-resursi\19-zivotni-vek-objekta\main.cpp
#
# Requires a g++ on PATH (MinGW-w64 via MSYS2 or w64devkit). Many MSYS2
# GCC builds don't ship libasan/libubsan for the mingw target -- if the
# sanitized build fails to LINK (not compile), this script automatically
# retries without sanitizers so you're not blocked. For real ASan/UBSan
# coverage on Windows, install the MSYS2 CLANG64 toolchain instead:
#   pacman -S mingw-w64-clang-x86_64-toolchain
# then run: .\build.ps1 <file> -Compiler clang++
# (add C:\msys64\clang64\bin to PATH first). Works in both PowerShell 5.1
# and PowerShell 7 (pwsh).

param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$Src,

    [string]$Compiler = "g++",

    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$ExtraArgs
)

$ErrorActionPreference = "Stop"

if (-not (Get-Command $Compiler -ErrorAction SilentlyContinue)) {
    Write-Error "$Compiler not found on PATH. Install MinGW-w64 (MSYS2 or w64devkit) and add its bin/ to PATH."
    exit 1
}

if (-not (Test-Path $Src)) {
    Write-Error "Source file not found: $Src"
    exit 1
}

$out = Join-Path $env:TEMP ("mcpp-" + [System.Guid]::NewGuid().ToString("N").Substring(0, 8) + ".exe")

function Invoke-Compile {
    param([string[]]$SanitizeFlags)

    # Local override: with the outer $ErrorActionPreference = "Stop", a mere
    # compiler WARNING on stderr (merged via 2>&1) would otherwise be treated
    # as a terminating error. This assignment only shadows the preference
    # inside this function, so it doesn't affect the rest of the script.
    $ErrorActionPreference = "Continue"

    # -pedantic-errors: code the standard calls ill-formed is always an error
    # (g++ otherwise only warns on narrowing from a variable inside {}).
    # clang only warns on out-of-order designated initializers, and that
    # warning is not part of -pedantic, so it is promoted separately.
    # Variable length arrays are only a warning on clang without -Werror=vla.
    $strictFlags = @("-pedantic-errors", "-Werror=vla")
    if ($Compiler -match "clang") { $strictFlags += "-Werror=reorder-init-list" }

    $compileArgs = @("-std=c++17", "-Wall", "-Wextra", "-Wshadow", "-g", "-O0") +
                   $strictFlags +
                   $SanitizeFlags +
                   @("-fno-omit-frame-pointer", $Src, "-o", $out) +
                   $ExtraArgs

    $output = & $Compiler @compileArgs 2>&1
    return @{ ExitCode = $LASTEXITCODE; Output = $output }
}

$result = Invoke-Compile -SanitizeFlags @("-fsanitize=address,undefined")

if ($result.ExitCode -ne 0 -and ($result.Output -match "cannot find -lasan|cannot find -lubsan")) {
    Write-Warning "Toolchain has no libasan/libubsan -- building WITHOUT sanitizers. Memory bugs in exercises (04, 06, etc.) won't be caught this way. See the comment at the top of this script to fix it properly."
    $result = Invoke-Compile -SanitizeFlags @()
}

$result.Output | ForEach-Object { Write-Host $_ }
if ($result.ExitCode -ne 0) {
    exit $result.ExitCode
}

& $out
$runExitCode = $LASTEXITCODE

Remove-Item -Force $out -ErrorAction SilentlyContinue

exit $runExitCode
