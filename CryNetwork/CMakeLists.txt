include("${FARCRY_ROOT}/cmake/sources/CryNetwork.cmake")

# The Ubi.com (GameSpy) SDK is pulled in with #pragma comment(lib, ...)
# directives, so only the library search paths have to be provided.
set(CRYNETWORK_LIBS      winmm.lib wininet.lib wsock32.lib ws2_32.lib)
set(CRYNETWORK_LIB_DIRS  "")
set(CRYNETWORK_INCLUDES  "${CMAKE_CURRENT_SOURCE_DIR}")

if(FARCRY_USE_UBICOM_SDK)
    foreach(sdk gs-sdk-base gs-sdk-common gs-sdk-msclient gs-sdk-regserver gs-sdk-cdkey)
        list(APPEND CRYNETWORK_LIB_DIRS "${FARCRY_UBICOM_SDK_DIR}/${sdk}/lib_win32")
        list(APPEND CRYNETWORK_INCLUDES "${FARCRY_UBICOM_SDK_DIR}/${sdk}/include")
    endforeach()
endif()

farcry_add_engine_module(CryNetwork
    SOURCES  ${CryNetwork_SOURCES}
    DEFINES  _USRDLL CRYNETWORK_EXPORTS
    INCLUDES ${CRYNETWORK_INCLUDES}
    LIBS     ${CRYNETWORK_LIBS}
    LIB_DIRS ${CRYNETWORK_LIB_DIRS})
