cmake_minimum_required(VERSION 3.20)
set(CMAKE_BUILD_TYPE Debug)

# --- Verificar entorno ---
if(NOT DEFINED ENV{DEVKITPRO})
    message(FATAL_ERROR "DEVKITPRO not defined.")
endif()

set(DEVKITPRO $ENV{DEVKITPRO})
set(DEVKITARM $ENV{DEVKITARM})

# --- Directorios base ---
set(EXTERN_DIR "${CMAKE_SOURCE_DIR}/extern")
set(SIMDE_INCLUDE_DIR "${EXTERN_DIR}/simde/simde")
set(SDL2_INCLUDE_DIR "${DEVKITPRO}/portlibs/switch/include")
set(IMGUI_INCLUDE_DIR "${EXTERN_DIR}/imgui/")
set(STB_INCLUDE_DIR "${EXTERN_DIR}/stb/")
set(O1_INCLUDE_DIR "${EXTERN_DIR}/o1heap/o1heap")
set(MSPACK_INCLUDE_DIR "${EXTERN_DIR}/libmspack/libmspack/mspack")

project(RushXenonNX-ALL LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BUILD_SHARED_LIBS OFF)

# --- Configuración de compilación global ---
add_compile_options(-march=armv8-a+simd -w)

# --- Directorios de salida ---
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# --- Subproyecto principal ---
add_subdirectory(RushXenonNX)

# --- Incluir SIMDe globalmente ---
include_directories(${SIMDE_INCLUDE_DIR})
include_directories(${O1_INCLUDE_DIR})
include_directories(${MSPACK_INCLUDE_DIR})

# --- Herramientas devkitPro ---
find_program(ELF2NRO elf2nro HINTS ${DEVKITPRO}/tools/bin)
find_program(NACPTOOL nacptool HINTS ${DEVKITPRO}/tools/bin)

if(NOT ELF2NRO)
    message(FATAL_ERROR "elf2nro not found.")
endif()

if(NOT NACPTOOL)
    message(FATAL_ERROR "nacptool not found.")
endif()

# --- Rutas de salida ---
set(OUTPUT_NRO ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.nro)
set(OUTPUT_NACP ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.nacp)
set(ICON ${DEVKITPRO}/libnx/default_icon.jpg)

# --- Generar NACP ---
add_custom_command(
        OUTPUT ${OUTPUT_NACP}
        COMMAND ${NACPTOOL} --create "${PROJECT_NAME}" "Autor" "1.0.0" ${OUTPUT_NACP}
        COMMENT "Creando ${PROJECT_NAME}.nacp"
        VERBATIM
)

# --- Generar NRO ---
add_custom_command(
        OUTPUT ${OUTPUT_NRO}
        DEPENDS RushXenonNX ${OUTPUT_NACP}
        COMMAND ${ELF2NRO} $<TARGET_FILE:RushXenonNX> ${OUTPUT_NRO}
        --icon=${ICON}
        --nacp=${OUTPUT_NACP}
        COMMENT "Generando ${PROJECT_NAME}.nro"
        VERBATIM
)

add_custom_target(nro ALL DEPENDS ${OUTPUT_NRO})
