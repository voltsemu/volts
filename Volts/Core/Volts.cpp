
#include "Utilities/Logger/Logger.h"

#include "Volts.h"
#include "Volts/Core/Utilities/Decrypt/UNSELF.h"

#include "../Platform/Interface/Native.h"

namespace FS = Cthulhu::FileSystem;

using namespace Volts;

Volts::Native* Volts::Window = nullptr;

int VoltsMain(void* Handle)
{
	auto* Win = (Native*)Handle;
	Volts::Window = Win;

	Log::Error("Main", "Starting...");
	return 0;
}
