include("${FARCRY_ROOT}/cmake/sources/CrySoundSystem.cmake")

farcry_add_engine_module(CrySoundSystem
    SOURCES  ${CrySoundSystem_SOURCES}
    DEFINES  _USRDLL CRYSOUNDSYSTEM_EXPORTS
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}"
             "${CMAKE_CURRENT_SOURCE_DIR}/OggVorbisInclude"
    LIBS     crysound.lib ogg_static.lib vorbis_static.lib vorbisfile_static.lib
    LIB_DIRS "${CMAKE_CURRENT_SOURCE_DIR}")
