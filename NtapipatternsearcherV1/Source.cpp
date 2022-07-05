#pragma once
#include "Class.h"

int main(void) {
	NtPatternClass find;

	uintptr_t procid = 0;

	procid = find.FindId(L"notepad.exe");

	std::cout << "\nProcess Id retrieved: " << procid << std::endl;

	find.GetRemotePEBmodules(procid); //get remote module information and fill the info struct so we can proceed to searching for a pattern

	std::wcout << "\nFirst Module: " << find.info.ModuleName[0] << std::endl;

	//search for the pattern using module base + size 

	//example pattern
	char* pattern = new char[] {"\x4d\x5a\x90\x00"};//todo: write function to parse user input and format to byte array


	std::cout << "\n pattern " << pattern << std::endl;

	find.IterateModules(procid, pattern);

	delete[] pattern;
}