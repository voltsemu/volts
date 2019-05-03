
#include "Utilities/Logger/Logger.h"

#include "Volts.h"
#include "Volts/Core/Utilities/Decrypt/UNSELF.h"

namespace FS = Cthulhu::FileSystem;

using namespace Volts;

int main(int argc, const char** argv)
{
	Volts::LogLevel = Level::Trace;
	FS::BufferedFile Boot(argv[1]);
	UNSELF::DecryptSELF(Boot);
}
