
#include "Volts.h"

#include "PS3/Util/Decrypt/UNSELF.h"
#include "Core/Files/TAR.h"
#include "Core/SIMD/SIMD.h"

using namespace Volts;
using namespace Volts::PS3;
using namespace Cthulhu;

// entry point, nothing here yet as right now we're just testing very basic parts
int main(int argc, const char** argv)
{
	LogLevel = Level::Trace;
	FileSystem::BufferedFile F(argv[1]);

	auto U = UNSELF::DecryptSELF(F);
	FILE* File = fopen("out2.elf", "w");

	//if(File == nullptr)
	//{
	//	printf("REEEEE\n");
	//	return 1;
	//}

	auto S = U.Get();
	fseek(File, 0, SEEK_SET);
	fwrite(S.GetData(), sizeof(Byte), S.Len(), File);
	fclose(File);
}
