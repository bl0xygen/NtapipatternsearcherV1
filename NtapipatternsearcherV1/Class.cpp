#pragma once
#include "Class.h"

uintptr_t NtPatternClass::FindId(const wchar_t* exename) {

	HMODULE hMod = LoadLibraryW(L"ntdll.dll");


	/* needed to use ntquerysysinfo */
	tdNtQuerySystemInformation NtQuerySystemInformation = 0;

	NtQuerySystemInformation = (tdNtQuerySystemInformation)GetProcAddress(hMod, "NtQuerySystemInformation");

	/* first call NtQuerySystemInformation to get the size of the buffer */
	ULONG buffersize;
	PSYSTEM_PROCESS_INFORMATION pspi;

	NTSTATUS status = NtQuerySystemInformation(SystemProcessInformation, nullptr, 0, &buffersize);

	/* operation succeeded now create a buffer of the appropriate size */
	PVOID buffer = malloc(buffersize);
	pspi = (PSYSTEM_PROCESS_INFORMATION)buffer;

	status = NtQuerySystemInformation(SystemProcessInformation, (PVOID)pspi, buffersize, 0);
	if (status != 0) {
		std::cout << "\nNtQuerySystemInformation failed, status was " << status;
	}

	std::cout << "\nMatching process name with id\n" << std::endl;

	uintptr_t procid = 0;
	
	do {
		printf("process id: %ld -- process image name: %ws\n", (uintptr_t)pspi->UniqueProcessId, pspi->ImageName.Buffer);

		if (wcsncmp(exename, pspi->ImageName.Buffer, pspi->ImageName.Length / sizeof(WCHAR)) == 0) {
			if (pspi->ImageName.Buffer != 0) {//skip the first entry
				printf("\nmatch found: %ld, %ws\n", pspi->UniqueProcessId, pspi->ImageName.Buffer);
				procid = (uintptr_t)pspi->UniqueProcessId;
				free(buffer);
				if (hMod != 0) {
					FreeLibrary(hMod);
				}
				return procid;
			}
		}
		pspi = (PSYSTEM_PROCESS_INFORMATION)((PBYTE)pspi + pspi->NextEntryOffset);

	} while (pspi->NextEntryOffset);

	/* process name was not found */
	std::cout << "\nNo process name was found" << std::endl;
	FreeLibrary(hMod);
	return EXIT_FAILURE;
}


void NtPatternClass::GetRemotePEBmodules(uintptr_t procid) {

	HMODULE hMod = LoadLibraryW(L"ntdll.dll");

	tdNtOpenProcess NtOpenProc = 0;
	NtOpenProc = (tdNtOpenProcess)GetProcAddress(hMod, "NtOpenProcess");

	tdNtClose tNtClose = 0;
	tNtClose = (tdNtClose)GetProcAddress(hMod, "NtClose");

	//setup for ntopenproc
	CLIENT_ID pid = { 0 };
	pid.UniqueProcess = (PDWORD)procid;
	pid.UniqueThread = NULL;

	HANDLE processhandle;

	static OBJECT_ATTRIBUTES soa = { sizeof(soa) };

	NTSTATUS status;

	status = NtOpenProc(&processhandle, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, &soa, &pid);
	if (status != 0) {
		printf("\nUnable to open process handle");

	}

	printf("\nProcess Handle opened with PROCESS_QUERY_INFORMATION | PROCESS_VM_READ privileges\n");


	tdNtReadVirtualMemory NtReadVirtualMem = 0;
	NtReadVirtualMem = (tdNtReadVirtualMemory)GetProcAddress(hMod, "NtReadVirtualMemory");

	/* going to call ntqueryinformationprocess to get a pointer to the peb in order to grab the base address */

	tdNtQueryInformationProcess NtQueryInfoProc = 0;
	NtQueryInfoProc = (tdNtQueryInformationProcess)GetProcAddress(hMod, "NtQueryInformationProcess");

	PROCESS_BASIC_INFORMATION pbi = { 0 };

	PEB peb = { 0 };//copy of the peb at the first level

	status = NtQueryInfoProc(processhandle, ProcessBasicInformation, &pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);
	if (status > 0) {
		printf("\n NtQueryInformationProcess failed.\n");
	}

	printf("\nPbi - peb base address: %p\n", pbi.PebBaseAddress);

	status = NtReadVirtualMem(processhandle, pbi.PebBaseAddress, &peb, sizeof(PEB), NULL);
	if (status != 0) {
		std::cout << "\nFailed to retrieve initial PEB copy" << std::endl;
	}

	std::cout << "\n Peb - Image Base Address: " << peb.ImageBaseAddress << "\n";

	PEB_LDR_DATA pebloader = { 0 }; //copy the loader which contains the head of the linked list to get our first forward flink

	status = NtReadVirtualMem(processhandle, (BYTE*)peb.Ldr, &pebloader, sizeof(PEB_LDR_DATA), NULL);

	uintptr_t listhead = (uintptr_t)pebloader.InMemoryOrderModuleList.Flink; 

	uintptr_t nextitem = listhead;

	LDR_MODULE loadermod = { 0 };//loadermod structure to read every list entry

	do {

		status = NtReadVirtualMem(processhandle, CONTAINING_RECORD(nextitem, LDR_MODULE, InMemoryOrderModuleList), &loadermod, sizeof(LDR_MODULE), NULL);

		if (loadermod.SizeOfImage == 0) {
			break;
		}

		wchar_t* dllnames = new wchar_t[loadermod.FullDllName.MaximumLength];
		status = NtReadVirtualMem(processhandle, loadermod.FullDllName.Buffer, dllnames, loadermod.FullDllName.MaximumLength, NULL);
		if (status != 0) {
			std::cout << "\nFailed to raed loadermod.FullDllName.Buffer" << std::endl;
		}

		std::wcout << "\n" << dllnames << std::endl;

		/* get mod names, module base address, and size, then push back to vectors in the nested structure */
		info.ModuleName.push_back(dllnames);

		std::cout << "\nModule Base Add: " << loadermod.BaseAddress << std::endl;
		info.ModuleBaseAdd.push_back(reinterpret_cast<uintptr_t>(loadermod.BaseAddress));

		std::cout << "\nSize of Module: " << loadermod.SizeOfImage << std::endl;
		info.ModuleSize.push_back(loadermod.SizeOfImage);

		nextitem = (uintptr_t)loadermod.InMemoryOrderModuleList.Flink;

		delete[] dllnames;

	} while (nextitem != listhead);
	

	tNtClose(processhandle);

	FreeLibrary(hMod);
}


void NtPatternClass::IterateModules(uintptr_t procid, char* pattern) {


	HMODULE hMod = LoadLibraryW(L"ntdll.dll");

	tdNtOpenProcess NtOpenProc = 0;
	NtOpenProc = (tdNtOpenProcess)GetProcAddress(hMod, "NtOpenProcess");

	tdNtClose tNtClose = 0;
	tNtClose = (tdNtClose)GetProcAddress(hMod, "NtClose");

	tdNtQueryVirtualMemory NtQueryVirtualMem = 0;
	NtQueryVirtualMem = (tdNtQueryVirtualMemory)GetProcAddress(hMod, "NtQueryVirtualMemory");

	tdNtReadVirtualMemory NtReadVirtualMem = 0;
	NtReadVirtualMem = (tdNtReadVirtualMemory)GetProcAddress(hMod, "NtReadVirtualMemory");

	//setup for NtOpenProcess

	CLIENT_ID pid = { 0 };
	pid.UniqueProcess = (PDWORD)procid;
	pid.UniqueThread = NULL;

	HANDLE processhandle = nullptr;

	static OBJECT_ATTRIBUTES soa = { sizeof(soa) };

	NTSTATUS status = NtOpenProc(&processhandle, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, &soa, &pid);
	if (status != 0) {
		std::cout << "\n Failed to open process handle with PROCESS_QUERY_INFORMATION | PROCESS_VM_READ rights";
	}
	std::cout << "\n Process handle to " << procid << " opened." << std::endl;


	for (int i = 0; i < info.ModuleBaseAdd.size(); i++) {
		
		info.base = info.ModuleBaseAdd[i];
		std::wcout << "Module Names: " << info.ModuleName[i] << std::endl;

		//call mbi on the starting address and fill an array with the first page returned

		uintptr_t currentAdd = info.base;
		uintptr_t end = info.base + info.ModuleSize[i];

		char* memory = { 0 };

		do {
			MEMORY_BASIC_INFORMATION mbi = { 0 };

			//NtQueryVirtualMemory and fill mbi
			status = NtQueryVirtualMem(processhandle, 
				reinterpret_cast<void*>(currentAdd), 
				MemoryBasicInformation,
				&mbi, 
				sizeof(MEMORY_BASIC_INFORMATION), NULL);
			if (status != 0) {
				std::cout << "\n Failed to retrieve MemoryBasicInformation" << std::endl;
				return;
			}
			if (mbi.Protect == PAGE_NOACCESS || mbi.Protect == PAGE_GUARD) {
				continue;//skip this page and move to the next iteration
			}
			//Could call NtProtectVirtualMemory here to change page access protections 

			if (mbi.State == MEM_COMMIT) {//if the state is mem_commit it's time to read
				if (memory != NULL) {
					delete[] memory;			
				}
				memory = new char[mbi.RegionSize];
				status = NtReadVirtualMem(processhandle, reinterpret_cast<void*>(info.base), memory, mbi.RegionSize, NULL);

				LocateBytes(pattern, memory, mbi.RegionSize, currentAdd);
			}


			//increment the size of the page returned until we are at the end of the module
			currentAdd += mbi.RegionSize;

		} while (currentAdd != end);
		if (memory != NULL) {
			delete[] memory;
		}
	}
	FreeLibrary(hMod);
	tNtClose(processhandle);
}


void NtPatternClass::LocateBytes(char* pattern, char* arr, size_t arrsize, uintptr_t currentAdd) {

	std::vector<char>memblock(arr, arr + arrsize);

	auto predicate = [](char memblock, char substr) {
		return (memblock == substr || substr == '?'); //todo: fix wildcards not matching
	};

	std::string_view substr = pattern;
	
	auto it = std::search(memblock.begin(), memblock.end(), std::boyer_moore_searcher(substr.begin(), substr.end(), predicate));
	if (it != memblock.end()) {

		//calculate rva of offset and return

		uintptr_t foundbytes = (currentAdd - info.base) + (it - memblock.begin());

		std::cout << "\nBytes found at offset from base: " << std::hex << foundbytes << std::endl;

		return;
	}

}