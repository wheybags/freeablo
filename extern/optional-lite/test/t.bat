@echo off & setlocal enableextensions enabledelayedexpansion
::
:: t.bat - compile & run tests (MSVC).
::

set unit=optional

:: if no std is given, use compiler default

set std=%1
if not "%std%"=="" set std=-std:%std%

call :CompilerVersion version
echo VC%version%: %args%

set UCAP=%unit%
call :toupper UCAP

set unit_select=-D%unit%_CONFIG_SELECT_%UCAP%=%unit%_%UCAP%_DEFAULT
::set unit_select=-D%unit%_CONFIG_SELECT_%UCAP%=%unit%_%UCAP%_NONSTD
::set unit_select=-D%unit%_CONFIG_SELECT_%UCAP%=%unit%_%UCAP%_STD

set unit_config=

set msvc_defines=^
    -D_CRT_SECURE_NO_WARNINGS ^
    -D_SCL_SECURE_NO_WARNINGS

set CppCoreCheckInclude=%VCINSTALLDIR%\Auxiliary\VS\include

cl -W3 -EHsc %std% %unit_select% %unit_config% %msvc_defines% -I../include %unit%-main.t.cpp %unit%.t.cpp && %unit%-main.t.exe
endlocal & goto :EOF

:: subroutines:

:CompilerVersion  version
@echo off & setlocal enableextensions
set tmpprogram=_getcompilerversion.tmp
set tmpsource=%tmpprogram%.c

echo #include ^<stdio.h^>                   >%tmpsource%
echo int main(){printf("%%d\n",_MSC_VER);} >>%tmpsource%

cl /nologo %tmpsource% >nul
for /f %%x in ('%tmpprogram%') do set version=%%x
del %tmpprogram%.* >nul
set offset=0
if %version% LSS 1900 set /a offset=1
set /a version="version / 10 - 10 * ( 5 + offset )"
endlocal & set %1=%version%& goto :EOF

:: toupper; makes use of the fact that string
:: replacement (via SET) is not case sensitive
:toupper
for %%L IN (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z) DO SET %1=!%1:%%L=%%L!
goto :EOF
