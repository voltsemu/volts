
#include "Volts.h"

#include "PS3/Util/Decrypt/UNSELF.h"
#include "Core/SIMD/SIMD.h"

using namespace Volts;
using namespace Volts::PS3;
using namespace Cthulhu;

// entry point, nothing here yet as right now we're just testing very basic parts
int main(int argc, const char** argv)
{
	LogLevel = Level::Trace;
	auto F = Cthulhu::FileSystem::BufferedFile{argv[1]};
	auto A = UNSELF::DecryptSELF(F);
	FILE* O = fopen("output.elf", "wb");
	fwrite(A.Get().GetData(), sizeof(Cthulhu::Byte), A.Get().GetLength(), O);
	fclose(O);
}
