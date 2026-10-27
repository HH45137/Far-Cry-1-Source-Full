@echo off
REM =============================================================================
REM  Far Cry 1 - build with clang-cl (Ninja generator)
REM
REM  Usage:  build-clang.bat [Release^|Debug] [optimize]
REM
REM  Compiler optimisations are disabled by default (FARCRY_NO_OPTIMIZATION=ON,
REM  see CMakeLists.txt).  Pass "optimize" as the second argument to build the
REM  regular /O2 binaries that can be shipped.
REM =============================================================================
setlocal
set CONFIG=%1
if "%CONFIG%"=="" set CONFIG=Debug

set NOOPT=ON
if /I "%2"=="optimize" set NOOPT=OFF

set BUILD_DIR=build-clang

where clang-cl.exe >nul 2>nul
if errorlevel 1 (
    echo [ERROR] clang-cl.exe was not found in PATH.
    echo         Install LLVM/Clang for Windows and add its bin directory to PATH.
    exit /b 1
)

rem clang-cl auto-detects the MSVC toolset and Windows SDK via the registry, so
rem no vcvars environment is required.  (Calling vcvars makes CMake enable its
rem "cmcldeps" dependency scanner for .rc files, which conflicts with the
rem Windows SDK rc.exe we use instead of llvm-rc.)

cmake -S . -B %BUILD_DIR% -G Ninja ^
      -DCMAKE_BUILD_TYPE=%CONFIG% ^
      -DCMAKE_C_COMPILER=clang-cl ^
      -DCMAKE_CXX_COMPILER=clang-cl ^
      -DFARCRY_BUILD_RENDERER_OGL=ON ^
      -DFARCRY_NO_OPTIMIZATION=%NOOPT% ^
      -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32
if errorlevel 1 exit /b 1

cmake --build %BUILD_DIR% --parallel
if errorlevel 1 exit /b 1

echo.
echo Build finished - binaries are in %BUILD_DIR%\Bin32
endlocal
