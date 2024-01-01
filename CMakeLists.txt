#############################################################################
#Copyright (c) 2023,                                          
#                                                                         
#Distributed under the terms of the MIT license.                 
#                                                                         
#The full license is in the file LICENSE, distributed with this software. 
#############################################################################

cmake_minimum_required(VERSION 3.4.3)
project(xeus-karilang)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

set(KARILANG_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Flex/Bison Build
find_package(BISON 3.0 REQUIRED)

bison_target(PARSER "src/KariLang/src/parser.y" "src/KariLang/src/parser.tab.c" DEFINES_FILE "src/KariLang/src/parser.tab.h" COMPILE_FLAGS "-Wall -Wcounterexamples")

find_package(FLEX 2.0 REQUIRED)

flex_target(LEXER "src/KariLang/src/lexer.l" "src/KariLang/src/lex.yy.c")

# Versionning
# ===========

file(STRINGS "${KARILANG_INCLUDE_DIR}/src/config.hpp" version_defines
     REGEX "#define KARILANG_VERSION_(MAJOR|MINOR|PATCH)")
foreach (ver ${version_defines})
    if (ver MATCHES "#define KARILANG_VERSION_(MAJOR|MINOR|PATCH) +([^ ]+)$")
        set(KARILANG_VERSION_${CMAKE_MATCH_1} "${CMAKE_MATCH_2}" CACHE INTERNAL "")
    endif ()
endforeach ()
set(${PROJECT_NAME}_VERSION
    ${KARILANG_VERSION_MAJOR}.${KARILANG_VERSION_MINOR}.${KARILANG_VERSION_PATCH})
message(STATUS "Building xeus-karilang v${${PROJECT_NAME}_VERSION}")

# Configuration
# =============

include(GNUInstallDirs)

if (NOT DEFINED KARILANG_KERNELSPEC_PATH)
    set(KARILANG_KERNELSPEC_PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/")
endif ()

configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xkarilang/kernel.json.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/xkarilang/kernel.json"
)

# Build options
# =============

option(KARILANG_BUILD_STATIC "Build xeus-karilang static library" ON)
OPTION(KARILANG_BUILD_SHARED "Split xkarilang build into executable and library" ON)
OPTION(KARILANG_BUILD_EXECUTABLE "Build the xkarilang executable" ON)

OPTION(KARILANG_USE_SHARED_XEUS "Link xkarilang  with the xeus shared library (instead of the static library)" ON)
OPTION(KARILANG_USE_SHARED_KARILANG "Link xkarilang  with the xeus shared library (instead of the static library)" ON)


if(EMSCRIPTEN)
    add_compile_definitions(KARILANG_EMSCRIPTEN_WASM_BUILD)
    message("Build with emscripten")
    SET(KARILANG_BUILD_STATIC ON)
    SET(KARILANG_BUILD_SHARED OFF)
    SET(KARILANG_BUILD_EXECUTABLE OFF)
    SET(KARILANG_USE_SHARED_XEUS OFF)
    SET(KARILANG_USE_SHARED_KARILANG OFF)
    SET(KARILANG_BUILD_TESTS OFF)
endif()


# Dependencies
# ============

set(xtl_REQUIRED_VERSION 0.7.0)
set(xeus_REQUIRED_VERSION 3.0.0)

if (NOT TARGET xtl)
    find_package(xtl ${xtl_REQUIRED_VERSION} REQUIRED)
endif ()
if (NOT TARGET xeus AND NOT TARGET xeus-static)
    find_package(xeus ${xeus_REQUIRED_VERSION} REQUIRED)
endif ()

# Flags
# =====
include(CheckCXXCompilerFlag)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4251 /wd4141")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4018 /wd4267 /wd4715 /wd4146 /wd4129")
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    if(NOT EMSCRIPTEN)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-parameter -Wextra -Wreorder")
    endif()
    

    # CHECK_CXX_COMPILER_FLAG("-std=c++17" HAS_CPP_17_FLAG)
    # if (HAS_CPP_17_FLAG)
    #     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    # else ()
    #     message(FATAL_ERROR "Unsupported compiler -- xeus requires C++17 support!")
    # endif ()
endif ()

if(EMSCRIPTEN)
    # ENV (https://github.com/emscripten-core/emscripten/commit/6d9681ad04f60b41ef6345ab06c29bbc9eeb84e0)
    set(EMSCRIPTEN_FEATURES "${EMSCRIPTEN_FEATURES} -s \"EXTRA_EXPORTED_RUNTIME_METHODS=[ENV']\"")
endif()


# Building KariLang as Static Library
add_library(KariLang_language STATIC 
    src/main.c
    src/KariLang/src/semantics.c
    src/KariLang/src/interpreter.c
    src/KariLang/src/parser.tab.c
    src/KariLang/src/lex.yy.c
)

# Source files
# ============

set(KARILANG_HEADERS
    include/src/config.hpp
    include/src/xinterpreter.hpp
)

set(KARILANG_SRC
    src/xinterpreter.cpp
    src/utils.cpp
)

set(KARILANG_MAIN_SRC
    src/main.cpp
)


# Targets and link - Macros
# =========================

include(CheckCXXCompilerFlag)

string(TOUPPER "${CMAKE_BUILD_TYPE}" U_CMAKE_BUILD_TYPE)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib; ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")

macro(karilang_set_common_options target_name)
    if (MSVC)
        target_compile_options(${target_name} PUBLIC /wd4251 /wd4141)
        target_compile_options(${target_name} PUBLIC /wd4018 /wd4267 /wd4715 /wd4146 /wd4129)
    endif ()

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
        CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR
        CMAKE_CXX_COMPILER_ID MATCHES "Intel")

        target_compile_options(${target_name} PUBLIC -Wunused-parameter -Wextra -Wreorder)

        # C++17 flag
        # CHECK_CXX_COMPILER_FLAG("-std=c++17" HAS_CPP_17_FLAG)
        # if (HAS_CPP_17_FLAG)
        #     target_compile_features(${target_name} PRIVATE cxx_std_17)
        # else ()
        #     message(FATAL_ERROR "Unsupported compiler -- xeus-karilang requires C++17 support!")
        # endif ()
    endif ()

    if (APPLE)
        set_target_properties(${target_name} PROPERTIES
            MACOSX_RPATH ON
        )
    else ()
        set_target_properties(${target_name} PROPERTIES
            BUILD_WITH_INSTALL_RPATH 1
            SKIP_BUILD_RPATH FALSE
        )
    endif ()

    set_target_properties(${target_name} PROPERTIES
        INSTALL_RPATH_USE_LINK_PATH TRUE
    )
endmacro()

# Common macro kernels (xkarilang)
macro(karilang_set_kernel_options target_name)
    if (KARILANG_USE_SHARED_KARILANG)
        target_link_libraries(${target_name} PRIVATE xeus-karilang)
        if(CMAKE_DL_LIBS)
            target_link_libraries(${target_name} PRIVATE ${CMAKE_DL_LIBS} util)
        endif()
    else ()
        target_link_libraries(${target_name} PRIVATE xeus-karilang-static)
    endif()

    if(NOT EMSCRIPTEN)
        find_package(Threads)
        target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
    endif()
    
endmacro()

# Common macro for shared and static library
macro(karilang_create_target target_name linkage output_name)
    string(TOUPPER "${linkage}" linkage_upper)

    if (NOT ${linkage_upper} MATCHES "^(SHARED|STATIC)$")
        message(FATAL_ERROR "Invalid library linkage: ${linkage}")
    endif ()

    add_library(${target_name} ${linkage_upper} ${KARILANG_SRC} ${KARILANG_HEADERS})
    karilang_set_common_options(${target_name})

    set_target_properties(${target_name} PROPERTIES
                          PUBLIC_HEADER "${KARILANG_HEADERS}"
                          PREFIX ""
                          VERSION ${${PROJECT_NAME}_VERSION}
                          SOVERSION ${KARILANG_VERSION_MAJOR}
                          OUTPUT_NAME "lib${output_name}")

    target_compile_definitions(${target_name} PUBLIC "KARILANG_EXPORTS")

    # target_compile_features(${target_name} PRIVATE cxx_std_17)

    target_include_directories(${target_name}
                               PUBLIC
                               $<BUILD_INTERFACE:${KARILANG_INCLUDE_DIR}>
                               $<INSTALL_INTERFACE:include>)

    if (KARILANG_USE_SHARED_XEUS)
        set(KARILANG_XEUS_TARGET xeus)
    else ()
        set(KARILANG_XEUS_TARGET xeus-static)
    endif ()

    target_link_libraries(${target_name} PUBLIC ${KARILANG_XEUS_TARGET} xtl)
    if (WIN32 OR CYGWIN)
        #
    elseif (APPLE)
        target_link_libraries(${target_name} PRIVATE "-undefined dynamic_lookup")
    endif ()
    if(NOT EMSCRIPTEN)
        find_package(Threads) # TODO: add Threads as a dependence of xeus-static?
        target_link_libraries(${target_name} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
    endif()
    
endmacro()

# xeus-karilang
# ===========

set(KARILANG_TARGETS "")

if (KARILANG_BUILD_SHARED)
    # Build libraries
    karilang_create_target(xeus-karilang SHARED xeus-karilang)
    list(APPEND KARILANG_TARGETS xeus-karilang)
endif ()

if (KARILANG_BUILD_STATIC)
    # On Windows, a static library should use a different output name
    # to avoid the conflict with the import library of a shared one.
    if (CMAKE_HOST_WIN32)
        karilang_create_target(xeus-karilang-static STATIC xeus-karilang-static)
    else ()
        karilang_create_target(xeus-karilang-static STATIC xeus-karilang)
    endif ()
    list(APPEND KARILANG_TARGETS xeus-karilang-static)
endif ()

# xkarilang
# =======
if (KARILANG_BUILD_EXECUTABLE)
    find_package(xeus-zmq 1.0.1 REQUIRED)
    add_executable(xkarilang ${KARILANG_MAIN_SRC})
    # target_compile_features(xkarilang PRIVATE cxx_std_17)
    karilang_set_common_options(xkarilang)
    karilang_set_kernel_options(xkarilang)
    target_link_libraries(xkarilang PRIVATE xeus-zmq KariLang_language)
endif()

if(EMSCRIPTEN)
    include(WasmBuildOptions)
    find_package(xeus-lite REQUIRED)
    add_executable(xkarilang src/main_emscripten_kernel.cpp )
    target_link_libraries(xkarilang PRIVATE xeus-karilang-static xeus-lite KariLang_language)
    KARILANG_set_kernel_options(xkarilang)
    xeus_wasm_compile_options(xkarilang)
    xeus_wasm_link_options(xkarilang "web,worker")
endif()


# Installation
# ============
include(CMakePackageConfigHelpers)

set(KARILANG_CMAKECONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" CACHE STRING "install path for xeus-karilangConfig.cmake")

# Install xeus-karilang and xeus-karilang-static
if (KARILANG_BUILD_SHARED)
    install(TARGETS ${KARILANG_TARGETS}
            EXPORT ${PROJECT_NAME}-targets
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/src)

    # Makes the project importable from the build directory
    export(EXPORT ${PROJECT_NAME}-targets
           FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake")
endif ()

# Install xkarilang
if (KARILANG_BUILD_EXECUTABLE)
    install(TARGETS xkarilang
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

if (KARILANG_BUILD_EXECUTABLE OR EMSCRIPTEN)
    # Configuration and data directories for jupyter and xeus-karilang
    set(XJUPYTER_DATA_DIR "share/jupyter"    CACHE STRING "Jupyter data directory")

    # Install xkarilang Jupyter kernelspec
    set(KERNELSPEC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels)
    install(DIRECTORY ${KERNELSPEC_DIR}
            DESTINATION ${XJUPYTER_DATA_DIR}
            PATTERN "*.in" EXCLUDE)


    # Extra path for installing Jupyter kernelspec
    if (XEXTRA_JUPYTER_DATA_DIR)
        install(DIRECTORY ${KERNELSPEC_DIR}
                DESTINATION ${XEXTRA_JUPYTER_DATA_DIR}
                PATTERN "*.in" EXCLUDE)
    endif ()
endif ()

# Configure 'xeus-karilangConfig.cmake' for a build tree
set(KARILANG_CONFIG_CODE "####### Expanded from \@KARILANG_CONFIG_CODE\@ #######\n")
set(KARILANG_CONFIG_CODE "${KARILANG_CONFIG_CODE}set(CMAKE_MODULE_PATH \"${CMAKE_CURRENT_SOURCE_DIR}/cmake;\${CMAKE_MODULE_PATH}\")\n")
set(KARILANG_CONFIG_CODE "${KARILANG_CONFIG_CODE}##################################################")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${PROJECT_BINARY_DIR})

# Configure 'xeus-karilangConfig.cmake.in for an install tree
set(KARILANG_CONFIG_CODE "")
configure_package_config_file(${PROJECT_NAME}Config.cmake.in
                              "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake"
                              INSTALL_DESTINATION ${KARILANG_CMAKECONFIG_INSTALL_DIR})

write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
                                 VERSION ${${PROJECT_NAME}_VERSION}
                                 COMPATIBILITY AnyNewerVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
              DESTINATION ${KARILANG_CMAKECONFIG_INSTALL_DIR})

if (KARILANG_BUILD_SHARED)
    install(EXPORT ${PROJECT_NAME}-targets
            FILE ${PROJECT_NAME}Targets.cmake
            DESTINATION ${KARILANG_CMAKECONFIG_INSTALL_DIR})
endif ()

if(EMSCRIPTEN)
    install(TARGETS xkarilang
            ARCHIVE DESTINATION ${KARILANG_CMAKECONFIG_INSTALL_DIR})

    install(FILES
            "$<TARGET_FILE_DIR:xkarilang>/xkarilang.js"
            "$<TARGET_FILE_DIR:xkarilang>/xkarilang.wasm"
            DESTINATION ${CMAKE_INSTALL_BINDIR})
endif ()

