include("${FARCRY_ROOT}/cmake/sources/XRenderOGL.cmake")

farcry_add_engine_module(XRenderOGL
    SOURCES  ${XRenderOGL_SOURCES}
    DEFINES  _USRDLL XRENDEROGL_EXPORTS _RENDERER OPENGL COMP_VC OS_WIN32 PROC_INTEL
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}"
             "${CMAKE_CURRENT_SOURCE_DIR}/CG"
             "${FARCRY_ROOT}/RenderDll"
             "${FARCRY_ROOT}"
             "${FARCRY_ROOT}/CryPhysics"
             "${FARCRY_ROOT}/CryNetwork"
    LIBS     glu32.lib opengl32.lib winmm.lib cg.lib cgGL.lib ijl15.lib nvDXTlib.lib
    LIB_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Libraries")

# nvDXTlib.lib is a VC6 era static library, see XRenderNULL/CMakeLists.txt
target_link_options(XRenderOGL PRIVATE /NODEFAULTLIB:LIBC /NODEFAULTLIB:libcp)
target_link_libraries(XRenderOGL PRIVATE legacy_stdio_definitions.lib)
target_sources(XRenderOGL PRIVATE "${FARCRY_ROOT}/cmake/compat/atl_crt_shim.cpp")
