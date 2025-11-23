#include "Memory.h"
#include <switch.h>
#include "nx/log/nxlogger.h"

Memory::Memory() {
    SDLogger::Log("Memory constructor called");

    // Memory allocation using libnx
    base = (uint8_t*)malloc(PPC_MEMORY_SIZE);  // Allocate memory
    if (base == nullptr) {
        SDLogger::Log("Failed to allocate memory!");
        return;
    } else {
        SDLogger::Log("Memory allocated successfully");
    }

    // Optionally, clear memory
    memset(base, 0, PPC_MEMORY_SIZE);
    SDLogger::Log("Memory cleared with memset");

    svcSetMemoryAttribute(base, 0x1000, MemType_Unmapped, 0);
    SDLogger::Log("Memory attribute set with svcSetMemoryAttribute");

    // Initialize the memory by setting up mappings
    for (size_t i = 0; PPCFuncMappings[i].guest != 0; i++) {
        if (PPCFuncMappings[i].host != nullptr) {
            SDLogger::Log(("Inserting function mapping: guest = " +
                           std::to_string(PPCFuncMappings[i].guest) +
                           ", host = " + std::to_string((uintptr_t)PPCFuncMappings[i].host)).c_str());
            InsertFunction(PPCFuncMappings[i].guest, PPCFuncMappings[i].host);
        }
    }

    SDLogger::Log("Memory initialization complete");
}

void* MmGetHostAddress(uint32_t ptr) {
    void* addr = g_memory.Translate(ptr);
    SDLogger::Log(("Translating guest address " + std::to_string(ptr) +
                   " to host address " + std::to_string((uintptr_t)addr)).c_str());
    return addr;
}
