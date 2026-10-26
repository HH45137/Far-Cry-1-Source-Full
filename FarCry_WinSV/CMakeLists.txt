include("${FARCRY_ROOT}/cmake/sources/FarCry_WinSV.cmake")

farcry_add_engine_executable(FarCry_WinSV
    SOURCES    ${FarCry_WinSV_SOURCES}
    DEFINES    _CONSOLE
    INCLUDES   "${CMAKE_CURRENT_SOURCE_DIR}"
    SUBSYSTEM  CONSOLE)
