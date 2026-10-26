include("${FARCRY_ROOT}/cmake/sources/CryAnimation.cmake")

# The original project defined COMP_VC, DO_ASM, OS_WIN32 and PROC_INTEL.  The
# DO_ASM switch enables hand written inline assembly, which clang-cl does not
# support, so it is intentionally not defined here.
farcry_add_engine_module(CryAnimation
    SOURCES  ${CryAnimation_SOURCES}
    DEFINES  _USRDLL CRYANIMATION_EXPORTS COMP_VC OS_WIN32
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}"
    LIBS     glu32.lib winmm.lib odbc32.lib odbccp32.lib)
