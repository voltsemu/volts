
#include "Volts.h"

#include "Core/IO/File.h"

#include "GUI/Frame.h"

#include <imgui.h>

using namespace Volts;
using namespace Cthulhu;

int main(int Argc, const char** Argv)
{
	GUI::Frame()
		.Title("Volts")
		.Run([]{

		});
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
	GUI::Instance = Instance;
	int Argc;
	wchar_t** Argv = CommandLineToArgvW(GetCommandLineW(), &Argc);

	main(Argc, (const char**)Argv);

	LocalFree(Argv);
}

#endif