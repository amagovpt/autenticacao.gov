<#
.SYNOPSIS
  Load the environment of a Visual Studio developer shell (vcvarsall.bat) into the
  following steps of a GitHub Actions job, by writing it to GITHUB_ENV.
.PARAMETER Arch
  vcvarsall.bat architecture: amd64 (x64) or amd64_x86 (x86 cross compiler).
#>
param([Parameter(Mandatory)][ValidateSet('amd64', 'amd64_x86')][string]$Arch)
$ErrorActionPreference = 'Stop'
$PSNativeCommandUseErrorActionPreference = $true

$vs = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * `
	-requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
$vcvarsall = "$vs\VC\Auxiliary\Build\vcvarsall.bat"
if (-not (Test-Path $vcvarsall)) { throw "$vcvarsall not found" }

$before = @{}
Get-ChildItem Env: | ForEach-Object { $before[$_.Name] = $_.Value }

# Only the variables vcvarsall.bat adds or changes
$changed = foreach ($line in cmd /c "`"$vcvarsall`" $Arch >nul && set") {
	$name, $value = $line -split '=', 2
	if ($name -and $before[$name] -ne $value) { $line }
}
if (-not $changed) { throw "vcvarsall.bat $Arch set no variables" }
$changed | Out-File -Append -Encoding utf8 $env:GITHUB_ENV
Write-Host "vcvarsall.bat $Arch`: $(@($changed).Count) variables"
