
#include "Volts.h"

#include "Core/Collections/Array.h"
#include "PS3/Util/Decrypt/UNSELF.h"

using namespace Volts;
using namespace Volts::PS3;

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

//#if !OS_WINDOWS

// entry point, nothing here yet as right now we're just testing very basic partss
int main(int argc, const char** argv)
{
	LogLevel = Level::Trace;
	auto F = Cthulhu::FileSystem::BufferedFile{argv[1]};
	auto A = UNSELF::DecryptSELF(F);
	FILE* O = fopen("output.elf", "wb");
	fwrite(A.Get().GetData(), sizeof(Cthulhu::Byte), A.Get().GetLength(), O);
	fclose(O);
}

//#endif