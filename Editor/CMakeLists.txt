# ------------------------------------------------------------------------------
# Editor.exe (MFC based level editor)
#
# Off by default.  It requires the shared MFC runtime and the prebuilt XT
# toolkit from Editor/XT/lib.  Build with -DFARCRY_BUILD_EDITOR=ON and a
# toolchain that ships MFC (Visual Studio "MFC for latest v143 build tools").
# ------------------------------------------------------------------------------
include("${FARCRY_ROOT}/cmake/sources/Editor.cmake")

if(NOT CMAKE_MFC_FLAG)
    set(CMAKE_MFC_FLAG 2) # shared MFC DLL, matches UseOfMFC="2"
endif()

farcry_add_engine_executable(Editor
    SOURCES    ${Editor_SOURCES}
    DEFINES    _XT_STATICLINK _AFXDLL
    INCLUDES   "${CMAKE_CURRENT_SOURCE_DIR}"
               "${FARCRY_ROOT}"
               "${CMAKE_CURRENT_SOURCE_DIR}/Include"
               "${CMAKE_CURRENT_SOURCE_DIR}/XT/Include"
    LIBS       XT3100LibDynStatic.lib IJL15.LIB nvDXTlib.lib
               winmm.lib version.lib comctl32.lib shlwapi.lib ws2_32.lib
    LIB_DIRS   "${CMAKE_CURRENT_SOURCE_DIR}/XT/lib"
               "${CMAKE_CURRENT_SOURCE_DIR}/Lib"
    SUBSYSTEM  WINDOWS)
