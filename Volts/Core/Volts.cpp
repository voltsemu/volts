
#include "Utilities/Logger/Logger.h"

#include "Volts.h"
#include "Volts/Core/Utilities/Decrypt/UNSELF.h"

#include "../Platform/Interface/EmulatorWindow.h"

namespace FS = Cthulhu::FileSystem;

using namespace Volts;

Volts::EmulatorWindow* Volts::Window = nullptr;

int VoltsMain(void* Handle)
{
	auto* Win = (EmulatorWindow*)Handle;
	Volts::Window = Win;

	Log::Error("Main", "Starting...");
	return 0;
}
