include("${FARCRY_ROOT}/cmake/sources/XRenderNULL.cmake")

farcry_add_engine_module(XRenderNULL
    SOURCES  ${XRenderNULL_SOURCES}
    DEFINES  _USRDLL NULL_RENDERER _RENDERER COMP_VC OS_WIN32 PROC_INTEL
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}"
             "${FARCRY_ROOT}/RenderDll"
    LIBS     ijl15.lib nvDXTlib.lib
    LIB_DIRS "${FARCRY_ROOT}/RenderDll/XRenderD3D9/Libraries")

# nvDXTlib.lib is a VC6 era static library that still carries /DEFAULTLIB:LIBC
# and /DEFAULTLIB:libcp directives for the pre-UCRT static runtime.  The legacy
# stdio definitions and the ATL shim provide the few symbols it references.
target_link_options(XRenderNULL PRIVATE /NODEFAULTLIB:LIBC /NODEFAULTLIB:libcp)
target_link_libraries(XRenderNULL PRIVATE legacy_stdio_definitions.lib)
target_sources(XRenderNULL PRIVATE "${FARCRY_ROOT}/cmake/compat/atl_crt_shim.cpp")
