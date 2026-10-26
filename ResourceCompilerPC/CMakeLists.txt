include("${FARCRY_ROOT}/cmake/sources/ResourceCompilerPC.cmake")

farcry_add_engine_module(ResourceCompilerPC
    SOURCES  ${ResourceCompilerPC_SOURCES}
             "${CMAKE_CURRENT_SOURCE_DIR}/ResourceCompilerPlugin.def"
    DEFINES  _USRDLL RESOURCECOMPILERPC_EXPORTS
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}"
             "${FARCRY_ROOT}/ResourceCompiler"
    LIBS     dbghelp.lib)
