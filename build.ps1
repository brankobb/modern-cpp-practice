#!/usr/bin/env pwsh
# Compile and run a single exercise with ASan + UBSan.
# Usage: .\build.ps1 week1-cpp03-to-move\s01-object-lifetime\main.cpp
#
# Requires MinGW-w64 g++ (e.g. via MSYS2 or w64devkit) on PATH, GCC 12+
# for AddressSanitizer support on Windows. Works in both PowerShell 5.1
# and PowerShell 7 (pwsh).

param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$Src,

    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$ExtraArgs
)

$ErrorActionPreference = "Stop"

if (-not (Get-Command g++ -ErrorAction SilentlyContinue)) {
    Write-Error "g++ not found on PATH. Install MinGW-w64 (MSYS2 or w64devkit) and add its bin/ to PATH."
    exit 1
}

if (-not (Test-Path $Src)) {
    Write-Error "Source file not found: $Src"
    exit 1
}

$out = Join-Path $env:TEMP ("mcpp-" + [System.Guid]::NewGuid().ToString("N").Substring(0, 8) + ".exe")

$compileArgs = @(
    "-std=c++17", "-Wall", "-Wextra", "-Wshadow", "-g", "-O0",
    "-fsanitize=address,undefined", "-fno-omit-frame-pointer",
    $Src, "-o", $out
) + $ExtraArgs

& g++ @compileArgs
if ($LASTEXITCODE -ne 0) {
    exit $LASTEXITCODE
}

& $out
$runExitCode = $LASTEXITCODE

Remove-Item -Force $out -ErrorAction SilentlyContinue

exit $runExitCode
