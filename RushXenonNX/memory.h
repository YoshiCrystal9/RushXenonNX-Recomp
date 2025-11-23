#pragma once
#include <cassert>
#include <cstring>
#include <ppc_context.h>
#include <switch.h>

#include "NX/log/nxlogger.h"

struct Memory {
    uint8_t* base{};

    Memory();
    bool IsInMemoryRange(const void* host) const noexcept {
        return host >= base && host < (base + PPC_MEMORY_SIZE);
    }

    void* Translate(size_t offset) const noexcept {
        SDLogger::Log(("Memory::Translate - Requested offset: " + std::to_string(offset) +
                       ", Memory size: " + std::to_string(PPC_MEMORY_SIZE)).c_str());

        if (offset) {
            //assert(offset < PPC_MEMORY_SIZE);
        }

        void* result = base + offset;
        SDLogger::Log(("Memory::Translate - Resulting pointer: " + std::to_string((uintptr_t)result)).c_str());

        return result;
    }


    uint32_t MapVirtual(const void* host) const noexcept {
        if (host) assert(IsInMemoryRange(host));
        return static_cast<uint32_t>(static_cast<const uint8_t*>(host) - base);
    }

    // Example function, modify according to actual need
    PPCFunc* FindFunction(uint32_t guest) const noexcept
    {
        return PPC_LOOKUP_FUNC(base, guest);
    }

    void InsertFunction(uint32_t guest, PPCFunc* host)
    {
        PPC_LOOKUP_FUNC(base, guest) = host;
    }
};

extern "C" void* MmGetHostAddress(uint32_t ptr);
extern Memory g_memory;
