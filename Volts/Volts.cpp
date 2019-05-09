
#include "Volts.h"

#include "PS3/Util/Decrypt/UNSELF.h"
#include "Core/Logger/Logger.h"

using namespace Volts::PS3;
using namespace Volts;

// entry point, nothing here yet as right now we're just testing very basic partss
int main(int argc, const char** argv)
{
	LogLevel = Level::Trace;
	FS::BufferedFile F{argv[1]};
	auto Data = UNSELF::DecryptSELF(F);
	printf("Here\n");
}
