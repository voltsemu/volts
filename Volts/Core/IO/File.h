#pragma once

#include <Meta/Macros.h>

#include <Core/Collections/CthulhuString.h>

#if OS_WINDOWS
#   include <fileapi.h>
#else
#   include <stdio.h>
#endif

namespace Volts
{
#if OS_WINDOWS
	using FileHandle = HANDLE;
#elif OS_LINUX
	using FileHandle = int;
#elif OS_APPLE
	using FileHandle = FILE*;
#endif

	FileHandle OpenFile(const Cthulhu::String& Path);
	void CloseFile(FileHandle Handle);
	void WriteFile(FileHandle Handle, Cthulhu::Byte* Data, Cthulhu::U32 Len);
}