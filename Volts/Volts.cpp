#include "Volts.h"

using namespace Cthulhu;

#include "ui.h"

// enter here
int main(int Argc, char** Argv)
{
	Volts::Args::CLI::Get()
		->Build()
		->Run(Argc, Argv);

	uiInitOptions Opts = {};
	uiInit(&Opts);

	auto* W = uiNewWindow("Volts", 720, 480, 1);
	uiWindowSetMargined(W, 1);

	uiWindowOnClosing(W, [](uiWindow*, void*){
		uiQuit();
		return 1;
	}, nullptr);

	uiOnShouldQuit([](void* Data){
		uiControlDestroy(uiControl(uiWindow(Data)));
		return 1;
	}, W);

	uiWindowCenter(W);

	uiControlShow(uiControl(W));
	uiMain();

	//Volts::Emulator::Get()->Run();
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

	char** RealArgv = (char**)alloca(sizeof(char*) * Argc);

	for(U32 I = 0; I < Argc; I++)
	{
		auto Len = wcslen(Argv[I]);
		char* Arg = new char[Len + 1]();
		wcstombs(Arg, Argv[I], Len);
		RealArgv[I] = Arg;
	}

	main(Argc, (char**)RealArgv);

	LocalFree(Argv);
}

#endif