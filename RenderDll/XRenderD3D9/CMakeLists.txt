include("${FARCRY_ROOT}/cmake/sources/XRenderD3D9.cmake")

farcry_add_engine_module(XRenderD3D9
    SOURCES  ${XRenderD3D9_SOURCES}
    DEFINES  _USRDLL XRENDERD3D9_EXPORTS _RENDERER DIRECT3D9 COMP_VC OS_WIN32 PROC_INTEL
    INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}"
             "${CMAKE_CURRENT_SOURCE_DIR}/CG"
             "${FARCRY_ROOT}/RenderDll"
             "${FARCRY_D3D9_SDK_DIR}/Include"
    LIBS     winmm.lib version.lib d3dx9.lib d3d9.lib dxerr9.lib cg.lib cgD3D9.lib
             ijl15.lib nvDXTlib.lib
    LIB_DIRS "${FARCRY_D3D9_SDK_DIR}/Lib"
             "${CMAKE_CURRENT_SOURCE_DIR}/Libraries")

# nvDXTlib.lib is a VC6 era static library, see XRenderNULL/CMakeLists.txt
target_link_options(XRenderD3D9 PRIVATE /NODEFAULTLIB:LIBC /NODEFAULTLIB:libcp)
target_link_libraries(XRenderD3D9 PRIVATE legacy_stdio_definitions.lib)
target_sources(XRenderD3D9 PRIVATE "${FARCRY_ROOT}/cmake/compat/atl_crt_shim.cpp")
