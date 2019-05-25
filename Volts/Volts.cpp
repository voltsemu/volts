
#include "Volts.h"

#include "PS3/Util/Decrypt/UNSELF.h"
#include "PS3/Util/PUP.h"
#include "Core/SIMD/SIMD.h"

using namespace Volts;
using namespace Volts::PS3;
using namespace Cthulhu;

// entry point, nothing here yet as right now we're just testing very basic parts
int main(int argc, const char** argv)
{
	LogLevel = Level::Trace;
	Cthulhu::FileSystem::BufferedFile F = String(argv[1]);
	PUP::Format A(&F);

	if(A.Parse())
	{
		auto U = A.GetFile(PUP::UpdateFiles);

		
	}
	//auto A = UNSELF::DecryptSELF(F);
	//FILE* O = fopen("output.elf", "wb");
	//fwrite(A.Get().GetData(), sizeof(Cthulhu::Byte), A.Get().GetLength(), O);
	//fclose(O);
}
