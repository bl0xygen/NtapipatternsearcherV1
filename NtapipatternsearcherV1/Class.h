#pragma once
#ifndef CLASS_H
#define CLASS_H

#include "typedefs.h"


class NtPatternClass
{
public:
	struct Info {
		uintptr_t base = 0;
		std::vector<std::wstring> ModuleName;
		std::vector<uintptr_t> ModuleBaseAdd;
		std::vector<size_t> ModuleSize;
	}; Info info;
	uintptr_t FindId(const wchar_t* processname);
	void GetRemotePEBmodules(uintptr_t procid);
	void IterateModules(uintptr_t procid, char* pattern);
private:
	void LocateBytes(char* pattern, char* arr, size_t arrsize, uintptr_t currentAdd);

	

}; //NtPatternClass find;


#endif