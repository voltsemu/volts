
#include "Volts.h"
#include "Volts/Core/Utilities/Decrypt/UNSELF.h"

#include "../Platform/Interface/EmulatorWindow.h"

namespace FS = Cthulhu::FileSystem;

using namespace Volts;

int VoltsMain(void* Handle)
{
	auto* Win = (EmulatorWindow*)Handle;
	Win->WriteLog("Working in volts", 0, "");
	return 0;
}
