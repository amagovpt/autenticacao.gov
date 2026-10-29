<#
.SYNOPSIS
  Install the build tools of the Windows middleware on a GitHub runner and write their
  PTEID_DIR_* variables (see pteid-mw-pt\windows\set_path_autenticacao.gov.bat) to GITHUB_ENV.
#>
$ErrorActionPreference = 'Stop'
$PSNativeCommandUseErrorActionPreference = $true

function Set-BuildEnv([string]$Name, [string]$Value) {
	if (-not (Test-Path $Value)) { throw "$Name`: $Value does not exist" }
	Write-Host "$Name=$Value"
	"$Name=$Value" | Out-File -Append -Encoding utf8 $env:GITHUB_ENV
}

choco install --no-progress -y doxygen.install swig

# Cryptographic Provider Development Kit (minidriver, CMD key storage provider)
$cpdk = Join-Path $env:RUNNER_TEMP 'cpdksetup.exe'
curl.exe --fail --location --silent --show-error --retry 5 --retry-all-errors --connect-timeout 30 `
	--max-time 600 --output $cpdk 'https://download.microsoft.com/download/1/7/6/176909b0-50f2-4df3-b29b-830a17ea7e38/CPDK_RELEASE_UPDATE/cpdksetup.exe'
if ((Get-FileHash -Algorithm SHA256 $cpdk).Hash -ne '76D6F7580FCE9BB1045B5F0871718342443EC5D99C316489EF8C7BC83D4DD6ED') {
	throw 'cpdksetup.exe: unexpected SHA-256'
}
Start-Process -Wait -FilePath $cpdk -ArgumentList '/quiet', '/norestart'
Set-BuildEnv PTEID_DIR_CRYPTPDK "${env:ProgramFiles(x86)}\Windows Kits\10\Cryptographic Provider Development Kit"

# WiX 3: preinstalled on the runner image, otherwise from Chocolatey
if (-not $env:WIX -or -not (Test-Path "$env:WIX\bin\candle.exe")) {
	choco install --no-progress -y wixtoolset
	$env:WIX = (Get-ChildItem "${env:ProgramFiles(x86)}" -Directory -Filter 'WiX Toolset v3*' | Select-Object -Last 1).FullName
}
Set-BuildEnv PTEID_DIR_WIX $env:WIX.TrimEnd('\')

$vs = & "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * `
	-requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
Set-BuildEnv PTEID_DIR_VS $vs

# Visual C++ 2015-2022 runtime merge modules
$msm = Get-ChildItem "$vs\VC\Redist\MSVC" -Recurse -Filter 'Microsoft_VC143_CRT_x64.msm' | Select-Object -First 1
if (-not $msm) { throw 'Microsoft_VC143_CRT_x64.msm not found' }
Set-BuildEnv PTEID_DIR_MSM $msm.DirectoryName

# SWIG: the real installation directory, the Chocolatey shim does not find the SWIG library
$swig = Get-ChildItem "$env:ChocolateyInstall\lib\swig" -Recurse -Filter swig.exe | Select-Object -First 1
Set-BuildEnv PTEID_DIR_SWIG $swig.DirectoryName
Set-BuildEnv PTEID_DIR_DOXYGEN "$env:ProgramFiles\doxygen\bin"
Set-BuildEnv PTEID_DIR_7ZIP "$env:ProgramFiles\7-Zip"
Set-BuildEnv PTEID_DIR_JDK $env:JAVA_HOME
