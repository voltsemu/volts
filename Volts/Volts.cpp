
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
}

// entry point, nothing here yet as right now we're just testing very basic parts
int main(int argc, const char** argv)
{
	LogLevel = Level::Trace;

	Volts::Close();
}

// windows specific entry point because windows does some funny stuff around windowing and the like
#if OS_WINDOWS
int __stdcall wWinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine,
	int nShowCmd
)
{
	RSX::CmdShow = nShowCmd;
	RSX::Instance = hInstance;
	int Argc;
	wchar_t** Argv = CommandLineToArgvW(GetCommandLineW(), &Argc);

	// call main with the args
	main(Argc, (const char**)Argv);

	LocalFree(Argv);
}
#endif
