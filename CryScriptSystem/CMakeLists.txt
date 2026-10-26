include("${FARCRY_ROOT}/cmake/sources/CryScriptSystem.cmake")

farcry_add_engine_module(CryScriptSystem
    SOURCES  ${CryScriptSystem_SOURCES}
    DEFINES  _USRDLL CRYSCRIPTSYSTEM_EXPORTS
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}"
             "${CMAKE_CURRENT_SOURCE_DIR}/LUA"
    LIBS     odbc32.lib odbccp32.lib)
