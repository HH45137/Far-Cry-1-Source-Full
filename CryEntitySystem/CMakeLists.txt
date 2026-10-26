include("${FARCRY_ROOT}/cmake/sources/CryEntitySystem.cmake")

farcry_add_engine_module(CryEntitySystem
    SOURCES  ${CryEntitySystem_SOURCES}
    DEFINES  _USRDLL CRYENTITYDLL_EXPORTS
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}"
             "${FARCRY_ROOT}"
             "${FARCRY_ROOT}/Cry3DEngine"
             "${FARCRY_ROOT}/CryPhysics")
