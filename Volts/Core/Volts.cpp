
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

	Log::LogLevel = Volts::Level::Trace;

	Log::Fatal("SPU", "Train go boom");
	Log::Error("Main", "Starting...");
	Log::Warning("RSX", "Yeet");
	Log::Info("Something", "Some Info");
	Log::Debug("Other", "Name Jeff");
	Log::Trace("Trace", "Some Function Call");
	return 0;
}
