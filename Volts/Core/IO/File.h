#pragma once

#include <Meta/Macros.h>

#if OS_WINDOWS
#   include <fileapi.h>
#else
#   include <stdio.h>
#endif

#if 0
    only this works on windows, idk why. something about posix
	HANDLE File = CreateFileA(
		"volts_output.bin",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);

	FileSystem::BufferedFile F{"C:\\Users\\Elliot\\source\\repos\\RPCS3X\\Build\\EBOOT.BIN"};
	auto S = UNSELF::DecryptSELF(F).Get();
	WriteFile(
		File,
		S.GetData(),
		S.Len(),
		nullptr,
		nullptr
	);

	FindClose(File);

#endif

namespace Volts
{
}