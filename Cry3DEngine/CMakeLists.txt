include("${FARCRY_ROOT}/cmake/sources/Cry3DEngine.cmake")

farcry_add_engine_module(Cry3DEngine
    SOURCES  ${Cry3DEngine_SOURCES}
    DEFINES  _USRDLL CRY3DENGINE_EXPORTS
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}")
