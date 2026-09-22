#!/usr/bin/env pwsh
# Compile and run a single exercise, with ASan + UBSan when the toolchain
# supports them.
# Usage: .\build.ps1 week1-cpp03-to-move\s01-object-lifetime\main.cpp
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

    $compileArgs = @("-std=c++17", "-Wall", "-Wextra", "-Wshadow", "-g", "-O0") +
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

if ($result.ExitCode -ne 0) {
    $result.Output | ForEach-Object { Write-Host $_ }
    exit $result.ExitCode
}

& $out
$runExitCode = $LASTEXITCODE

Remove-Item -Force $out -ErrorAction SilentlyContinue

exit $runExitCode
