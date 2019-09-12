
#include "Volts.h"

#include "PS3/Util/Decrypt/UNSELF.h"
#include "PS3/VM/Interpreter/PPUInterpreter.h"
#include "PS3/VM/Render/Frame.h"
#include "PS3/VM/Render/Backends.h"

#include "Core/IO/File.h"

using namespace Volts;
using namespace Volts::PS3;
using namespace Cthulhu;

#if OS_WINDOWS
BOOL APIENTRY DllMain(HANDLE hModule, DWORD Reason, void* Something) { return TRUE; }
#endif

void Load()
{

}

void Unload()
{
	Volts::Close();
}
