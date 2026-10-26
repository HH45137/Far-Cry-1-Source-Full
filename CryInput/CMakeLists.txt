include("${FARCRY_ROOT}/cmake/sources/CryInput.cmake")

farcry_add_engine_module(CryInput
    SOURCES  ${CryInput_SOURCES}
    DEFINES  _USRDLL CRYINPUT_EXPORTS
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}"
    LIBS     dinput8.lib dxguid.lib winmm.lib)
