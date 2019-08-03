
#include "Volts.h"

#include "PS3/Util/Decrypt/UNSELF.h"
#include "PS3/VM/Interpreter/PPUInterpreter.h"
#include "PS3/VM/Render/Frame.h"
#include "PS3/VM/Render/Backends.h"

#if OS_WINDOWS
#	include <WinBase.h>
#	include <shellapi.h>
#	include <io.h>
#	include <fileapi.h>
#	include <stdio.h>
#	include <stdlib.h>
#	include <fcntl.h>
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <errno.h>
#	include <share.h>
#endif

using namespace Volts;
using namespace Volts::PS3;
using namespace Cthulhu;

// entry point, nothing here yet as right now we're just testing very basic parts
int main(int argc, const char** argv)
{
	LogLevel = Level::Trace;

	HANDLE H = CreateFileA(
		"volts_output_win32.elf",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);

	auto* H2 = fopen("volts_output_posix.elf", "wb");

	auto H3 = open("volts_output_ll_posix.elf", _O_WRONLY | _O_CREAT, 0777);

	FileSystem::BufferedFile F{"C:\\Users\\Elliot\\source\\repos\\RPCS3X\\Build\\EBOOT.BIN"};
	auto S = UNSELF::DecryptSELF(F).Get();

	WriteFile(
		H,
		S.GetData(),
		S.Len(),
		nullptr,
		nullptr
	);

	fwrite(S.GetData(), 1, S.Len(), H2);

	write(H3, S.GetData(), S.Len());

	FindClose(H);

	fclose(H2);

	close(H3);

	Volts::Close();
}

// windows specific entry point because windows does some funny stuff around windowing and the like
#if OS_WINDOWS
int APIENTRY wWinMain(
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
