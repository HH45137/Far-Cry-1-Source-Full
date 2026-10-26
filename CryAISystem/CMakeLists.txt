# ------------------------------------------------------------------------------
# CryAISystem
#
# The published source drop only contains a fraction of the AI module (the
# behaviour/agent implementation files referenced by the original vcproj are
# not part of the tree), therefore this target is off by default.  It is
# provided so the module can be completed later on.
# ------------------------------------------------------------------------------
include("${FARCRY_ROOT}/cmake/sources/CryAISystem.cmake")

farcry_add_engine_module(CryAISystem
    SOURCES  ${CryAISystem_SOURCES}
    DEFINES  _USRDLL CRYAISYSTEM_EXPORTS
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}")
