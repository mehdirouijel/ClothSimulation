@echo off
REM ==========================================================================================
REM Project Name  : ZelosEngine
REM File Name     : build.bat
REM
REM Creation Date : 09/24/2017
REM Last Modified : 03/12/2017 - 05:12
REM ==========================================================================================
REM Description   : Build configuration
REM
REM Author        : Mehdi Rouijel
REM ========================================================================================== */

@echo.

set CompilerFlags=-nologo -MDd -Gm- -GR- -EHsc -Oi- -WX -W4 -wd4100 -wd4189 -wd4201 -Z7
set IncludesPath="..\Includes"
set LibrariesPath=..\Libs
set AdditionalLibs=user32.lib gdi32.lib opengl32.lib shell32.lib glfw3.lib
set LinkerFlags=-OPT:REF -LIBPATH:%LibrariesPath% %AdditionalLibs%


mkdir ..\Build 2> nul
pushd ..\Build

@echo Compilation started on - %date% - %time:~0,2%:%time:~3,2%,%time:~6,2% -
@echo.
cl %CompilerFlags% -c ..\Sources\glad.c /I %IncludesPath%
cl %CompilerFlags% -c ..\Sources\*.cpp /I %IncludesPath%
echo.
cl %CompilerFlags% *.obj /link %LinkerFlags% /OUT:ZelosEngine.exe
@echo.
@echo Compilation finished on - %date% - %time:~0,2%:%time:~3,2%,%time:~6,2% -

popd
