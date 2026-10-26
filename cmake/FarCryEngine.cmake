# ==============================================================================
#  FarCryEngine.cmake
#  Shared build settings and helper functions for the Far Cry 1 CMake build.
# ==============================================================================

include_guard(GLOBAL)

get_filename_component(FARCRY_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

# ------------------------------------------------------------------------------
# Third party SDK locations (all of them are part of the source drop)
# ------------------------------------------------------------------------------
set(FARCRY_BINK_DIR       "${FARCRY_ROOT}/BinkSDK"                     CACHE PATH "Bink SDK directory")
set(FARCRY_D3D9_SDK_DIR   "${FARCRY_ROOT}/RenderDll/Dx9Sdk"            CACHE PATH "DirectX 9 SDK directory")
set(FARCRY_UBICOM_SDK_DIR "${FARCRY_ROOT}/Ubisoft.com/GSServices/sdks" CACHE PATH "Ubi.com GameSpy SDK directory")

# The vintage code base is compiled with "/MD" like the retail binaries.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

# ------------------------------------------------------------------------------
# farcry_common : interface target with the defines/options every module needs
# ------------------------------------------------------------------------------
add_library(farcry_common INTERFACE)

target_include_directories(farcry_common INTERFACE
    "${FARCRY_ROOT}/CryCommon"
    "${FARCRY_ROOT}/cmake/compat")

target_compile_definitions(farcry_common INTERFACE
    WIN32
    _WINDOWS
    _CRT_SECURE_NO_WARNINGS
    _CRT_SECURE_NO_DEPRECATE
    _CRT_NONSTDC_NO_WARNINGS
    _WIN32_WINNT=0x0501
    WINVER=0x0501
    $<$<NOT:$<BOOL:${FARCRY_USE_BINK_SDK}>>:NOT_USE_BINK_SDK>
    $<$<NOT:$<BOOL:${FARCRY_USE_DIVX_SDK}>>:NOT_USE_DIVX_SDK>
    $<$<NOT:$<BOOL:${FARCRY_USE_UBICOM_SDK}>>:NOT_USE_UBICOM_SDK>
    $<$<NOT:$<BOOL:${FARCRY_USE_PUNKBUSTER_SDK}>>:NOT_USE_PUNKBUSTER_SDK>
    $<$<NOT:$<BOOL:${FARCRY_USE_ASE_SDK}>>:NOT_USE_ASE_SDK>)

# ------------------------------------------------------------------------------
# Compiler options
# ------------------------------------------------------------------------------
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # The code contains large amounts of MSVC inline assembly (__asm {}) which
    # clang-cl does not support.  Every assembly block is guarded by
    # CRY_NO_INLINE_ASM and falls back to portable C/C++ code.
    target_compile_definitions(farcry_common INTERFACE CRY_NO_INLINE_ASM=1)
endif()

target_compile_options(farcry_common INTERFACE
    "$<$<COMPILE_LANGUAGE:C,CXX>:/W0>"
    "$<$<COMPILE_LANGUAGE:C,CXX>:/bigobj>"
    "$<$<COMPILE_LANGUAGE:C,CXX>:-Wno-c++11-narrowing>"
    # The engine passes "&SomeStruct(...)" as a function argument in many places;
    # the pointer is only used inside the same full expression, so this is safe.
    "$<$<COMPILE_LANGUAGE:C,CXX>:-Wno-address-of-temporary>"
    "$<$<COMPILE_LANGUAGE:C,CXX>:/EHsc>"
    "$<$<COMPILE_LANGUAGE:C,CXX>:/std:c++14>"
    "$<$<COMPILE_LANGUAGE:C,CXX>:/FI${FARCRY_ROOT}/cmake/compat/cry_stl_compat.h>"
    "$<$<COMPILE_LANGUAGE:C,CXX>:/Oy>"
    "$<$<COMPILE_LANGUAGE:C,CXX>:/GF>"
    "$<$<COMPILE_LANGUAGE:C,CXX>:$<$<CONFIG:Release>:/O2>>"
    "$<$<COMPILE_LANGUAGE:C,CXX>:$<$<CONFIG:RelWithDebInfo>:/O2>>"
    "$<$<COMPILE_LANGUAGE:C,CXX>:$<$<CONFIG:MinSizeRel>:/O1>>"
    "$<$<COMPILE_LANGUAGE:C>:-Wno-implicit-function-declaration>"
    "$<$<COMPILE_LANGUAGE:C>:-Wno-implicit-int>"
    "$<$<COMPILE_LANGUAGE:C>:-Wno-int-conversion>"
    "$<$<COMPILE_LANGUAGE:C>:-Wno-incompatible-function-pointer-types>"
    "$<$<COMPILE_LANGUAGE:C>:-Wno-incompatible-pointer-types>"
    "$<$<COMPILE_LANGUAGE:C>:-Wno-return-type>")

# ------------------------------------------------------------------------------
# farcry_add_engine_module(<name> SOURCES ... [DEFINES ...] [INCLUDES ...]
#                          [LIBS ...] [LIB_DIRS ...] [TYPE SHARED|STATIC])
#
# Creates one of the engine DLLs (CrySystem, Cry3DEngine, ...).  They are all
# shared libraries that CrySystem loads dynamically at runtime.
# ------------------------------------------------------------------------------
function(farcry_add_engine_module NAME)
    cmake_parse_arguments(MOD "" "TYPE" "SOURCES;DEFINES;INCLUDES;LIBS;LIB_DIRS" ${ARGN})

    if(NOT MOD_TYPE)
        set(MOD_TYPE SHARED)
    endif()

    add_library(${NAME} ${MOD_TYPE} ${MOD_SOURCES})
    target_link_libraries(${NAME} PRIVATE farcry_common)

    if(MOD_DEFINES)
        target_compile_definitions(${NAME} PRIVATE ${MOD_DEFINES})
    endif()

    if(MOD_INCLUDES)
        target_include_directories(${NAME} PRIVATE ${MOD_INCLUDES})
    endif()

    if(MOD_LIB_DIRS)
        target_link_directories(${NAME} PRIVATE ${MOD_LIB_DIRS})
    endif()

    if(MOD_LIBS)
        target_link_libraries(${NAME} PRIVATE ${MOD_LIBS})
    endif()

    set_target_properties(${NAME} PROPERTIES
        PREFIX ""
        OUTPUT_NAME "${NAME}"
        RUNTIME_OUTPUT_DIRECTORY "${FARCRY_RUNTIME_OUTPUT_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${FARCRY_ARCHIVE_OUTPUT_DIR}")

    # The prebuilt third party import libraries (ijl15.dll, ...) are not
    # SEH compatible and the original binaries did not use /SAFESEH either.
    target_link_options(${NAME} PRIVATE /SAFESEH:NO)

    # Multi config generators append a per-configuration sub directory unless
    # the output directory is overridden for every configuration.
    foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER "${cfg}" cfg_upper)
        set_target_properties(${NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${cfg_upper} "${FARCRY_RUNTIME_OUTPUT_DIR}"
            ARCHIVE_OUTPUT_DIRECTORY_${cfg_upper} "${FARCRY_ARCHIVE_OUTPUT_DIR}")
    endforeach()
endfunction()


# ------------------------------------------------------------------------------
# farcry_add_engine_executable(<name> SOURCES ... [SUBSYSTEM WINDOWS|CONSOLE])
# ------------------------------------------------------------------------------
function(farcry_add_engine_executable NAME)
    cmake_parse_arguments(APP "" "TYPE;SUBSYSTEM" "SOURCES;DEFINES;INCLUDES;LIBS;LIB_DIRS" ${ARGN})

    add_executable(${NAME} ${APP_SOURCES})
    target_link_libraries(${NAME} PRIVATE farcry_common)

    if(APP_DEFINES)
        target_compile_definitions(${NAME} PRIVATE ${APP_DEFINES})
    endif()

    if(APP_INCLUDES)
        target_include_directories(${NAME} PRIVATE ${APP_INCLUDES})
    endif()

    if(APP_LIB_DIRS)
        target_link_directories(${NAME} PRIVATE ${APP_LIB_DIRS})
    endif()

    if(APP_LIBS)
        target_link_libraries(${NAME} PRIVATE ${APP_LIBS})
    endif()

    if(APP_SUBSYSTEM)
        target_link_options(${NAME} PRIVATE "/SUBSYSTEM:${APP_SUBSYSTEM}")
    endif()

    set_target_properties(${NAME} PROPERTIES
        OUTPUT_NAME "${NAME}"
        RUNTIME_OUTPUT_DIRECTORY "${FARCRY_RUNTIME_OUTPUT_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${FARCRY_ARCHIVE_OUTPUT_DIR}")

    # see farcry_add_engine_module()
    target_link_options(${NAME} PRIVATE /SAFESEH:NO)

    foreach(cfg ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER "${cfg}" cfg_upper)
        set_target_properties(${NAME} PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${cfg_upper} "${FARCRY_RUNTIME_OUTPUT_DIR}"
            ARCHIVE_OUTPUT_DIRECTORY_${cfg_upper} "${FARCRY_ARCHIVE_OUTPUT_DIR}")
    endforeach()
endfunction()

