
#include "Volts.h"
#include "Volts/Core/Utilities/Decrypt/UNSELF.h"
using namespace Volts;

int main()
{
	FS::BufferedFile F = {"./EBOOT.BIN"};
	UNSELF::DecryptSELF(F);
	return 0;
}
