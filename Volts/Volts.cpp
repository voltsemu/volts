
#include "Volts.h"

#include "PS3/Util/Decrypt/UNSELF.h"
#include "PS3/VM/Interpreter/PPUInterpreter.h"
#include "PS3/VM/Render/Frame.h"
#include "PS3/VM/Render/Backends.h"

#include "Core/IO/File.h"
using namespace Volts;
using namespace Volts::PS3;
using namespace Cthulhu;

// entry point, nothing here yet as right now we're just testing very basic parts
int main(int argc, const char** argv)
{
	LogLevel = Level::Trace;

	auto Backends = RSX::GetBackends();

	for(auto Backend : Backends)
	{
		if(strcmp(Backend->Name(), "Metal") == 0)
		{
			unsigned Count = 0;
			auto* Devices = Backend->Devices(Count);
			for(U32 I = 0; I < Count; I++)
			{
				printf("%ls\n", Devices[I].Name().c_str());
			}
			Backend->Init(Devices);
			printf("Jeff\n");
		}
	}
	
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
