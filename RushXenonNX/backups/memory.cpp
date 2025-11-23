#include "Memory.h"
#include <switch.h>
#include "nx/log/nxlogger.h"

Memory::Memory() {
    SDLogger::Log("Memory constructor called");

    // Configuración de bloques de memoria
    block_size = 128 * 1024 * 1024; // 128 MB por bloque
    total_blocks = 24; // Total 3GB
    total_memory = block_size * total_blocks;

    blocks.reserve(total_blocks);
    SDLogger::Log("Inicializando %zu bloques de %zu MB cada uno (Total: %zu MB)...",
                  total_blocks, block_size / (1024 * 1024), total_memory / (1024 * 1024));

    for (size_t i = 0; i < total_blocks; ++i) {
        uint8_t* block = (uint8_t*)malloc(block_size);
        if (!block) {
            SDLogger::Log("ERROR: Falló malloc para bloque %zu", i);
            // Liberar bloques ya asignados
            for (auto b : blocks) free(b);
            return;
        }
        memset(block, 0, block_size);
        blocks.push_back(block);
        SDLogger::Log("Bloque %zu asignado en %p", i, block);
    }

    // Asignar el primer bloque a 'base' para compatibilidad
    base = blocks[0];
    SDLogger::Log("Base establecida en %p", base);

    SDLogger::Log("Memory initialization complete - %zu bloques asignados", blocks.size());
}

Memory::~Memory() {
    for (auto block : blocks) {
        free(block);
    }
    blocks.clear();
    base = nullptr;
}

void* Memory::Translate(size_t offset) const noexcept {
    SDLogger::Log(("Memory::Translate - Requested offset: " + std::to_string(offset) +
                   ", Total memory: " + std::to_string(total_memory)).c_str());

    if (offset >= total_memory) {
        SDLogger::Log("ERROR: Memory::Translate - Offset %zu fuera de rango (max: %zu)", offset, total_memory);
        return nullptr;
    }

    size_t block_index = offset / block_size;
    size_t block_offset = offset % block_size;

    void* result = blocks[block_index] + block_offset;

    SDLogger::Log(("Memory::Translate - Block index: " + std::to_string(block_index) +
                   ", Block offset: " + std::to_string(block_offset) +
                   ", Result: " + std::to_string((uintptr_t)result)).c_str());

    return result;
}

uint32_t Memory::MapVirtual(const void* host) const noexcept {
    if (!host) return 0;

    // Encontrar en qué bloque está este puntero
    for (size_t i = 0; i < blocks.size(); i++) {
        uint8_t* block_start = blocks[i];
        uint8_t* block_end = block_start + block_size;

        if (host >= block_start && host < block_end) {
            size_t offset = (static_cast<const uint8_t*>(host) - block_start) + (i * block_size);
            SDLogger::Log(("Memory::MapVirtual - Host " + std::to_string((uintptr_t)host) +
                          " mapped to offset " + std::to_string(offset)).c_str());
            return static_cast<uint32_t>(offset);
        }
    }

    SDLogger::Log("ERROR: Memory::MapVirtual - Puntero %p no encontrado en bloques de memoria", host);
    return 0;
}

bool Memory::IsInMemoryRange(const void* host) const noexcept {
    for (auto block : blocks) {
        if (host >= block && host < (block + block_size)) {
            return true;
        }
    }
    return false;
}

void* MmGetHostAddress(uint32_t ptr) {
    void* addr = g_memory.Translate(ptr);
    SDLogger::Log(("MmGetHostAddress - Translating guest address " + std::to_string(ptr) +
                   " to host address " + std::to_string((uintptr_t)addr)).c_str());
    return addr;
}