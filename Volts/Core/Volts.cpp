
#include "Volts.h"
#include "Volts/Core/Utilities/Decrypt/UNSELF.h"

namespace Volts
{

	int VoltsMain()
	{
		FS::BufferedFile F = {"./EBOOT.BIN"};
		UNSELF::DecryptSELF(F);
		return 0;
	}

}