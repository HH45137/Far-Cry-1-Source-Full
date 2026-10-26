include("${FARCRY_ROOT}/cmake/sources/CryPhysics.cmake")

farcry_add_engine_module(CryPhysics
    SOURCES  ${CryPhysics_SOURCES}
    DEFINES  _USRDLL PHYSICS_EXPORTS PIII_SSE
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}"
    LIBS     odbc32.lib odbccp32.lib)
