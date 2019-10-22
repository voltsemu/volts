#include "Volts.h"

// enter here
int main(int Argc, char** Argv)
{
	Volts::Args::CLI::Get()
		->Build()
		->Run(Argc, Argv);
	Volts::Emulator::Get()->Run();
}


#if OS_WINDOWS

#include <windows.h>

// on windows we need to use wWinMain to get an hInstance so we can make win32 windows and stuff
int APIENTRY wWinMain(
	HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPWSTR CmdLine,
	int ShowCmd
)
{
	int Argc;
	wchar_t** Argv = CommandLineToArgvW(GetCommandLineW(), &Argc);

	main(Argc, (const char**)Argv);

	LocalFree(Argv);
}

#endif