@echo off
REM =============================================================================
REM  Far Cry 1 - build with clang-cl through the Visual Studio generator
REM  (Visual Studio "ClangCL" toolset, Win32 platform)
REM
REM  Usage:  build-clang-vs.bat [Release^|Debug]
REM =============================================================================
setlocal
set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=Release

set BUILD_DIR=build-vs

cmake -S . -B %BUILD_DIR% -G "Visual Studio 17 2022" -A Win32 -T ClangCL
if errorlevel 1 exit /b 1

cmake --build %BUILD_DIR% --config %CONFIG% --parallel
if errorlevel 1 exit /b 1

echo.
echo Build finished - binaries are in %BUILD_DIR%\Bin32
endlocal
