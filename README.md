# NtapipatternsearcherV1

A user mode byte pattern searcher using functions from ntdll.dll and standard input-output stream, for research and learning purposes

Usage:
Give this program a byte pattern, something like char* pattern = new char["\x4d\x5a\x90\x00"], and it returns all occurences of the matching pattern found in the memory of a remote process one module at a time.
