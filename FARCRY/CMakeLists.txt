include("${FARCRY_ROOT}/cmake/sources/FarCry.cmake")

# FarCry.exe is a tiny launcher: it loads CrySystem.dll dynamically and hands
# over to the engine, so it does not link against any engine import library.
farcry_add_engine_executable(FarCry
    SOURCES    ${FarCry_SOURCES}
    DEFINES    _WINDOWS
    INCLUDES   "${CMAKE_CURRENT_SOURCE_DIR}"
    LIBS       odbc32.lib odbccp32.lib
    SUBSYSTEM  WINDOWS)
