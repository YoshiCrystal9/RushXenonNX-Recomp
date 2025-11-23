# RushXenonNX/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

project(RushXenonNX LANGUAGES CXX)
#
#set(RushXenonNX_PRECOMPILED_HEADERS
#        "Xenon/stdafx.h"
#)
#
#set(RushXenonNX_KERNEL_CXX_SOURCES
#        "Xenon/kernel/imports.cpp"
#        "Xenon/kernel/xdm.cpp"
#        "Xenon/kernel/heap.cpp"
#        "Xenon/kernel/memory.cpp"
#        "Xenon/kernel/xam.cpp"
#        "Xenon/kernel/io/file_system.cpp"
#)

set(RushXenonNX_NX_CXX_SOURCES
        "NX/log/nxlogger.cpp"
        "NX/fs/fs_helpers.cpp"
)

# --- Recolectar fuentes ---
file(GLOB_RECURSE PPC_RECOMP_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/ppc/ppc_recomp.*.cpp"
)

set(SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/memory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/guest_thread.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/heap.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/kernel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ppc/ppc_func_mapping.cpp
        ${PPC_RECOMP_SOURCES}
        ${RushXenonNX_NX_CXX_SOURCES}
        ${CMAKE_CURRENT_SOURCE_DIR}/xdbf_wrapper.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/xex.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/xex_patcher.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/image.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/aes.c
        ${CMAKE_CURRENT_SOURCE_DIR}/memory_mapped_file.cpp
        ${O1_INCLUDE_DIR}/o1heap.c
        ${MSPACK_INCLUDE_DIR}/lzxd.c
)

set(HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/ppc/ppc_config.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ppc/ppc_context.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ppc/ppc_recomp_shared.h
        ${CMAKE_CURRENT_SOURCE_DIR}/NX/log/nxlogger.h
        ${CMAKE_CURRENT_SOURCE_DIR}/NX/fs/fs_helpers.h
)

# --- Compilación ---
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/ppc
        ${SIMDE_INCLUDE_DIR}
        ${SDL2_INCLUDE_DIR}
        ${O1_INCLUDE_DIR}
        ${MSPACK_INCLUDE_DIR}
)

# --- Librerías de libnx ---
target_link_libraries(${PROJECT_NAME} PRIVATE nx SDL2)

# --- Opciones de compilador ---
target_compile_options(${PROJECT_NAME} PRIVATE -march=armv8-a+simd -w)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
