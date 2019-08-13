:: If DEBUG=1 open project in VS,
:: if      =0 just build it all
@if "%DEBUG%" == "" (
	echo Variable DEBUG is NOT defined. Set DEBUG mode on.
	@set DEBUG=1
) 

:: Output Build log to console
@if "%OUTPUTLOG%" == "" (
	echo Output Build log to console is NOT defined. Set OUTPUTLOG on.
	@set OUTPUTLOG=1
) 

:: MW version
@set MW_VERSION=3

:: If SKIP_X64_DEPS_CHECK=1 if set will skip cheking %<DEPS>_X64% to see if they are in place.
:: Useful if for just compiling and creating the x86 targets
@set SKIP_X64_DEPS_CHECK=0

echo DEBUG=%DEBUG%
echo OUTPUTLOG=%OUTPUTLOG%
echo MW_VERSION=%MW_VERSION%
echo SKIP_X64_DEPS_CHECK=%SKIP_X64_DEPS_CHECK%

@call "%~dp0make_win_vc9.bat"