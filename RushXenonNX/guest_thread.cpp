#include "guest_thread.h"
#include "memory.h"
#include "byteswap.h"
#include "ppc_context.h"
#include "nx/log/nxlogger.h"

#include <cstring>
#include <cassert>
#include <algorithm>
#include <bit>
#include <iostream>

constexpr size_t PCR_SIZE = 0xAB0;
constexpr size_t TLS_SIZE = 0x100;
constexpr size_t TEB_SIZE = 0x2E0;
constexpr size_t STACK_SIZE = 0x40000;
constexpr size_t TOTAL_SIZE = PCR_SIZE + TLS_SIZE + TEB_SIZE + STACK_SIZE;

constexpr size_t TEB_OFFSET = PCR_SIZE + TLS_SIZE;

// libnx thread entry point wrapper
void GuestThreadFuncWrapper(void* arg)
{
    GuestThreadHandle* hThread = static_cast<GuestThreadHandle*>(arg);
    SDLogger::Log("GuestThreadFuncWrapper - Waiting for thread to be resumed");

    hThread->suspended.wait(true);
    SDLogger::Log("GuestThreadFuncWrapper - Starting thread function");

    // Call the actual thread function
    GuestThread::Start(hThread->params);

    // Exit the thread when done
    threadExit();
}

GuestThreadContext::GuestThreadContext(uint32_t cpuNumber)
{
    SDLogger::Log("GuestThreadContext - Allocating thread context");

    assert(thread == nullptr);

    thread = (uint8_t*)g_userHeap.Alloc(TOTAL_SIZE);
    SDLogger::Log(("GuestThreadContext - Allocated memory at " + std::to_string((uintptr_t)thread)).c_str());

    memset(thread, 0, TOTAL_SIZE);

    *(uint32_t*)thread = ByteSwap(g_memory.MapVirtual(thread + PCR_SIZE)); // tls pointer
    *(uint32_t*)(thread + 0x100) = ByteSwap(g_memory.MapVirtual(thread + PCR_SIZE + TLS_SIZE)); // teb pointer
    *(thread + 0x10C) = cpuNumber;

    *(uint32_t*)(thread + PCR_SIZE + 0x10) = 0xFFFFFFFF;
    *(uint32_t*)(thread + PCR_SIZE + TLS_SIZE + 0x14C) = ByteSwap(GuestThread::GetCurrentThreadId());

    ppcContext.r1.u64 = g_memory.MapVirtual(thread + PCR_SIZE + TLS_SIZE + TEB_SIZE + STACK_SIZE);
    ppcContext.r13.u64 = g_memory.MapVirtual(thread);
    ppcContext.fpscr.loadFromHost();

    SDLogger::Log(("GuestThreadContext - Context initialized for CPU " + std::to_string(cpuNumber)).c_str());
    assert(GetPPCContext() == nullptr);
    SetPPCContext(ppcContext);
}

GuestThreadContext::~GuestThreadContext()
{
    SDLogger::Log(("GuestThreadContext - Freeing thread context at " + std::to_string((uintptr_t)thread)).c_str());
    g_userHeap.Free(thread);
}

GuestThreadHandle::GuestThreadHandle(const GuestThreadParams& params)
    : params(params), suspended((params.flags & 0x1) != 0)
{
    SDLogger::Log("GuestThreadHandle - Creating libnx thread");

    // Allocate stack for libnx thread (64KB should be sufficient)
    const size_t libnx_stack_size = 0x10000;
    thread_stack = malloc(libnx_stack_size);

    if (thread_stack == nullptr) {
        SDLogger::Log("GuestThreadHandle - Failed to allocate stack memory!");
        return;
    }

    // Create the libnx thread
    Result rc = threadCreate(
        &thread_handle,
        GuestThreadFuncWrapper,
        this,
        thread_stack,
        libnx_stack_size,
        0x2C,  // Default priority
        -2     // Default CPU
    );

    if (R_FAILED(rc)) {
        SDLogger::Log(("GuestThreadHandle - threadCreate failed: " + std::to_string(rc)).c_str());
        free(thread_stack);
        thread_stack = nullptr;
        return;
    }

    // Start the thread
    rc = threadStart(&thread_handle);
    if (R_FAILED(rc)) {
        SDLogger::Log(("GuestThreadHandle - threadStart failed: " + std::to_string(rc)).c_str());
        threadClose(&thread_handle);
        free(thread_stack);
        thread_stack = nullptr;
        return;
    }

    SDLogger::Log("GuestThreadHandle - libnx thread created and started");
}

GuestThreadHandle::~GuestThreadHandle()
{
    SDLogger::Log("GuestThreadHandle - Cleaning up libnx thread");

    if (thread_stack != nullptr) {
        // Wait for thread to exit if it's still running
        if (thread_handle.handle != 0) {
            threadWaitForExit(&thread_handle);
            threadClose(&thread_handle);
        }

        free(thread_stack);
        thread_stack = nullptr;
    }
}

uint32_t GuestThreadHandle::GetThreadId() const
{
    // For libnx, we can use the thread handle as the ID
    uint32_t tid = static_cast<uint32_t>(thread_handle.handle);
    SDLogger::Log(("GuestThreadHandle - Thread ID is " + std::to_string(tid)).c_str());
    return tid;
}

uint32_t GuestThreadHandle::Wait(uint32_t timeout)
{
    SDLogger::Log(("GuestThreadHandle::Wait - Waiting for thread with timeout " + std::to_string(timeout)).c_str());
    assert(timeout == INFINITE);

    if (thread_handle.handle != 0) {
        threadWaitForExit(&thread_handle);
    }

    return STATUS_WAIT_0;
}

uint32_t GuestThread::Start(const GuestThreadParams& params)
{
    const auto procMask = (uint8_t)(params.flags >> 24);
    const auto cpuNumber = procMask == 0 ? 0 : 7 - std::countl_zero(procMask);

    SDLogger::Log(("GuestThread::Start - Starting thread on CPU " + std::to_string(cpuNumber)).c_str());

    GuestThreadContext ctx(cpuNumber);
    ctx.ppcContext.r3.u64 = params.value;

    SDLogger::Log("GuestThread::Start - Calling guest function");
    g_memory.FindFunction(params.function)(ctx.ppcContext, g_memory.base);

    SDLogger::Log(("GuestThread::Start - Thread finished with return value " + std::to_string(ctx.ppcContext.r3.u32)).c_str());
    return ctx.ppcContext.r3.u32;
}

GuestThreadHandle* GuestThread::Start(const GuestThreadParams& params, uint32_t* threadId)
{
    auto hThread = CreateKernelObject<GuestThreadHandle>(params);
    SDLogger::Log("GuestThread::Start - Created GuestThreadHandle");

    if (threadId != nullptr && hThread->thread_handle.handle != 0) {
        *threadId = hThread->GetThreadId();
    }

    return hThread;
}

uint32_t GuestThread::GetCurrentThreadId()
{
    // For libnx, use the current thread handle as ID
    Thread* current_thread = threadGetSelf();
    uint32_t tid = current_thread ? static_cast<uint32_t>(current_thread->handle) : 0;
    SDLogger::Log(("GuestThread::GetCurrentThreadId - Current thread ID is " + std::to_string(tid)).c_str());
    return tid;
}

void GuestThread::SetLastError(uint32_t error)
{
    SDLogger::Log(("GuestThread::SetLastError - Setting error code " + std::to_string(error)).c_str());
    auto* thread = (char*)g_memory.Translate(GetPPCContext()->r13.u32);
    if (*(uint32_t*)(thread + 0x150))
    {
        return;
    }

    *(uint32_t*)(thread + TEB_OFFSET + 0x160) = ByteSwap(error);
}

void SetThreadNameImpl(uint32_t a1, uint32_t threadId, uint32_t* name)
{
    SDLogger::Log(("SetThreadNameImpl - ThreadID=" + std::to_string(threadId)).c_str());
}

int GetThreadPriorityImpl(GuestThreadHandle* hThread)
{
    SDLogger::Log("GetThreadPriorityImpl - Called for thread handle");
    return 0;
}

uint32_t SetThreadIdealProcessorImpl(GuestThreadHandle* hThread, uint32_t dwIdealProcessor)
{
    SDLogger::Log(("SetThreadIdealProcessorImpl - ThreadID=" + std::to_string(hThread->GetThreadId()) +
                   ", IdealProcessor=" + std::to_string(dwIdealProcessor)).c_str());
    return 0;
}