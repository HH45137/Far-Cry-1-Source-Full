include("${FARCRY_ROOT}/cmake/sources/CrySystem.cmake")

farcry_add_engine_module(CrySystem
    SOURCES  ${CrySystem_SOURCES}
    DEFINES  _USRDLL CRYSYSTEM_EXPORTS
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}"
    LIBS     wininet.lib version.lib winmm.lib)
