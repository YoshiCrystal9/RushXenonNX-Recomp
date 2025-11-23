#include "heap.h"
#include "memory.h"
#include "function.h"
#include <switch.h>
#include <algorithm> // std::max
#include <cstring>   // memset, memcpy
#include <cassert>
#include "nx/log/nxlogger.h" // logging

// RAII wrapper para Mutex de libnx
struct LockGuard {
    Mutex* mtx;
    LockGuard(Mutex* m) : mtx(m) { mutexLock(mtx); }
    ~LockGuard() { mutexUnlock(mtx); }
};

// Mutex de libnx
static Mutex mutex;
static Mutex physicalMutex;

constexpr size_t RESERVED_BEGIN = 0x7FEA0000;
constexpr size_t RESERVED_END = 0xA0000000;
constexpr size_t PHYSICAL_MEMORY_END = 0xC0000000; // 3GB = 0xC0000000

void Heap::Init()
{
    mutexInit(&mutex);
    mutexInit(&physicalMutex);

    SDLogger::Log("Heap::Init - Initializing virtual heap");
    heap = o1heapInit(g_memory.Translate(0x20000), RESERVED_BEGIN - 0x20000);

    SDLogger::Log("Heap::Init - Initializing physical heap");
    physicalHeap = o1heapInit(g_memory.Translate(RESERVED_END), PHYSICAL_MEMORY_END - RESERVED_END);
}

void* Heap::Alloc(size_t size)
{
    LockGuard lock(&mutex);

    size_t allocSize = std::max<size_t>(1, size);
    SDLogger::Log("Heap::Alloc - Solicitud de %zu bytes (ajustado a %zu)", size, allocSize);

    void* ptr = o1heapAllocate(heap, allocSize);
    SDLogger::Log("Heap::Alloc - Puntero asignado: %p", ptr);

    return ptr;
}


void* Heap::AllocPhysical(size_t size, size_t alignment)
{
    size = std::max<size_t>(1, size);
    alignment = alignment == 0 ? 0x1000 : std::max<size_t>(16, alignment);

    LockGuard lock(&physicalMutex);

    SDLogger::Log(("Heap::AllocPhysical - Allocating " + std::to_string(size) + " bytes with alignment " + std::to_string(alignment)).c_str());

    void* ptr = o1heapAllocate(physicalHeap, size + alignment);
    size_t aligned = ((size_t)ptr + alignment) & ~(alignment - 1);

    *((void**)aligned - 1) = ptr;
    *((size_t*)aligned - 2) = size + O1HEAP_ALIGNMENT;

    SDLogger::Log(("Heap::AllocPhysical - Allocated physical memory at " + std::to_string((uintptr_t)aligned)).c_str());
    return (void*)aligned;
}

void Heap::Free(void* ptr)
{
    if (ptr >= physicalHeap)
    {
        LockGuard lock(&physicalMutex);
        SDLogger::Log(("Heap::Free - Freeing physical memory at " + std::to_string((uintptr_t)ptr)).c_str());
        o1heapFree(physicalHeap, *((void**)ptr - 1));
    }
    else
    {
        LockGuard lock(&mutex);
        SDLogger::Log(("Heap::Free - Freeing virtual memory at " + std::to_string((uintptr_t)ptr)).c_str());
        o1heapFree(heap, ptr);
    }
}

size_t Heap::Size(void* ptr)
{
    if (ptr)
    {
        size_t sz = *((size_t*)ptr - 2) - O1HEAP_ALIGNMENT;
        SDLogger::Log(("Heap::Size - Size of memory at " + std::to_string((uintptr_t)ptr) + " is " + std::to_string(sz)).c_str());
        return sz;
    }

    return 0;
}

uint32_t RtlAllocateHeap(uint32_t heapHandle, uint32_t flags, uint32_t size)
{
    SDLogger::Log(("RtlAllocateHeap - Allocating " + std::to_string(size) + " bytes, flags=" + std::to_string(flags)).c_str());
    void* ptr = g_userHeap.Alloc(size);
    if ((flags & 0x8) != 0)
        memset(ptr, 0, size);

    assert(ptr);
    return g_memory.MapVirtual(ptr);
}

uint32_t RtlReAllocateHeap(uint32_t heapHandle, uint32_t flags, uint32_t memoryPointer, uint32_t size)
{
    SDLogger::Log(("RtlReAllocateHeap - Reallocating memoryPointer=" + std::to_string(memoryPointer) + " to " + std::to_string(size) + " bytes, flags=" + std::to_string(flags)).c_str());
    void* ptr = g_userHeap.Alloc(size);
    if ((flags & 0x8) != 0)
        memset(ptr, 0, size);

    if (memoryPointer != 0)
    {
        void* oldPtr = g_memory.Translate(memoryPointer);
        memcpy(ptr, oldPtr, std::min<size_t>(size, g_userHeap.Size(oldPtr)));
        g_userHeap.Free(oldPtr);
    }

    assert(ptr);
    return g_memory.MapVirtual(ptr);
}

uint32_t RtlFreeHeap(uint32_t heapHandle, uint32_t flags, uint32_t memoryPointer)
{
    SDLogger::Log(("RtlFreeHeap - Freeing memoryPointer=" + std::to_string(memoryPointer)).c_str());
    if (memoryPointer != NULL)
        g_userHeap.Free(g_memory.Translate(memoryPointer));

    return true;
}

uint32_t RtlSizeHeap(uint32_t heapHandle, uint32_t flags, uint32_t memoryPointer)
{
    if (memoryPointer != NULL)
    {
        uint32_t sz = (uint32_t)g_userHeap.Size(g_memory.Translate(memoryPointer));
        SDLogger::Log(("RtlSizeHeap - Size of memoryPointer=" + std::to_string(memoryPointer) + " is " + std::to_string(sz)).c_str());
        return sz;
    }

    return 0;
}

uint32_t XAllocMem(uint32_t size, uint32_t flags)
{
    SDLogger::Log(("XAllocMem - Allocating " + std::to_string(size) + " bytes, flags=" + std::to_string(flags)).c_str());
    void* ptr = (flags & 0x80000000) != 0 ?
        g_userHeap.AllocPhysical(size, (1ull << ((flags >> 24) & 0xF))) :
        g_userHeap.Alloc(size);

    if ((flags & 0x40000000) != 0)
        memset(ptr, 0, size);

    assert(ptr);
    return g_memory.MapVirtual(ptr);
}

void XFreeMem(uint32_t baseAddress, uint32_t flags)
{
    SDLogger::Log(("XFreeMem - Freeing memory at " + std::to_string(baseAddress)).c_str());
    if (baseAddress != NULL)
        g_userHeap.Free(g_memory.Translate(baseAddress));
}
