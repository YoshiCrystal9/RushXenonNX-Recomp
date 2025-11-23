#pragma once
#include <cassert>
#include <cstring>
#include <vector>
#include <ppc_context.h>
#include <switch.h>
#include "NX/log/nxlogger.h"

struct Memory {
    uint8_t* base{};  // Mantenemos base como el primer bloque
    std::vector<uint8_t*> blocks;
    size_t block_size;
    size_t total_blocks;
    size_t total_memory;

    Memory();
    ~Memory();

    bool IsInMemoryRange(const void* host) const noexcept;
    void* Translate(size_t offset) const noexcept;
    uint32_t MapVirtual(const void* host) const noexcept;

    PPCFunc* FindFunction(uint32_t guest) const noexcept {
        // Calcula la entrada correspondiente en la tabla
        uint64_t table_guest_offset = (uint64_t)PPC_IMAGE_BASE +
                                      (uint64_t)PPC_IMAGE_SIZE +
                                      (guest - PPC_CODE_BASE) * 2ULL;

        uint8_t* host_entry = (uint8_t*)Translate(static_cast<size_t>(table_guest_offset));
        if (!host_entry) {
            SDLogger::Log("FindFunction - host_entry null for guest %08x", guest);
            return nullptr;
        }

        // La tabla contiene un offset relativo de 16 bits (little endian)
        uint16_t offset = host_entry[0] | (host_entry[1] << 8);

        // Calcula la dirección real del código en memoria traducida
        void* func_ptr = Translate(PPC_CODE_BASE + offset);
        if (!func_ptr) {
            SDLogger::Log("FindFunction - func_ptr null for guest %08x, offset=%04x", guest, offset);
            return nullptr;
        }

        SDLogger::Log("FindFunction - guest %08x -> func_ptr %p (offset %04x)", guest, func_ptr, offset);
        return (PPCFunc*)func_ptr;
    }


    void InsertFunction(uint32_t guest, PPCFunc* host) {
        uint64_t table_guest_offset = (uint64_t)PPC_IMAGE_BASE +
                                      (uint64_t)PPC_IMAGE_SIZE +
                                      (uint64_t)(guest - PPC_CODE_BASE) * 2ULL;
        void* host_entry = Translate(static_cast<size_t>(table_guest_offset));
        if (!host_entry) {
            SDLogger::Log("InsertFunction - Translate devolvió nullptr para offset %llu", (unsigned long long)table_guest_offset);
            return;
        }
        *(PPCFunc**)host_entry = host;
    }
};

extern "C" void* MmGetHostAddress(uint32_t ptr);
extern Memory g_memory;