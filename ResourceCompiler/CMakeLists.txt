include("${FARCRY_ROOT}/cmake/sources/ResourceCompiler.cmake")

# The tool is called "rc.exe" in the original projects.
farcry_add_engine_executable(ResourceCompiler
    SOURCES    ${ResourceCompiler_SOURCES}
    DEFINES    _CONSOLE
    INCLUDES   "${CMAKE_CURRENT_SOURCE_DIR}"
    LIBS       dbghelp.lib
    SUBSYSTEM  CONSOLE)

set_target_properties(ResourceCompiler PROPERTIES OUTPUT_NAME "rc")
