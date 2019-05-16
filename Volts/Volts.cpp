
#include "Volts.h"

#include "Core/Collections/Array.h"

using namespace Volts;

Cthulhu::Array<Volts::Emulator*> Emulators = {};

Cthulhu::Empty Volts::AddEmulator(Volts::Emulator* Emu)
{
	Emulators.Append(Emu);
	return {};
}

const char* Volts::Stub()
{
	return "Yeet";
}

#if !OS_WINDOWS

// entry point, nothing here yet as right now we're just testing very basic partss
int main(int argc, const char** argv)
{
	LogLevel = Level::Trace;
}

#endif