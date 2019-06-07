
#include <unistd.h>

#include "Volts.h"

#include "PS3/Util/Decrypt/UNSELF.h"
#include "PS3/VM/Interpreter/PPUInterpreter.h"

using namespace Volts;
using namespace Volts::PS3;
using namespace Cthulhu;

// entry point, nothing here yet as right now we're just testing very basic parts
int main(int argc, const char** argv)
{
	LogLevel = Level::Trace;
	FileSystem::BufferedFile F(argv[1]);

	auto U = UNSELF::DecryptSELF(F);

	FILE* DE = fopen("volts.txt", "wt");
	for(U32 I = 0; I < U.Get().Len(); I++)
	{
		fprintf(DE, "idx %u val: %u :end\n", I, U.Get().GetData()[I]);
	}
	fclose(DE);

	FILE* Out = fopen("volts.elf", "wb");
	write(fileno(Out), U.Get().GetData(), U.Get().Len());
	fclose(Out);
}
