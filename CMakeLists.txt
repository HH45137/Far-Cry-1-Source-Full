# ==============================================================================
#  Far Cry 1 / CryEngine 1 source - CMake build (Windows, Clang / clang-cl)
# ==============================================================================
#
#  The original source tree ships with Visual Studio 2003/2005 project files
#  (Game01.sln + *.vcproj) that only build with the ancient MSVC 7.1/8.0
#  compilers, 32 bit, /MD.  This CMake project reproduces the same set of
#  targets (engine DLLs, FarCry.exe, renderers, tools) so that the code can be
#  built with a modern toolchain.
#
#  Requirements
#    * Windows 10/11, x86 (Win32) target - the prebuilt third party libraries
#      (D3D9 SDK, Bink, Cg, Ogg/Vorbis, Ubi.com SDK) are 32 bit only.
#    * CMake >= 3.21
#    * LLVM/Clang for Windows (clang-cl.exe), version 16 or newer.
#
#  Quick start (see README-CMake.md for details)
#    cmake -S . -B build -G Ninja ^
#          -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl ^
#          -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32
#    cmake --build build
#
#  Or simply run build-clang.bat (Ninja) / build-clang-vs.bat (VS generator).
#
# ==============================================================================

cmake_minimum_required(VERSION 3.21)

# ------------------------------------------------------------------------------
# Resource compiler (must be selected before project() / enable_language(RC))
#
# With clang-cl CMake picks llvm-rc, which cannot parse a couple of legacy
# resource types used by this project (e.g. the "TOOLBAR 16, 15" syntax of the
# Lua debugger).  Use the Windows SDK rc.exe instead when clang is requested
# explicitly (Ninja / Makefile generators).  The Visual Studio generator uses
# the SDK resource compiler for the ClangCL toolset anyway.
# ------------------------------------------------------------------------------
if(WIN32 AND NOT CMAKE_GENERATOR MATCHES "Visual Studio" AND NOT DEFINED CMAKE_RC_COMPILER)
    if(DEFINED CMAKE_CXX_COMPILER AND CMAKE_CXX_COMPILER MATCHES "clang")
        file(GLOB _farcry_rc_candidates
            "$ENV{ProgramFiles\(x86\)}/Windows Kits/10/bin/*/x64/rc.exe"
            "$ENV{ProgramFiles\(x86\)}/Windows Kits/10/bin/*/x86/rc.exe")
        if(_farcry_rc_candidates)
            list(SORT _farcry_rc_candidates)
            list(GET _farcry_rc_candidates -1 _farcry_rc_exe)
            set(CMAKE_RC_COMPILER "${_farcry_rc_exe}" CACHE FILEPATH "Resource compiler" FORCE)
            message(STATUS "RC compiler: ${_farcry_rc_exe} (Windows SDK)")
        endif()
    endif()
endif()

project(FarCryEngine
    VERSION 1.0.0
    DESCRIPTION "Far Cry 1 (CryEngine 1.x) source port"
    LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ------------------------------------------------------------------------------
# Platform / toolchain checks
# ------------------------------------------------------------------------------
if(NOT WIN32)
    message(FATAL_ERROR "Far Cry 1 targets Windows only.")
endif()

if(NOT MSVC)
    message(FATAL_ERROR
        "Far Cry 1 requires an MSVC compatible toolchain (clang-cl or MSVC).\n"
        "The code relies on __declspec(dllexport), #pragma comment(lib), "
        "SEH and MSVC ABI details.")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR
        "This source tree must be built as 32 bit (Win32) because all prebuilt\n"
        "third party libraries (D3D9 SDK, Bink, Cg, Ogg/Vorbis, Ubi.com) are x86.\n"
        "  Visual Studio generator : cmake -A Win32 -T ClangCL ...\n"
        "  Ninja generator         : cmake -DCMAKE_C_FLAGS=-m32 "
        "-DCMAKE_CXX_FLAGS=-m32 ...")
endif()

message(STATUS "Compiler  : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Generator : ${CMAKE_GENERATOR}")

# ------------------------------------------------------------------------------
# Build options
# ------------------------------------------------------------------------------
option(FARCRY_BUILD_GAME             "Build the client game module (CryGame) and FarCry.exe" ON)
option(FARCRY_BUILD_DEDICATED_SERVER "Build the standalone server (FarCry_WinSV.exe)"      ON)
option(FARCRY_BUILD_RENDERER_D3D9    "Build the Direct3D 9 renderer (XRenderD3D9)"          ON)
option(FARCRY_BUILD_RENDERER_NULL    "Build the NULL (headless) renderer (XRenderNULL)"     ON)
option(FARCRY_BUILD_RENDERER_OGL     "Build the legacy OpenGL renderer (XRenderOGL)"        OFF)
option(FARCRY_BUILD_RESOURCE_COMPILER    "Build the resource compiler (rc.exe)"                 ON)
option(FARCRY_BUILD_RESOURCE_COMPILER_PC "Build the PC resource compiler plugin (ATL heavy, WIP)" OFF)
option(FARCRY_BUILD_EDITOR           "Build the game editor (requires MFC, XT library)"     OFF)
option(FARCRY_BUILD_AISYSTEM         "Build CryAISystem (source drop is incomplete)"        OFF)

# The Ubi.com / GameSpy SDK that ships with the source drop was built with
# STLport and an old CRT, so its prebuilt import libraries cannot be linked
# into a modern MSVC STL build.  Disabled by default; the master servers are
# offline anyway.  Enable at your own risk (the code paths are still there).
option(FARCRY_USE_UBICOM_SDK     "Enable the Ubi.com (GameSpy) master server SDK"     OFF)
option(FARCRY_USE_PUNKBUSTER_SDK "Enable the PunkBuster client/server SDK"            ON)
option(FARCRY_USE_ASE_SDK        "Enable the ASE query SDK (library is not included)" OFF)
option(FARCRY_USE_BINK_SDK       "Enable Bink video playback"                         ON)
option(FARCRY_USE_DIVX_SDK       "Enable the DivX video SDK"                         OFF)

# ------------------------------------------------------------------------------
# Output layout: everything ends up next to FarCry.exe, like the retail game
# ------------------------------------------------------------------------------
set(FARCRY_RUNTIME_OUTPUT_DIR "${CMAKE_BINARY_DIR}/Bin32" CACHE PATH
    "Directory that receives all .exe/.dll files")
set(FARCRY_ARCHIVE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/Lib32" CACHE PATH
    "Directory that receives import libraries")

include(FarCryEngine)

# ------------------------------------------------------------------------------
# Resource compilation with the Windows SDK rc.exe
#
# When the Windows SDK resource compiler is used together with clang-cl, CMake
# would go through its "cmake_llvm_rc" wrapper (which emits GNU style line
# markers that rc.exe cannot read) and rc.exe would not see the Windows SDK
# headers because clang finds them on its own.  Use the plain RC rule and feed
# the SDK include directories to rc.exe.
# ------------------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_RC_COMPILER MATCHES "Windows Kits")
    set(CMAKE_RC_COMPILE_OBJECT
        "<CMAKE_RC_COMPILER> <DEFINES> -I <SOURCE_DIR> <INCLUDES> <FLAGS> /fo <OBJECT> <SOURCE>")
    # rc.exe does not understand the clang dependency flags ("-clang:-MD ...")
    # and the Ninja "cmcldeps" scanner wrapper does not work with it either.
    set(CMAKE_DEPFILE_FLAGS_RC "")
    set(CMAKE_RC_DEPENDS "")
    set(CMAKE_NINJA_CMCLDEPS_RC 0)
    # clang finds the Windows SDK headers on its own, rc.exe does not - pass the
    # SDK include directories explicitly.
    file(GLOB _farcry_sdk_versions LIST_DIRECTORIES true
        "$ENV{ProgramFiles\(x86\)}/Windows Kits/10/Include/*")
    # Pick the newest SDK version that actually contains the "um" headers.
    set(_farcry_sdk_root "")
    foreach(_farcry_sdk_candidate ${_farcry_sdk_versions})
        if(IS_DIRECTORY "${_farcry_sdk_candidate}/um")
            if(NOT _farcry_sdk_root OR _farcry_sdk_candidate VERSION_GREATER _farcry_sdk_root)
                set(_farcry_sdk_root "${_farcry_sdk_candidate}")
            endif()
        endif()
    endforeach()
    if(_farcry_sdk_root)
        foreach(_farcry_sdk_sub um shared ucrt winrt)
            if(IS_DIRECTORY "${_farcry_sdk_root}/${_farcry_sdk_sub}")
                string(APPEND CMAKE_RC_FLAGS " -I\"${_farcry_sdk_root}/${_farcry_sdk_sub}\"")
            endif()
        endforeach()
    endif()
    message(STATUS "RC flags    : ${CMAKE_RC_FLAGS}")
endif()

add_subdirectory(CryFont)
add_subdirectory(CrySystem)
add_subdirectory(CryScriptSystem)
add_subdirectory(CryMovie)
add_subdirectory(CryInput)
add_subdirectory(CryEntitySystem)
add_subdirectory(CryAnimation)
add_subdirectory(CryPhysics)
add_subdirectory(Cry3DEngine)
add_subdirectory(CrySoundSystem)
add_subdirectory(CryNetwork)

if(FARCRY_BUILD_AISYSTEM)
    add_subdirectory(CryAISystem)
endif()

if(FARCRY_BUILD_RENDERER_D3D9)
    add_subdirectory(RenderDll/XRenderD3D9)
endif()

if(FARCRY_BUILD_RENDERER_NULL)
    add_subdirectory(RenderDll/XRenderNULL)
endif()

if(FARCRY_BUILD_RENDERER_OGL)
    add_subdirectory(RenderDll/XRenderOGL)
endif()

if(FARCRY_BUILD_GAME)
    add_subdirectory(CryGame)
    add_subdirectory(FARCRY)
endif()

if(FARCRY_BUILD_DEDICATED_SERVER)
    add_subdirectory(FarCry_WinSV)
endif()

if(FARCRY_BUILD_RESOURCE_COMPILER)
    add_subdirectory(ResourceCompiler)
endif()

if(FARCRY_BUILD_RESOURCE_COMPILER_PC)
    add_subdirectory(ResourceCompilerPC)
endif()

if(FARCRY_BUILD_EDITOR)
    add_subdirectory(Editor)
endif()

# ------------------------------------------------------------------------------
# Handy summary
# ------------------------------------------------------------------------------
message(STATUS "")
message(STATUS "Far Cry 1 build configuration")
message(STATUS "  Binaries     : ${FARCRY_RUNTIME_OUTPUT_DIR}")
message(STATUS "  Renderers    : D3D9=${FARCRY_BUILD_RENDERER_D3D9} NULL=${FARCRY_BUILD_RENDERER_NULL} OGL=${FARCRY_BUILD_RENDERER_OGL}")
message(STATUS "  Game         : ${FARCRY_BUILD_GAME}")
message(STATUS "  Ded. server  : ${FARCRY_BUILD_DEDICATED_SERVER}")
message(STATUS "  Editor       : ${FARCRY_BUILD_EDITOR}")
message(STATUS "  Ubi.com SDK  : ${FARCRY_USE_UBICOM_SDK}")
message(STATUS "  PunkBuster   : ${FARCRY_USE_PUNKBUSTER_SDK}")
message(STATUS "")
