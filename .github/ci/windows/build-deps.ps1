<#
.SYNOPSIS
  Build the third-party libraries of the Windows middleware for one architecture.

.DESCRIPTION
  The Visual Studio projects take their dependencies from the PTEID_DIR_* variables of
  pteid-mw-pt\windows\set_path_autenticacao.gov.bat. This script builds the libraries
  into that layout, with the library names the projects link to, so the build itself
  runs unchanged: everything static, against the DLL C runtime (/MD), as in the
  official installers, which ship no third-party DLL besides those of the application.

  For x64 it also builds poppler with the Qt 6 frontend (poppler.dll, poppler-qt6.dll)
  for the application. Run it from a Visual Studio developer shell for the target
  architecture. It writes the PTEID_* variables to $env:GITHUB_ENV when that is set.

.PARAMETER Arch
  x86 or x64.
.PARAMETER Prefix
  Directory the libraries are installed into; OpenPACE goes to <Prefix>\..\openpace,
  shared by both architectures, as PTEID_DIR_OPEN_PACE expects.
.PARAMETER QtDir
  Qt 6 for MSVC x64 (x64 only, for poppler-qt6).
#>
param(
	[Parameter(Mandatory)][ValidateSet('x86', 'x64')][string]$Arch,
	[Parameter(Mandatory)][string]$Prefix,
	[string]$QtDir,
	[string]$WorkDir = (Join-Path $env:RUNNER_TEMP "deps-$Arch")
)
$ErrorActionPreference = 'Stop'
$PSNativeCommandUseErrorActionPreference = $true

$Sources = @{
	'zlib-1.3.1.tar.gz'           = @('9a93b2b7dfdac77ceba5a558a580e74667dd6fede4585b91eefb60f03b72df23', 'https://github.com/madler/zlib/releases/download/v1.3.1/zlib-1.3.1.tar.gz')
	'openssl-3.0.18.tar.gz'       = @('d80c34f5cf902dccf1f1b5df5ebb86d0392e37049e5d73df1b3abae72e4ffe8b', 'https://github.com/openssl/openssl/releases/download/openssl-3.0.18/openssl-3.0.18.tar.gz')
	'xerces-c-3.2.4.tar.gz'       = @('8dfaa30d6a641bda113625ef65e43c433e8ffd94fadd3b8d39dfe6faf450f26d', 'https://github.com/apache/xerces-c/archive/refs/tags/v3.2.4.tar.gz')
	'xml-security-c-2.0.4.tar.gz' = @('a78da6720f6c2ba14100d2426131e0d33eac5a2dba5cc11bdd04974b7eb89003', 'https://archive.apache.org/dist/santuario/c-library/xml-security-c-2.0.4.tar.gz')
	'curl-8.16.0.tar.xz'          = @('40c8cddbcb6cc6251c03dea423a472a6cea4037be654ba5cf5dec6eb2d22ff1d', 'https://github.com/curl/curl/releases/download/curl-8_16_0/curl-8.16.0.tar.xz')
	'libzip-1.11.4.tar.xz'        = @('8a247f57d1e3e6f6d11413b12a6f28a9d388de110adc0ec608d893180ed7097b', 'https://github.com/nih-at/libzip/releases/download/v1.11.4/libzip-1.11.4.tar.xz')
	'libpng-1.6.50.tar.gz'        = @('71158e53cfdf2877bc99bcab33641d78df3f48e6e0daad030afe9cb8c031aa46', 'https://github.com/pnggroup/libpng/archive/refs/tags/v1.6.50.tar.gz')
	'openjpeg-2.4.0.tar.gz'       = @('8702ba68b442657f11aaeb2b338443ca8d5fb95b0d845757968a7be31ef7f16d', 'https://github.com/uclouvain/openjpeg/archive/refs/tags/v2.4.0.tar.gz')
	'cJSON-1.7.15.tar.gz'         = @('5308fd4bd90cef7aa060558514de6a1a4a0819974a26e6ed13973c5f624c24b2', 'https://github.com/DaveGamble/cJSON/archive/refs/tags/v1.7.15.tar.gz')
	'openpace-1.1.3.tar.gz'       = @('a686df0037fd5cbdc23298ad4254f8c264cd65a5f6cbfa8a834c8cc542d3fd2a', 'https://github.com/frankmorgner/openpace/archive/refs/tags/1.1.3.tar.gz')
	'freetype-2.13.3.tar.xz'      = @('0550350666d427c74daeb85d5ac7bb353acba5f76956395995311a9c6f063289', 'https://download.savannah.gnu.org/releases/freetype/freetype-2.13.3.tar.xz')
	'libjpeg-turbo-3.0.4.tar.gz'  = @('99130559e7d62e8d695f2c0eaeef912c5828d5b84a0537dcb24c9678c9d5b76b', 'https://github.com/libjpeg-turbo/libjpeg-turbo/releases/download/3.0.4/libjpeg-turbo-3.0.4.tar.gz')
	'poppler-25.10.0.tar.xz'      = @('6b5e9bb64dabb15787a14db1675291c7afaf9387438cc93a4fb7f6aec4ee6fe0', 'https://poppler.freedesktop.org/poppler-25.10.0.tar.xz')
}

New-Item -ItemType Directory -Force -Path $WorkDir, $Prefix | Out-Null
$Prefix = (Resolve-Path $Prefix).Path

# Download, verify and extract an archive; returns the source directory
function Get-Source([string]$Name) {
	$sha, $url = $Sources[$Name]
	$archive = Join-Path $WorkDir $Name
	if (-not (Test-Path $archive)) {
		Write-Host "Downloading $url"
		# curl.exe of Windows: Invoke-WebRequest has no transfer timeout and may hang for hours
		curl.exe --fail --location --silent --show-error --retry 5 --retry-all-errors `
			--connect-timeout 30 --max-time 600 --output $archive $url
	}
	$actual = (Get-FileHash -Algorithm SHA256 $archive).Hash.ToLower()
	if ($actual -ne $sha) { throw "$Name`: SHA-256 $actual, expected $sha" }
	$dir = Join-Path $WorkDir ($Name -replace '\.tar\.(gz|xz)$', '')
	Remove-Item -Recurse -Force $dir -ErrorAction SilentlyContinue
	New-Item -ItemType Directory -Path $dir | Out-Null
	Write-Host "Extracting $Name"
	if ($archive -like '*.xz') {
		# The tar of Windows has no xz support: unpack the .tar with 7-Zip first
		7z x -y -bd "-o$WorkDir" $archive | Out-Null
		$archive = $archive -replace '\.xz$', ''
	}
	tar -xf $archive -C $dir --strip-components 1
	return $dir
}

$CMakeCommon = @('-G', 'Ninja', '-DCMAKE_BUILD_TYPE=Release', '-DBUILD_SHARED_LIBS=OFF',
	'-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL', '-DCMAKE_POLICY_VERSION_MINIMUM=3.5',
	'-DCMAKE_POLICY_DEFAULT_CMP0091=NEW')

function Invoke-CMakeBuild([string]$Src, [string]$Dest, [string[]]$Options) {
	cmake -S $Src -B "$Src\build" @CMakeCommon "-DCMAKE_INSTALL_PREFIX=$Dest" @Options
	cmake --build "$Src\build" --parallel
	cmake --install "$Src\build"
}

# Build one library unless the same recipe was already built into $Prefix (restored from
# the cache): the stamp is the SHA-256 of the script block, which names the source archive.
# A failed run thus resumes at the library that failed.
function Invoke-Library([string]$Name, [scriptblock]$Build) {
	$stamp = [Convert]::ToHexString([Security.Cryptography.SHA256]::HashData(
		[Text.Encoding]::UTF8.GetBytes("$Arch`n$Build")))
	$marker = Join-Path $Prefix ".built\$Name"
	if ((Test-Path $marker) -and (Get-Content -Raw $marker).Trim() -eq $stamp) {
		Write-Host "$Name`: already built"
		return
	}
	Write-Host "::group::$Name"
	& $Build
	New-Item -ItemType Directory -Force -Path (Split-Path $marker) | Out-Null
	Set-Content $marker $stamp
	if ($env:GITHUB_OUTPUT) { 'built=true' | Out-File -Append -Encoding utf8 $env:GITHUB_OUTPUT }
	Write-Host '::endgroup::'
}

# zlib: zlibstatic.lib
$Zlib = "$Prefix\zlib"
Invoke-Library zlib {
	$src = Get-Source 'zlib-1.3.1.tar.gz'
	Invoke-CMakeBuild $src $Zlib @('-DZLIB_BUILD_EXAMPLES=OFF')
}

# OpenSSL: static libcrypto.lib, libssl.lib
Invoke-Library OpenSSL {
	$src = Get-Source 'openssl-3.0.18.tar.gz'
	$target = if ($Arch -eq 'x64') { 'VC-WIN64A' } else { 'VC-WIN32' }
	Push-Location $src
	perl Configure $target no-shared no-tests "--prefix=$Prefix\openssl" "--openssldir=$Prefix\openssl\ssl"
	# no-shared selects the static C runtime (/MT); the middleware links against the DLL one
	(Get-Content makefile) -replace '/MT\b', '/MD' -replace '/Zi\b', '/Zi /FS' | Set-Content makefile
	# Libraries and headers only: the openssl.exe link step still expects the static runtime
	$make = if (Get-Command jom -ErrorAction SilentlyContinue) { 'jom' } else { 'nmake' }
	& $make /nologo build_libs
	nmake /nologo install_dev
	Pop-Location
}

# Xerces-C: xerces-c_3.lib; the generated header defines XERCES_STATIC_LIBRARY
Invoke-Library Xerces-C {
	$src = Get-Source 'xerces-c-3.2.4.tar.gz'
	Invoke-CMakeBuild $src "$Prefix\xerces" @('-Dnetwork-accessor=winsock', '-Dtranscoder=windows',
		'-Dmessage-loader=inmemory', '-DBUILD_TESTING=OFF')
}

# XML-Security-C: xsec_lib.lib from the upstream Visual Studio project ("Release Minimal":
# without Xalan), built as a static library
Invoke-Library XML-Security-C {
	$src = Get-Source 'xml-security-c-2.0.4.tar.gz'
	$platform = if ($Arch -eq 'x64') { 'x64' } else { 'Win32' }
	$env:CL = "/I`"$Prefix\openssl\include`" /I`"$Prefix\xerces\include`" /DXERCES_STATIC_LIBRARY"
	msbuild "$src\Projects\VC15.0\xsec\xsec_lib\xsec_lib.vcxproj" /nologo /m /v:minimal `
		"/p:Configuration=Release Minimal" "/p:Platform=$platform" /p:ConfigurationType=StaticLibrary `
		/p:PlatformToolset=v143 /p:WindowsTargetPlatformVersion=10.0 "/p:XERCESCROOT=$Prefix\xerces"
	Remove-Item Env:CL
	New-Item -ItemType Directory -Force -Path "$Prefix\xmlsec\lib", "$Prefix\xmlsec\include\xsec" | Out-Null
	Copy-Item (Get-ChildItem -Recurse "$src\Build" -Filter xsec_lib.lib | Select-Object -First 1).FullName "$Prefix\xmlsec\lib\"
	Get-ChildItem -Recurse "$src\xsec" -Include *.hpp | ForEach-Object {
		$rel = $_.FullName.Substring("$src\xsec".Length + 1)
		$dest = Join-Path "$Prefix\xmlsec\include\xsec" $rel
		New-Item -ItemType Directory -Force -Path (Split-Path $dest) | Out-Null
		Copy-Item $_.FullName $dest
	}
	# The Windows configuration header plays the part of the generated XSECConfig.hpp: it must
	# declare the provider the library was built with ("Release Minimal": OpenSSL only)
	$config = "$Prefix\xmlsec\include\xsec\framework\XSECW32Config.hpp"
	(Get-Content $config) -replace '^// #define XSEC_HAVE_OPENSSL 1', '#define XSEC_HAVE_OPENSSL 1' | Set-Content $config
	if (-not (Select-String -Quiet -Pattern '^#define XSEC_HAVE_OPENSSL 1' $config)) { throw "$config`: XSEC_HAVE_OPENSSL not set" }
}

# libcurl: libcurl_a.lib, the name of curl's own static Windows build, with OpenSSL
Invoke-Library libcurl {
	$src = Get-Source 'curl-8.16.0.tar.xz'
	Invoke-CMakeBuild $src "$Prefix\curl" @('-DBUILD_CURL_EXE=OFF', '-DBUILD_TESTING=OFF', '-DBUILD_LIBCURL_DOCS=OFF',
		'-DBUILD_MISC_DOCS=OFF', '-DENABLE_CURL_MANUAL=OFF', '-DCURL_USE_OPENSSL=ON', "-DOPENSSL_ROOT_DIR=$Prefix\openssl",
		'-DOPENSSL_USE_STATIC_LIBS=ON', '-DCURL_USE_LIBPSL=OFF', '-DCURL_USE_LIBSSH2=OFF', '-DUSE_LIBIDN2=OFF',
		'-DUSE_NGHTTP2=OFF', '-DCURL_BROTLI=OFF', '-DCURL_ZSTD=OFF', '-DCURL_ZLIB=OFF', '-DCURL_DISABLE_LDAP=ON',
		'-DLIBCURL_OUTPUT_NAME=libcurl_a')
	if (-not (Test-Path "$Prefix\curl\lib\libcurl_a.lib")) {
		Copy-Item (Get-ChildItem "$Prefix\curl\lib" -Filter 'libcurl*.lib' | Select-Object -First 1).FullName "$Prefix\curl\lib\libcurl_a.lib"
	}
}

# libzip: libzip.lib (Windows crypto, no OpenSSL)
Invoke-Library libzip {
	$src = Get-Source 'libzip-1.11.4.tar.xz'
	Invoke-CMakeBuild $src "$Prefix\libzip" @("-DZLIB_ROOT=$Zlib", "-DZLIB_LIBRARY=$Zlib\lib\zlibstatic.lib",
		'-DENABLE_GNUTLS=OFF', '-DENABLE_MBEDTLS=OFF', '-DENABLE_OPENSSL=OFF', '-DENABLE_BZIP2=OFF', '-DENABLE_LZMA=OFF',
		'-DENABLE_ZSTD=OFF', '-DBUILD_TOOLS=OFF', '-DBUILD_REGRESS=OFF', '-DBUILD_OSSFUZZ=OFF', '-DBUILD_EXAMPLES=OFF',
		'-DBUILD_DOC=OFF')
	Copy-Item "$Prefix\libzip\lib\zip.lib" "$Prefix\libzip\lib\libzip.lib"
}

# libpng: libpng16_static.lib
Invoke-Library libpng {
	$src = Get-Source 'libpng-1.6.50.tar.gz'
	Invoke-CMakeBuild $src "$Prefix\libpng" @("-DZLIB_ROOT=$Zlib", "-DZLIB_LIBRARY=$Zlib\lib\zlibstatic.lib",
		'-DPNG_SHARED=OFF', '-DPNG_STATIC=ON', '-DPNG_TESTS=OFF', '-DPNG_TOOLS=OFF')
}

# OpenJPEG: openjp2.lib, include\openjpeg-2.4 (the projects define OPJ_STATIC)
Invoke-Library OpenJPEG {
	$src = Get-Source 'openjpeg-2.4.0.tar.gz'
	Invoke-CMakeBuild $src "$Prefix\openjpeg" @('-DBUILD_CODEC=OFF', '-DBUILD_STATIC_LIBS=ON')
}

# cJSON: cjson.lib; some sources include "cJSON.h", others <cjson/cJSON.h>
Invoke-Library cJSON {
	$src = Get-Source 'cJSON-1.7.15.tar.gz'
	Invoke-CMakeBuild $src "$Prefix\cjson" @('-DENABLE_CJSON_TEST=OFF', '-DENABLE_CJSON_UTILS=OFF', '-DENABLE_CUSTOM_COMPILER_FLAGS=OFF')
	Copy-Item "$Prefix\cjson\include\cjson\cJSON.h" "$Prefix\cjson\include\"
}

# OpenPACE: libeac.lib. OpenPACE has no Windows build of the library, so its sources
# (libeac_la_SOURCES of src/Makefile.am) are compiled directly. CVCDIR and X509DIR are
# the autotools defaults.
$OpenPace = Join-Path (Split-Path $Prefix) 'openpace'
$libDir = Join-Path $OpenPace ($(if ($Arch -eq 'x64') { 'x64' } else { 'win32' }))
Invoke-Library OpenPACE {
	$src = Get-Source 'openpace-1.1.3.tar.gz'
	New-Item -ItemType Directory -Force -Path "$OpenPace\include\eac", $libDir | Out-Null
	$libeac = (Select-String -Path "$src\src\Makefile.am" -Pattern '^libeac_la_SOURCES\s*=\s*(.*)$').Matches[0].Groups[1].Value -split '\s+' | Where-Object { $_ }
	Push-Location "$src\src"
	# What configure finds in OpenSSL 3; without it ssl_compat.h redeclares OpenSSL functions
	@('ASN1_STRING_GET0_DATA', 'DECL_OPENSSL_ZALLOC', 'DH_GET0_KEY', 'DH_GET0_PQG', 'DH_SET0_KEY', 'DH_SET0_PQG',
		'ECDSA_SIG_GET0', 'ECDSA_SIG_SET0', 'EC_KEY_METHOD', 'EC_POINT_GET_AFFINE_COORDINATES',
		'EC_POINT_SET_AFFINE_COORDINATES', 'EVP_PKEY_DUP', 'RSA_GET0_KEY', 'RSA_SET0_KEY') |
		ForEach-Object { "#define HAVE_$_ 1" } | Set-Content config.h
	cl /nologo /c /O2 /MD /DHAVE_CONFIG_H /DWIN32_LEAN_AND_MEAN /D_CRT_SECURE_NO_WARNINGS '/DCVCDIR="/usr/local/etc/eac/cvc"' `
		'/DX509DIR="/usr/local/etc/eac/x509"' /I. "/I$Prefix\openssl\include" @libeac
	lib /nologo "/OUT:$libDir\libeac.lib" ($libeac | ForEach-Object { $_ -replace '\.c$', '.obj' })
	Copy-Item eac\*.h "$OpenPace\include\eac\"
	Pop-Location
}

if ($Arch -eq 'x64') {
	if (-not $QtDir) { throw 'QtDir is required for x64 (poppler-qt6)' }
	# poppler with the Qt 6 frontend for the application: poppler.dll and poppler-qt6.dll,
	# with FreeType, libjpeg, libpng, OpenJPEG and zlib linked in
	$PopplerDeps = "$Prefix\poppler-deps"

	Invoke-Library FreeType {
		$src = Get-Source 'freetype-2.13.3.tar.xz'
		Invoke-CMakeBuild $src $PopplerDeps @('-DFT_DISABLE_HARFBUZZ=ON', '-DFT_DISABLE_BROTLI=ON', '-DFT_DISABLE_BZIP2=ON',
			'-DFT_DISABLE_PNG=ON', '-DFT_DISABLE_ZLIB=ON')
	}

	Invoke-Library libjpeg-turbo {
		$src = Get-Source 'libjpeg-turbo-3.0.4.tar.gz'
		Invoke-CMakeBuild $src $PopplerDeps @('-DENABLE_SHARED=OFF', '-DENABLE_STATIC=ON', '-DWITH_TURBOJPEG=OFF')
	}

	Invoke-Library poppler {
		$src = Get-Source 'poppler-25.10.0.tar.xz'
		cmake -S $src -B "$src\build" -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON `
			-DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDLL "-DCMAKE_INSTALL_PREFIX=$Prefix\poppler" `
			-DCMAKE_CXX_FLAGS=/DOPJ_STATIC `
			"-DCMAKE_PREFIX_PATH=$QtDir;$PopplerDeps;$Prefix\libpng;$Prefix\openjpeg;$Zlib" `
			"-DZLIB_LIBRARY=$Zlib\lib\zlibstatic.lib" "-DZLIB_INCLUDE_DIR=$Zlib\include" `
			-DENABLE_QT6=ON -DENABLE_QT5=OFF -DENABLE_GLIB=OFF -DENABLE_CPP=OFF -DENABLE_UTILS=OFF -DENABLE_LIBCURL=OFF `
			-DENABLE_NSS3=OFF -DENABLE_GPGME=OFF -DENABLE_BOOST=OFF -DENABLE_LCMS=OFF -DENABLE_LIBTIFF=OFF `
			-DENABLE_LIBOPENJPEG=openjpeg2 -DENABLE_DCTDECODER=libjpeg -DFONT_CONFIGURATION=win32 `
			-DBUILD_GTK_TESTS=OFF -DBUILD_QT5_TESTS=OFF -DBUILD_QT6_TESTS=OFF -DBUILD_CPP_TESTS=OFF -DBUILD_MANUAL_TESTS=OFF `
			-DENABLE_GOBJECT_INTROSPECTION=OFF
		cmake --build "$src\build" --parallel
		cmake --install "$src\build"
	}
}

# The layout of set_path_autenticacao.gov.bat
$suffix = if ($Arch -eq 'x64') { '_X64' } else { '' }
$vars = [ordered]@{
	"PTEID_DIR_OPENSSL$suffix"  = "$Prefix\openssl"
	"PTEID_DIR_XERCES$suffix"   = "$Prefix\xerces"
	"PTEID_XMLSECURITY$suffix"  = "$Prefix\xmlsec"
	"PTEID_DIR_CURL$suffix"     = "$Prefix\curl"
	"PTEID_DIR_ZLIB$suffix"     = $Zlib
	"PTEID_DIR_LIBZIP$suffix"   = "$Prefix\libzip"
	"PTEID_DIR_LIBPNG$suffix"   = "$Prefix\libpng"
	"PTEID_DIR_OPENJPEG$suffix" = "$Prefix\openjpeg"
	"PTEID_DIR_CJSON$suffix"    = "$Prefix\cjson"
	'PTEID_DIR_OPEN_PACE'       = $OpenPace
}
if ($Arch -eq 'x64') { $vars['PTEID_POPPLER_QT6'] = "$Prefix\poppler" }

# Every library the Visual Studio projects link to must be where they look for it
$expected = @("openssl\lib\libcrypto.lib", "openssl\lib\libssl.lib", "xerces\lib\xerces-c_3.lib", "xmlsec\lib\xsec_lib.lib",
	"curl\lib\libcurl_a.lib", "zlib\lib\zlibstatic.lib", "libzip\lib\libzip.lib", "libpng\lib\libpng16_static.lib",
	"openjpeg\lib\openjp2.lib", "openjpeg\include\openjpeg-2.4\openjpeg.h", "cjson\lib\cjson.lib")
if ($Arch -eq 'x64') { $expected += @("poppler\lib\poppler-qt6.lib", "poppler\bin\poppler-qt6.dll", "poppler\bin\poppler.dll") }
$missing = $expected | Where-Object { -not (Test-Path (Join-Path $Prefix $_)) }
if (-not (Test-Path "$libDir\libeac.lib")) { $missing += "$libDir\libeac.lib" }
if ($missing) { throw "Missing after the build: $($missing -join ', ')" }

foreach ($name in $vars.Keys) {
	Write-Host "$name=$($vars[$name])"
	if ($env:GITHUB_ENV) { "$name=$($vars[$name])" | Out-File -Append -Encoding utf8 $env:GITHUB_ENV }
}
