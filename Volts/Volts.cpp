
#include "Volts.h"

#include "PS3/Util/Decrypt/UNSELF.h"
#include "PS3/VM/Interpreter/PPUInterpreter.h"
#include "PS3/VM/Render/Frame.h"
#include "PS3/VM/Render/Backends.h"

#include "Core/IO/File.h"

using namespace Volts;
using namespace Volts::PS3;
using namespace Cthulhu;

#if OS_WINDOWS
#	include <windows.h>
extern "C" BOOL WINAPI DllMain(
	HINSTANCE const Instance,
	DWORD const Reason,
	LPVOID const Reserved
)
{
	switch(Reason)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#endif

void Load(LogFunc Func)
{
	Func("Jeff", 0);
}

void Unload()
{
	Volts::Close();
}
