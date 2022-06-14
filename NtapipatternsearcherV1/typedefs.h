#pragma once
#include "ntdllx64dbg.h"

#include <iostream>
#include <vector>
#include <functional> //boyer_moore 
#include <algorithm>//std::search
#include <stdint.h> //uintptr_t

#include <winnt.h>



typedef NTSYSCALLAPI NTSTATUS(NTAPI* tdNtQuerySystemInformation)(
    SYSTEM_INFORMATION_CLASS,
    PVOID,
    ULONG,
    PULONG);



typedef NTSYSAPI NTSTATUS(NTAPI* tdNtQueryInformationProcess)(
    IN HANDLE               ProcessHandle,
    IN PROCESSINFOCLASS ProcessInformationClass,
    OUT PVOID               ProcessInformation,
    IN ULONG                ProcessInformationLength,
    OUT PULONG              ReturnLength);


typedef NTSYSAPI NTSTATUS(NTAPI* tdNtOpenProcess)(
    PHANDLE ProcessHandle,
    ACCESS_MASK AccessMask,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PCLIENT_ID ClientId);



typedef NTSYSAPI NTSTATUS(NTAPI* tdNtClose)(
    HANDLE Handle);



typedef NTSYSCALLAPI NTSTATUS(NTAPI* tdNtReadVirtualMemory)(
    _In_ HANDLE ProcessHandle,
    _In_opt_ PVOID BaseAddress,
    _Out_ PVOID Buffer,
    _In_ SIZE_T BufferSize,
    _Out_opt_ PSIZE_T NumberOfBytesRead);


typedef struct _LDR_MODULE {



    LIST_ENTRY              InLoadOrderModuleList;
    LIST_ENTRY              InMemoryOrderModuleList;
    LIST_ENTRY              InInitializationOrderModuleList;
    PVOID                   BaseAddress;
    PVOID                   EntryPoint;
    ULONG                   SizeOfImage;
    UNICODE_STRING          FullDllName;
    UNICODE_STRING          BaseDllName;
    ULONG                   Flags;
    SHORT                   LoadCount;
    SHORT                   TlsIndex;
    LIST_ENTRY              HashTableEntry;
    ULONG                   TimeDateStamp;

} LDR_MODULE, * PLDR_MODULE;


typedef NTSYSCALLAPI NTSTATUS(NTAPI* tdNtQueryVirtualMemory)(
    _In_ HANDLE ProcessHandle,
    _In_ PVOID BaseAddress,
    _In_ MEMORY_INFORMATION_CLASS MemoryInformationClass,
    _Out_ PVOID MemoryInformation,
    _In_ SIZE_T MemoryInformationLength,
    _Out_opt_ PSIZE_T ReturnLength);


typedef NTSYSAPI NTSTATUS(NTAPI* tdNtReadVirtualMemory)(
    _In_ HANDLE ProcessHandle,
    _In_opt_ PVOID BaseAddress,
    _Out_ PVOID Buffer,
    _In_ SIZE_T BufferSize,
    _Out_opt_ PSIZE_T NumberOfBytesRead);