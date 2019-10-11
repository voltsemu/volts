
#include "Volts.h"

#include "GUI/Frame.h"

#include "Core/Emulator.h"

using namespace Volts;
using namespace Cthulhu;

// enter here
int main(int Argc, const char** Argv)
{
	// run the main window
	Emulator::Get().Run();
}


#if OS_WINDOWS
// on windows we need to use wWinMain to get an hInstance so we can make win32 windows and stuff
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