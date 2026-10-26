include("${FARCRY_ROOT}/cmake/sources/CryMovie.cmake")

farcry_add_engine_module(CryMovie
    SOURCES  ${CryMovie_SOURCES}
    DEFINES  _USRDLL CRYMOVIE_EXPORTS
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}")
