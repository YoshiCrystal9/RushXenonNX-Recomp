#include <iostream>
#include <switch.h>
#include "ppc/ppc_context.h"
#include "function.h"
#include "ppc/ppc_recomp_shared.h"
#include "nx/log/nxlogger.h"
#include <atomic>

uint32_t KeGetCurrentProcessType()
{
    return 1;
}

uint32_t RtlInitializeCriticalSection(XRTL_CRITICAL_SECTION* cs)
{
    SDLogger::Log("RtlInitializeCriticalSection called");
    SDLogger::Log("CS pointer = %p", cs);
    cs->Header.Absolute = 0;
    cs->LockCount = -1;
    cs->RecursionCount = 0;
    cs->OwningThread = 0;

    return 0;
}

void RtlEnterCriticalSection(XRTL_CRITICAL_SECTION* cs)
{
    uint32_t thisThread = g_ppcContext->r13.u32;
    // printf("RtlEnterCriticalSection %x %x %x %x\n", thisThread, cs->OwningThread, cs->LockCount, cs->RecursionCount);
    assert(thisThread != NULL);

    std::atomic_ref owningThread(cs->OwningThread);

    while (true)
    {
        be<unsigned> previousOwner = 0;

        if (owningThread.compare_exchange_weak(previousOwner, thisThread) || previousOwner == thisThread)
        {
            cs->RecursionCount = cs->RecursionCount.get() + 1;
            return;
        }

        // printf("wait start %x\n", cs);
        owningThread.wait(previousOwner);
        // printf("wait end\n");
    }
}

void RtlLeaveCriticalSection(XRTL_CRITICAL_SECTION* cs)
{
    // printf("RtlLeaveCriticalSection");
    cs->RecursionCount = cs->RecursionCount.get() - 1;

    if (cs->RecursionCount.get() != 0)
        return;

    std::atomic_ref owningThread(cs->OwningThread);
    owningThread.store(0);
    owningThread.notify_one();
}


GUEST_FUNCTION_STUB(__imp__XNotifyGetNext);//XNotifyGetNext);;
GUEST_FUNCTION_STUB(__imp__XamMarketplaceAcquireFreeContent);//XamMarketplaceAcquireFreeContent);;
GUEST_FUNCTION_STUB(__imp__XNotifyPositionUI);//XNotifyPositionUI);;
GUEST_FUNCTION_STUB(__imp__XamLoaderSetLaunchData);//XamLoaderSetLaunchData);;
GUEST_FUNCTION_STUB(__imp__XamLoaderGetLaunchData);//XamLoaderGetLaunchData);;
GUEST_FUNCTION_STUB(__imp__XamLoaderGetLaunchDataSize);//XamLoaderGetLaunchDataSize);;
GUEST_FUNCTION_STUB(__imp__XGetVideoMode);//XGetVideoMode);;
GUEST_FUNCTION_STUB(__imp__XGetGameRegion);//XGetGameRegion);;
GUEST_FUNCTION_STUB(__imp__XamShowMessageBoxUIEx);//XamShowMessageBoxUIEx);;
GUEST_FUNCTION_STUB(__imp__XGetAVPack);//XGetAVPack);;
GUEST_FUNCTION_STUB(__imp__XamLoaderTerminateTitle);//XamLoaderTerminateTitle);;
GUEST_FUNCTION_STUB(__imp__XamGetLocaleEx);//XamGetLocaleEx);;
GUEST_FUNCTION_STUB(__imp__XamBackgroundDownloadSetMode);//XamBackgroundDownloadSetMode);;
GUEST_FUNCTION_STUB(__imp__XamBackgroundDownloadGetMode);//XamBackgroundDownloadGetMode);;
GUEST_FUNCTION_STUB(__imp__XamNotifyCreateListener);//XamNotifyCreateListener);;
GUEST_FUNCTION_STUB(__imp__XMsgStartIORequest);//XMsgStartIORequest);;
GUEST_FUNCTION_STUB(__imp__XamUserGetName);//XamUserGetName);;
GUEST_FUNCTION_STUB(__imp__XamUserGetSigninState);//XamUserGetSigninState);;
GUEST_FUNCTION_STUB(__imp__XamUserAreUsersFriends);//XamUserAreUsersFriends);;
GUEST_FUNCTION_STUB(__imp__XamUserCheckPrivilege);//XamUserCheckPrivilege);;
GUEST_FUNCTION_STUB(__imp__XamGetSystemVersion);//XamGetSystemVersion);;
GUEST_FUNCTION_STUB(__imp__XamUserCreateAchievementEnumerator);//XamUserCreateAchievementEnumerator);;
GUEST_FUNCTION_STUB(__imp__XamUserGetXUID);//XamUserGetXUID);;
GUEST_FUNCTION_STUB(__imp__XamContentCreateEx);//XamContentCreateEx);;
GUEST_FUNCTION_STUB(__imp__XamContentDelete);//XamContentDelete);;
GUEST_FUNCTION_STUB(__imp__XamContentClose);//XamContentClose);;
GUEST_FUNCTION_STUB(__imp__XamContentSetThumbnail);//XamContentSetThumbnail);;
GUEST_FUNCTION_STUB(__imp__XamContentGetCreator);//XamContentGetCreator);;
GUEST_FUNCTION_STUB(__imp__XamContentCreateEnumerator);//XamContentCreateEnumerator);;
GUEST_FUNCTION_STUB(__imp__XamContentGetDeviceData);//XamContentGetDeviceData);;
GUEST_FUNCTION_STUB(__imp__XamContentResolve);//XamContentResolve);;
GUEST_FUNCTION_STUB(__imp__XamShowSigninUI);//XamShowSigninUI);;
GUEST_FUNCTION_STUB(__imp__XamShowKeyboardUI);//XamShowKeyboardUI);;
GUEST_FUNCTION_STUB(__imp__XamShowGamerCardUIForXUID);//XamShowGamerCardUIForXUID);;
GUEST_FUNCTION_STUB(__imp__XamShowPlayerReviewUI);//XamShowPlayerReviewUI);;
GUEST_FUNCTION_STUB(__imp__XamShowMarketplaceUI);//XamShowMarketplaceUI);;
GUEST_FUNCTION_STUB(__imp__XamShowDeviceSelectorUI);//XamShowDeviceSelectorUI);;
GUEST_FUNCTION_STUB(__imp__XamShowMessageBoxUI);//XamShowMessageBoxUI);;
GUEST_FUNCTION_STUB(__imp__XamShowMessageComposeUI);//XamShowMessageComposeUI);;
GUEST_FUNCTION_STUB(__imp__XamShowFriendRequestUI);//XamShowFriendRequestUI);;
GUEST_FUNCTION_STUB(__imp__XamShowDirtyDiscErrorUI);//XamShowDirtyDiscErrorUI);;
GUEST_FUNCTION_STUB(__imp__XamEnumerate);//XamEnumerate);;
GUEST_FUNCTION_STUB(__imp__XamBackgroundDownloadItemGetHistoryStatus);//XamBackgroundDownloadItemGetHistoryStatus);;
GUEST_FUNCTION_STUB(__imp__XamBackgroundDownloadItemGetStatus);//XamBackgroundDownloadItemGetStatus);;
GUEST_FUNCTION_STUB(__imp__XMsgInProcessCall);//XMsgInProcessCall);;
GUEST_FUNCTION_STUB(__imp__XamShowMarketplaceDownloadItemsUI);//XamShowMarketplaceDownloadItemsUI);;
GUEST_FUNCTION_STUB(__imp__XamTaskShouldExit);//XamTaskShouldExit);;
GUEST_FUNCTION_STUB(__imp__XamTaskCloseHandle);//XamTaskCloseHandle);;
GUEST_FUNCTION_STUB(__imp__XamTaskSchedule);//XamTaskSchedule);;
GUEST_FUNCTION_STUB(__imp__XamLoaderLaunchTitle);//XamLoaderLaunchTitle);;
GUEST_FUNCTION_STUB(__imp__XamInputGetCapabilities);//XamInputGetCapabilities);;
GUEST_FUNCTION_STUB(__imp__XamInputGetState);//XamInputGetState);;
GUEST_FUNCTION_STUB(__imp__XamInputSetState);//XamInputSetState);;
GUEST_FUNCTION_STUB(__imp__XamInputGetKeystrokeEx);//XamInputGetKeystrokeEx);;
GUEST_FUNCTION_STUB(__imp__XamUserGetAgeGroup);//XamUserGetAgeGroup);;
GUEST_FUNCTION_STUB(__imp__XamUserGetSigninInfo);//XamUserGetSigninInfo);;
GUEST_FUNCTION_STUB(__imp__XMsgCancelIORequest);//XMsgCancelIORequest);;
GUEST_FUNCTION_STUB(__imp__XamGetExecutionId);//XamGetExecutionId);;
GUEST_FUNCTION_STUB(__imp____C_specific_handler);//__C_specific_handler);;
GUEST_FUNCTION_STUB(__imp__RtlInitializeCriticalSectionAndSpinCount);//RtlInitializeCriticalSectionAndSpinCount);;
GUEST_FUNCTION_STUB(__imp__RtlTryEnterCriticalSection);//RtlTryEnterCriticalSection);;
GUEST_FUNCTION_STUB(__imp__KeBugCheck);//KeBugCheck);;
GUEST_FUNCTION_STUB(__imp__RtlUnwind);//RtlUnwind);;
GUEST_FUNCTION_STUB(__imp__KeTlsGetValue);//KeTlsGetValue);;
GUEST_FUNCTION_STUB(__imp__KeTlsSetValue);//KeTlsSetValue);;
GUEST_FUNCTION_STUB(__imp__KeTlsFree);//KeTlsFree);;
GUEST_FUNCTION_STUB(__imp__KeTlsAlloc);//KeTlsAlloc);;
GUEST_FUNCTION_STUB(__imp__NtQueryVirtualMemory);//NtQueryVirtualMemory);;
GUEST_FUNCTION_STUB(__imp__MmQueryStatistics);//MmQueryStatistics);;
GUEST_FUNCTION_STUB(__imp__NtClose);//NtClose);;
GUEST_FUNCTION_STUB(__imp__NtCreateEvent);//NtCreateEvent);;
GUEST_FUNCTION_STUB(__imp__ExGetXConfigSetting);//ExGetXConfigSetting);;
GUEST_FUNCTION_STUB(__imp__XexCheckExecutablePrivilege);//XexCheckExecutablePrivilege);;
GUEST_FUNCTION_STUB(__imp__DbgPrint);//DbgPrint);;
GUEST_FUNCTION_STUB(__imp__MmQueryAddressProtect);//MmQueryAddressProtect);;
GUEST_FUNCTION_STUB(__imp__RtlRaiseException);//RtlRaiseException);;
GUEST_FUNCTION_STUB(__imp__NtOpenFile);//NtOpenFile);;
GUEST_FUNCTION_STUB(__imp__RtlInitAnsiString);//RtlInitAnsiString);;
GUEST_FUNCTION_STUB(__imp__NtCreateFile);//NtCreateFile);;
GUEST_FUNCTION_STUB(__imp__NtWaitForSingleObjectEx);//NtWaitForSingleObjectEx);;
GUEST_FUNCTION_STUB(__imp__ObDereferenceObject);//ObDereferenceObject);;
GUEST_FUNCTION_STUB(__imp__KeSetBasePriorityThread);//KeSetBasePriorityThread);;
GUEST_FUNCTION_STUB(__imp__ObReferenceObjectByHandle);//ObReferenceObjectByHandle);;
GUEST_FUNCTION_STUB(__imp__KeQueryBasePriorityThread);//KeQueryBasePriorityThread);;
GUEST_FUNCTION_STUB(__imp__KeSetDisableBoostThread);//KeSetDisableBoostThread);;
GUEST_FUNCTION_STUB(__imp__KeSetAffinityThread);//KeSetAffinityThread);;
GUEST_FUNCTION_STUB(__imp__KeDelayExecutionThread);//KeDelayExecutionThread);;
GUEST_FUNCTION_STUB(__imp__XexUnloadImage);//XexUnloadImage);;
GUEST_FUNCTION_STUB(__imp__XexGetProcedureAddress);//XexGetProcedureAddress);;
GUEST_FUNCTION_STUB(__imp__XexLoadImage);//XexLoadImage);;
GUEST_FUNCTION_STUB(__imp__NtCreateSemaphore);//NtCreateSemaphore);;
GUEST_FUNCTION_STUB(__imp__NtReleaseSemaphore);//NtReleaseSemaphore);;
GUEST_FUNCTION_STUB(__imp__NtCreateMutant);//NtCreateMutant);;
GUEST_FUNCTION_STUB(__imp__NtReleaseMutant);//NtReleaseMutant);;
GUEST_FUNCTION_STUB(__imp__ExTerminateThread);//ExTerminateThread);;
GUEST_FUNCTION_STUB(__imp__NtSetEvent);//NtSetEvent);;
GUEST_FUNCTION_STUB(__imp__MmAllocatePhysicalMemoryEx);//MmAllocatePhysicalMemoryEx);;
GUEST_FUNCTION_STUB(__imp__MmQueryAllocationSize);//MmQueryAllocationSize);;
GUEST_FUNCTION_STUB(__imp__MmFreePhysicalMemory);//MmFreePhysicalMemory);;
GUEST_FUNCTION_STUB(__imp__NtResumeThread);//NtResumeThread);;
GUEST_FUNCTION_STUB(__imp__StfsControlDevice);//StfsControlDevice);;
GUEST_FUNCTION_STUB(__imp__StfsCreateDevice);//StfsCreateDevice);;
GUEST_FUNCTION_STUB(__imp__NtQueryVolumeInformationFile);//NtQueryVolumeInformationFile);;
GUEST_FUNCTION_STUB(__imp__RtlImageXexHeaderField);//RtlImageXexHeaderField);;
GUEST_FUNCTION_STUB(__imp__XeKeysConsoleSignatureVerification);//XeKeysConsoleSignatureVerification);;
GUEST_FUNCTION_STUB(__imp__XeCryptSha);//XeCryptSha);;
GUEST_FUNCTION_STUB(__imp__NtWriteFile);//NtWriteFile);;
GUEST_FUNCTION_STUB(__imp__NtReadFile);//NtReadFile);;
GUEST_FUNCTION_STUB(__imp__XeKeysConsolePrivateKeySign);//XeKeysConsolePrivateKeySign);;
GUEST_FUNCTION_STUB(__imp__NtFreeVirtualMemory);//NtFreeVirtualMemory);;
GUEST_FUNCTION_STUB(__imp__NtAllocateVirtualMemory);//NtAllocateVirtualMemory);;
GUEST_FUNCTION_STUB(__imp___snprintf);//_snprintf);;
GUEST_FUNCTION_STUB(__imp__KeLeaveCriticalRegion);//KeLeaveCriticalRegion);;
GUEST_FUNCTION_STUB(__imp__NtFlushBuffersFile);//NtFlushBuffersFile);;
GUEST_FUNCTION_STUB(__imp__KeEnterCriticalRegion);//KeEnterCriticalRegion);;
GUEST_FUNCTION_STUB(__imp__IoDismountVolume);//IoDismountVolume);;
GUEST_FUNCTION_STUB(__imp__RtlNtStatusToDosError);//RtlNtStatusToDosError);;
GUEST_FUNCTION_STUB(__imp__ObCreateSymbolicLink);//ObCreateSymbolicLink);;
GUEST_FUNCTION_STUB(__imp__ObDeleteSymbolicLink);//ObDeleteSymbolicLink);;
GUEST_FUNCTION_STUB(__imp__KeResetEvent);//KeResetEvent);;
GUEST_FUNCTION_STUB(__imp__KeWaitForSingleObject);//KeWaitForSingleObject);;
GUEST_FUNCTION_STUB(__imp__KeSetEvent);//KeSetEvent);;
GUEST_FUNCTION_STUB(__imp__ExRegisterTitleTerminateNotification);//ExRegisterTitleTerminateNotification);;
GUEST_FUNCTION_STUB(__imp__NtSetInformationFile);//NtSetInformationFile);;
GUEST_FUNCTION_STUB(__imp__NtQueryFullAttributesFile);//NtQueryFullAttributesFile);;
GUEST_FUNCTION_STUB(__imp__NtQueryInformationFile);//NtQueryInformationFile);;
GUEST_FUNCTION_STUB(__imp__RtlTimeToTimeFields);//RtlTimeToTimeFields);;
GUEST_FUNCTION_STUB(__imp__KeQuerySystemTime);//KeQuerySystemTime);;
GUEST_FUNCTION_STUB(__imp__RtlTimeFieldsToTime);//RtlTimeFieldsToTime);;
GUEST_FUNCTION_STUB(__imp__RtlMultiByteToUnicodeN);//RtlMultiByteToUnicodeN);;
GUEST_FUNCTION_STUB(__imp__KeQueryPerformanceFrequency);//KeQueryPerformanceFrequency);;
GUEST_FUNCTION_STUB(__imp__NtClearEvent);//NtClearEvent);;
GUEST_FUNCTION_STUB(__imp__RtlFillMemoryUlong);//RtlFillMemoryUlong);;
GUEST_FUNCTION_STUB(__imp__KeBugCheckEx);//KeBugCheckEx);;
GUEST_FUNCTION_STUB(__imp__RtlCompareMemoryUlong);//RtlCompareMemoryUlong);;
GUEST_FUNCTION_STUB(__imp__RtlFreeAnsiString);//RtlFreeAnsiString);;
GUEST_FUNCTION_STUB(__imp__RtlUnicodeStringToAnsiString);//RtlUnicodeStringToAnsiString);;
GUEST_FUNCTION_STUB(__imp__RtlInitUnicodeString);//RtlInitUnicodeString);;
GUEST_FUNCTION_STUB(__imp__RtlUnicodeToMultiByteN);//RtlUnicodeToMultiByteN);;
GUEST_FUNCTION_STUB(__imp__HalReturnToFirmware);//HalReturnToFirmware);;
GUEST_FUNCTION_STUB(__imp__NtQueryDirectoryFile);//NtQueryDirectoryFile);;
GUEST_FUNCTION_STUB(__imp__NtReadFileScatter);//NtReadFileScatter);;
GUEST_FUNCTION_STUB(__imp__NtDuplicateObject);//NtDuplicateObject);;
GUEST_FUNCTION_STUB(__imp__ExCreateThread);//ExCreateThread);;
GUEST_FUNCTION_STUB(__imp__IoInvalidDeviceRequest);//IoInvalidDeviceRequest);;
GUEST_FUNCTION_STUB(__imp__ObReferenceObject);//ObReferenceObject);;
GUEST_FUNCTION_STUB(__imp__IoCreateDevice);//IoCreateDevice);;
GUEST_FUNCTION_STUB(__imp__IoDeleteDevice);//IoDeleteDevice);;
GUEST_FUNCTION_STUB(__imp__ExAllocatePoolTypeWithTag);//ExAllocatePoolTypeWithTag);;
GUEST_FUNCTION_STUB(__imp__ExFreePool);//ExFreePool);;
GUEST_FUNCTION_STUB(__imp__RtlCompareStringN);//RtlCompareStringN);;
GUEST_FUNCTION_STUB(__imp__IoCompleteRequest);//IoCompleteRequest);;
GUEST_FUNCTION_STUB(__imp__NtWriteFileGather);//NtWriteFileGather);;
GUEST_FUNCTION_STUB(__imp__KfReleaseSpinLock);//KfReleaseSpinLock);;
GUEST_FUNCTION_STUB(__imp__KfAcquireSpinLock);//KfAcquireSpinLock);;
GUEST_FUNCTION_STUB(__imp__RtlUpcaseUnicodeChar);//RtlUpcaseUnicodeChar);;
GUEST_FUNCTION_STUB(__imp__ObIsTitleObject);//ObIsTitleObject);;
GUEST_FUNCTION_STUB(__imp__IoCheckShareAccess);//IoCheckShareAccess);;
GUEST_FUNCTION_STUB(__imp__IoSetShareAccess);//IoSetShareAccess);;
GUEST_FUNCTION_STUB(__imp__IoRemoveShareAccess);//IoRemoveShareAccess);;
GUEST_FUNCTION_STUB(__imp__IoDismountVolumeByFileHandle);//IoDismountVolumeByFileHandle);;
GUEST_FUNCTION_STUB(__imp__NtDeviceIoControlFile);//NtDeviceIoControlFile);;
GUEST_FUNCTION_STUB(__imp__ExAllocatePool);//ExAllocatePool);;
GUEST_FUNCTION_STUB(__imp__XAudioGetVoiceCategoryVolume);//XAudioGetVoiceCategoryVolume);;
GUEST_FUNCTION_STUB(__imp__MmGetPhysicalAddress);//MmGetPhysicalAddress);;
GUEST_FUNCTION_STUB(__imp__MmMapIoSpace);//MmMapIoSpace);;
GUEST_FUNCTION_STUB(__imp__XMACreateContext);//XMACreateContext);;
GUEST_FUNCTION_STUB(__imp__XMAReleaseContext);//XMAReleaseContext);;
GUEST_FUNCTION_STUB(__imp__KeWaitForMultipleObjects);//KeWaitForMultipleObjects);;
GUEST_FUNCTION_STUB(__imp__XAudioSubmitRenderDriverFrame);//XAudioSubmitRenderDriverFrame);;
GUEST_FUNCTION_STUB(__imp__XAudioUnregisterRenderDriverClient);//XAudioUnregisterRenderDriverClient);;
GUEST_FUNCTION_STUB(__imp__XAudioRegisterRenderDriverClient);//XAudioRegisterRenderDriverClient);;
GUEST_FUNCTION_STUB(__imp__XAudioGetSpeakerConfig);//XAudioGetSpeakerConfig);;
GUEST_FUNCTION_STUB(__imp__XexGetModuleHandle);//XexGetModuleHandle);;
GUEST_FUNCTION_STUB(__imp__XAudioGetDuckerLevel);//XAudioGetDuckerLevel);;
GUEST_FUNCTION_STUB(__imp__XAudioEnableDucker);//XAudioEnableDucker);;
GUEST_FUNCTION_STUB(__imp__XAudioGetDuckerReleaseTime);//XAudioGetDuckerReleaseTime);;
GUEST_FUNCTION_STUB(__imp__XAudioGetDuckerAttackTime);//XAudioGetDuckerAttackTime);;
GUEST_FUNCTION_STUB(__imp__XAudioGetDuckerHoldTime);//XAudioGetDuckerHoldTime);;
GUEST_FUNCTION_STUB(__imp__XAudioGetDuckerThreshold);//XAudioGetDuckerThreshold);;
GUEST_FUNCTION_STUB(__imp__KeReleaseSpinLockFromRaisedIrql);//KeReleaseSpinLockFromRaisedIrql);;
GUEST_FUNCTION_STUB(__imp__KeAcquireSpinLockAtRaisedIrql);//KeAcquireSpinLockAtRaisedIrql);;
GUEST_FUNCTION_STUB(__imp__KeInsertQueueDpc);//KeInsertQueueDpc);;
GUEST_FUNCTION_STUB(__imp__KiApcNormalRoutineNop);//KiApcNormalRoutineNop);;
GUEST_FUNCTION_STUB(__imp__VdEnableRingBufferRPtrWriteBack);//VdEnableRingBufferRPtrWriteBack);;
GUEST_FUNCTION_STUB(__imp__VdInitializeRingBuffer);//VdInitializeRingBuffer);;
GUEST_FUNCTION_STUB(__imp__VdSetSystemCommandBufferGpuIdentifierAddress);//VdSetSystemCommandBufferGpuIdentifierAddress);;
GUEST_FUNCTION_STUB(__imp__VdPersistDisplay);//VdPersistDisplay);;
GUEST_FUNCTION_STUB(__imp__VdSwap);//VdSwap);;
GUEST_FUNCTION_STUB(__imp__VdGetSystemCommandBuffer);//VdGetSystemCommandBuffer);;
GUEST_FUNCTION_STUB(__imp__VdEnableDisableClockGating);//VdEnableDisableClockGating);;
GUEST_FUNCTION_STUB(__imp__sprintf);//sprintf);;
GUEST_FUNCTION_STUB(__imp__VdGetCurrentDisplayGamma);//VdGetCurrentDisplayGamma);;
GUEST_FUNCTION_STUB(__imp___vsnprintf);//_vsnprintf);;
GUEST_FUNCTION_STUB(__imp__VdCallGraphicsNotificationRoutines);//VdCallGraphicsNotificationRoutines);;
GUEST_FUNCTION_STUB(__imp__VdGetCurrentDisplayInformation);//VdGetCurrentDisplayInformation);;
GUEST_FUNCTION_STUB(__imp__VdSetDisplayMode);//VdSetDisplayMode);;
GUEST_FUNCTION_STUB(__imp__VdQueryVideoMode);//VdQueryVideoMode);;
GUEST_FUNCTION_STUB(__imp__VdSetDisplayModeOverride);//VdSetDisplayModeOverride);;
GUEST_FUNCTION_STUB(__imp__VdShutdownEngines);//VdShutdownEngines);;
GUEST_FUNCTION_STUB(__imp__VdSetGraphicsInterruptCallback);//VdSetGraphicsInterruptCallback);;
GUEST_FUNCTION_STUB(__imp__VdInitializeEngines);//VdInitializeEngines);;
GUEST_FUNCTION_STUB(__imp__VdIsHSIOTrainingSucceeded);//VdIsHSIOTrainingSucceeded);;
GUEST_FUNCTION_STUB(__imp__KeLockL2);//KeLockL2);;
GUEST_FUNCTION_STUB(__imp__KeUnlockL2);//KeUnlockL2);;
GUEST_FUNCTION_STUB(__imp__KeInitializeDpc);//KeInitializeDpc);;
GUEST_FUNCTION_STUB(__imp__VdQueryVideoFlags);//VdQueryVideoFlags);;
GUEST_FUNCTION_STUB(__imp__VdInitializeScalerCommandBuffer);//VdInitializeScalerCommandBuffer);;
GUEST_FUNCTION_STUB(__imp__VdRetrainEDRAM);//VdRetrainEDRAM);;
GUEST_FUNCTION_STUB(__imp__VdRetrainEDRAMWorker);//VdRetrainEDRAMWorker);;
GUEST_FUNCTION_STUB(__imp__KeSetCurrentProcessType);//KeSetCurrentProcessType);;
GUEST_FUNCTION_STUB(__imp__XamSwapDisc);//XamSwapDisc);;
GUEST_FUNCTION_STUB(__imp__XamSwapCancel);//XamSwapCancel);;
GUEST_FUNCTION_STUB(__imp__XamVoiceIsActiveProcess);//XamVoiceIsActiveProcess);;
GUEST_FUNCTION_STUB(__imp__XamUserGetDeviceContext);//XamUserGetDeviceContext);;
GUEST_FUNCTION_STUB(__imp__XamUserReadProfileSettings);//XamUserReadProfileSettings);;
GUEST_FUNCTION_STUB(__imp__XamVoiceHeadsetPresent);//XamVoiceHeadsetPresent);;
GUEST_FUNCTION_STUB(__imp__XamVoiceClose);//XamVoiceClose);;
GUEST_FUNCTION_STUB(__imp__XamVoiceSubmitPacket);//XamVoiceSubmitPacket);;
GUEST_FUNCTION_STUB(__imp__XamVoiceCreate);//XamVoiceCreate);;
GUEST_FUNCTION_STUB(__imp__XMsgStartIORequestEx);//XMsgStartIORequestEx);;
GUEST_FUNCTION_STUB(__imp__XamCreateEnumeratorHandle);//XamCreateEnumeratorHandle);;
GUEST_FUNCTION_STUB(__imp__XamGetPrivateEnumStructureFromHandle);//XamGetPrivateEnumStructureFromHandle);;
GUEST_FUNCTION_STUB(__imp__XamUserWriteProfileSettings);//XamUserWriteProfileSettings);;
GUEST_FUNCTION_STUB(__imp__XamAlloc);//XamAlloc);;
GUEST_FUNCTION_STUB(__imp__XamUserGetMembershipTierFromXUID);//XamUserGetMembershipTierFromXUID);;
GUEST_FUNCTION_STUB(__imp__XamGetLanguage);//XamGetLanguage);;
GUEST_FUNCTION_STUB(__imp__XamUserGetOnlineCountryFromXUID);//XamUserGetOnlineCountryFromXUID);;
GUEST_FUNCTION_STUB(__imp__XNetLogonGetTitleID);//XNetLogonGetTitleID);;
GUEST_FUNCTION_STUB(__imp__XamFree);//XamFree);;
GUEST_FUNCTION_STUB(__imp__XNetLogonGetMachineID);//XNetLogonGetMachineID);;
GUEST_FUNCTION_STUB(__imp__NetDll_WSAStartup);//NetDll_WSAStartup);;
GUEST_FUNCTION_STUB(__imp__NetDll_WSACleanup);//NetDll_WSACleanup);;
GUEST_FUNCTION_STUB(__imp__NetDll_socket);//NetDll_socket);;
GUEST_FUNCTION_STUB(__imp__NetDll_closesocket);//NetDll_closesocket);;
GUEST_FUNCTION_STUB(__imp__NetDll_shutdown);//NetDll_shutdown);;
GUEST_FUNCTION_STUB(__imp__NetDll_ioctlsocket);//NetDll_ioctlsocket);;
GUEST_FUNCTION_STUB(__imp__NetDll_setsockopt);//NetDll_setsockopt);;
GUEST_FUNCTION_STUB(__imp__NetDll_getsockname);//NetDll_getsockname);;
GUEST_FUNCTION_STUB(__imp__NetDll_bind);//NetDll_bind);;
GUEST_FUNCTION_STUB(__imp__NetDll_connect);//NetDll_connect);;
GUEST_FUNCTION_STUB(__imp__NetDll_listen);//NetDll_listen);;
GUEST_FUNCTION_STUB(__imp__NetDll_accept);//NetDll_accept);;
GUEST_FUNCTION_STUB(__imp__NetDll_select);//NetDll_select);;
GUEST_FUNCTION_STUB(__imp__NetDll_recv);//NetDll_recv);;
GUEST_FUNCTION_STUB(__imp__NetDll_recvfrom);//NetDll_recvfrom);;
GUEST_FUNCTION_STUB(__imp__NetDll_send);//NetDll_send);;
GUEST_FUNCTION_STUB(__imp__NetDll_sendto);//NetDll_sendto);;
GUEST_FUNCTION_STUB(__imp__NetDll_inet_addr);//NetDll_inet_addr);;
GUEST_FUNCTION_STUB(__imp__NetDll_WSAGetLastError);//NetDll_WSAGetLastError);;
GUEST_FUNCTION_STUB(__imp__NetDll___WSAFDIsSet);//NetDll___WSAFDIsSet);;
GUEST_FUNCTION_STUB(__imp__NetDll_XNetStartup);//NetDll_XNetStartup);;
GUEST_FUNCTION_STUB(__imp__NetDll_XNetCleanup);//NetDll_XNetCleanup);;
GUEST_FUNCTION_STUB(__imp__NetDll_XNetRegisterKey);//NetDll_XNetRegisterKey);;
GUEST_FUNCTION_STUB(__imp__NetDll_XNetUnregisterKey);//NetDll_XNetUnregisterKey);;
GUEST_FUNCTION_STUB(__imp__NetDll_XNetXnAddrToInAddr);//NetDll_XNetXnAddrToInAddr);;
GUEST_FUNCTION_STUB(__imp__NetDll_XNetServerToInAddr);//NetDll_XNetServerToInAddr);;
GUEST_FUNCTION_STUB(__imp__NetDll_XNetInAddrToXnAddr);//NetDll_XNetInAddrToXnAddr);;
GUEST_FUNCTION_STUB(__imp__NetDll_XNetInAddrToServer);//NetDll_XNetInAddrToServer);;
GUEST_FUNCTION_STUB(__imp__NetDll_XNetUnregisterInAddr);//NetDll_XNetUnregisterInAddr);;
GUEST_FUNCTION_STUB(__imp__NetDll_XNetConnect);//NetDll_XNetConnect);;
GUEST_FUNCTION_STUB(__imp__NetDll_XNetGetConnectStatus);//NetDll_XNetGetConnectStatus);;
GUEST_FUNCTION_STUB(__imp__NetDll_XNetDnsLookup);//NetDll_XNetDnsLookup);;
GUEST_FUNCTION_STUB(__imp__NetDll_XNetDnsRelease);//NetDll_XNetDnsRelease);;
GUEST_FUNCTION_STUB(__imp__NetDll_XNetQosListen);//NetDll_XNetQosListen);;
GUEST_FUNCTION_STUB(__imp__NetDll_XNetGetTitleXnAddr);//NetDll_XNetGetTitleXnAddr);;
GUEST_FUNCTION_STUB(__imp__NetDll_XNetGetEthernetLinkStatus);//NetDll_XNetGetEthernetLinkStatus);;
GUEST_FUNCTION_STUB(__imp__NetDll_XNetGetOpt);//NetDll_XNetGetOpt);;
GUEST_FUNCTION_STUB(__imp__XamSessionRefObjByHandle);//XamSessionRefObjByHandle);;
GUEST_FUNCTION_STUB(__imp__XamSessionCreateHandle);//XamSessionCreateHandle);;
GUEST_FUNCTION_STUB(__imp__NetDll_XnpLogonGetStatus);//NetDll_XnpLogonGetStatus);;
GUEST_FUNCTION_STUB(__imp__NetDll_XHttpStartup);//NetDll_XHttpStartup);;
GUEST_FUNCTION_STUB(__imp__NetDll_XHttpShutdown);//NetDll_XHttpShutdown);;
GUEST_FUNCTION_STUB(__imp__NetDll_XHttpOpen);//NetDll_XHttpOpen);;
GUEST_FUNCTION_STUB(__imp__NetDll_XHttpCloseHandle);//NetDll_XHttpCloseHandle);;
GUEST_FUNCTION_STUB(__imp__NetDll_XHttpConnect);//NetDll_XHttpConnect);;
GUEST_FUNCTION_STUB(__imp__NetDll_XHttpSetStatusCallback);//NetDll_XHttpSetStatusCallback);;
GUEST_FUNCTION_STUB(__imp__NetDll_XHttpOpenRequest);//NetDll_XHttpOpenRequest);;
GUEST_FUNCTION_STUB(__imp__NetDll_XHttpSendRequest);//NetDll_XHttpSendRequest);;
GUEST_FUNCTION_STUB(__imp__NetDll_XHttpReceiveResponse);//NetDll_XHttpReceiveResponse);;
GUEST_FUNCTION_STUB(__imp__NetDll_XHttpQueryHeaders);//NetDll_XHttpQueryHeaders);;
GUEST_FUNCTION_STUB(__imp__NetDll_XHttpReadData);//NetDll_XHttpReadData);;
GUEST_FUNCTION_STUB(__imp__NetDll_XHttpWriteData);//NetDll_XHttpWriteData);;
GUEST_FUNCTION_STUB(__imp__NetDll_XHttpDoWork);//NetDll_XHttpDoWork);;
GUEST_FUNCTION_STUB(__imp__XamFreeToken);//XamFreeToken);;
GUEST_FUNCTION_STUB(__imp__XamGetToken);//XamGetToken);;
GUEST_FUNCTION_STUB(__imp__XampXAuthStartup);//XampXAuthStartup);;
GUEST_FUNCTION_STUB(__imp__XampXAuthGetTitleBuffer);//XampXAuthGetTitleBuffer);;
GUEST_FUNCTION_STUB(__imp__XampXAuthShutdown);//XampXAuthShutdown);;
GUEST_FUNCTION_STUB(__imp__NtYieldExecution);//NtYieldExecution);;
GUEST_FUNCTION_STUB(__imp__NtCancelTimer);//NtCancelTimer);;
GUEST_FUNCTION_STUB(__imp__NtSetTimerEx);//NtSetTimerEx);;
GUEST_FUNCTION_STUB(__imp__NtCreateTimer);//NtCreateTimer);

GUEST_FUNCTION_HOOK(__imp__RtlLeaveCriticalSection,RtlLeaveCriticalSection);
GUEST_FUNCTION_HOOK(__imp__RtlInitializeCriticalSection,RtlInitializeCriticalSection);
GUEST_FUNCTION_HOOK(__imp__KeGetCurrentProcessType,KeGetCurrentProcessType);
GUEST_FUNCTION_HOOK(__imp__RtlEnterCriticalSection,RtlEnterCriticalSection);;