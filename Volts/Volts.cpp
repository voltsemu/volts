
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

	FileSystem::BufferedFile F{"C:\\Users\\Elliot\\source\\repos\\RPCS3X\\Build\\EBOOT.BIN"};
	auto S = UNSELF::DecryptSELF(F).Get();
	
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
