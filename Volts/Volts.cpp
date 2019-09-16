
#include "Volts.h"

#include "PS3/Util/Decrypt/UNSELF.h"
#include "PS3/VM/Interpreter/PPUInterpreter.h"
#include "PS3/VM/Render/Frame.h"
#include "PS3/VM/Render/Backends.h"

#include "Core/IO/File.h"

#include "GUI/Frame.h"

#include <imgui.h>

using namespace Volts;
using namespace Volts::PS3;
using namespace Cthulhu;

int main(int Argc, const char** Argv)
{
	GUI::Frame{500, 500, 500, 500, "Volts"};
	LogLevel = Level::Trace;
	Volts::Close();
}


#if OS_WINDOWS

int wWinMain(
	HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPWSTR CmdLine,
	int ShowCmd
)
{
	RSX::CmdShow = ShowCmd;
	RSX::Instance = Instance;
	GUI::Instance = Instance;
	int Argc;
	wchar_t** Argv = CommandLineToArgvW(GetCommandLineW(), &Argc);

	main(Argc, (const char**)Argv);

	LocalFree(Argv);
}

#endif