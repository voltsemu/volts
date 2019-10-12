// enter here
int main(int Argc, const char** Argv)
{
	// run the main window	
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
	int Argc;
	wchar_t** Argv = CommandLineToArgvW(GetCommandLineW(), &Argc);

	main(Argc, (const char**)Argv);

	LocalFree(Argv);
}

#endif