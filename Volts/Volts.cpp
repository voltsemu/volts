
#include "Volts.h"

#include "PS3/Util/Decrypt/UNSELF.h"
#include "PS3/VM/Interpreter/PPUInterpreter.h"
#include "PS3/VM/Render/Frame.h"
#include "PS3/VM/Render/Backends.h"

#include "Core/IO/File.h"

using namespace Volts;
using namespace Volts::PS3;
using namespace Cthulhu;

int Entry(int argc, const char** argv)
{
	LogLevel = Level::Trace;

	Volts::Close();

	return 0;
}
