include("${FARCRY_ROOT}/cmake/sources/CryGame.cmake")

set(CRYGAME_INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${FARCRY_BINK_DIR}")

set(CRYGAME_LIB_DIRS
    "${FARCRY_BINK_DIR}")

if(FARCRY_USE_UBICOM_SDK)
    foreach(sdk gs-sdk-base gs-sdk-common gs-sdk-msclient gs-sdk-regserver gs-sdk-cdkey)
        list(APPEND CRYGAME_INCLUDES "${FARCRY_UBICOM_SDK_DIR}/${sdk}/include")
        list(APPEND CRYGAME_LIB_DIRS "${FARCRY_UBICOM_SDK_DIR}/${sdk}/lib_win32")
    endforeach()
endif()

farcry_add_engine_module(CryGame
    SOURCES  ${CryGame_SOURCES}
    DEFINES  _USRDLL CRYGAME_EXPORTS USE_GAME_DLL
    INCLUDES ${CRYGAME_INCLUDES}
    LIBS     shlwapi.lib wininet.lib ws2_32.lib binkw32.lib
    LIB_DIRS ${CRYGAME_LIB_DIRS})
