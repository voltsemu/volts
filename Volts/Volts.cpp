
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

	FILE* Out = fopen("volts.elf", "wb");
	fwrite(U.Get().GetData(), sizeof(Byte), U.Get().Len(), Out);

	fseek(Out, 4310, SEEK_SET);
	U8 AAA[10];
	fread(AAA, 1, 10, Out);
	for(U32 I = 0; I < 10; I++)
	{
		printf("--- %u\n", AAA[I]);
	}
	fclose(Out);

	//auto B = U.Get();

	//Byte* Buf = new Byte[F.Size()];
	//F.ReadN(Buf, F.Size());

	//B.WriteN(Buf, F.Size());
	//PPUInterpreter Interp;
	//Interp.Run(B);
}
