include("${FARCRY_ROOT}/cmake/sources/CryFont.cmake")

farcry_add_engine_module(CryFont
    SOURCES  ${CryFont_SOURCES}
    DEFINES  _USRDLL CRYFONT_EXPORTS
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}"
             "${CMAKE_CURRENT_SOURCE_DIR}/FreeType2/include")
