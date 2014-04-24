:: If DEBUG=1 open project in VS,
:: if      =0 just build it all
@set DEBUG=1

:: If SKIP_X64_DEPS_CHECK=1 if set will skip cheking %<DEPS>_X64% to see if they are in place.
:: Useful if for just compiling and creating the x86 targets
::@set SKIP_X64_DEPS_CHECK=1

@call "%~dp0make_win_vc9.bat"
