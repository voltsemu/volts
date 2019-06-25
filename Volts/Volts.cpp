
#include "Volts.h"

#include "PS3/Util/Decrypt/UNSELF.h"
#include "PS3/VM/Interpreter/PPUInterpreter.h"
#include "PS3/VM/Render/Frame.h"
#include "PS3/VM/Render/Backends.h"

#if OS_WINDOWS
#	include <WinBase.h>
#	include <shellapi.h>
#endif

using namespace Volts;
using namespace Volts::PS3;
using namespace Cthulhu;

// entry point, nothing here yet as right now we're just testing very basic parts
int main(int argc, const char** argv)
{
	LogLevel = Level::Trace;
	//FileSystem::BufferedFile F(argv[1]);

	for(auto Render : RSX::GetBackends())
	{
		printf("%s\n", Render->Name());
	}
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
	LogLevel = Level::Trace;

	for(auto* Render : RSX::GetBackends())
	{
		if(!Render->RequiresDevice())
			continue;

		U32 Count = 0;
		RSX::RenderDevice* Devices = Render->Devices(Count);
		for(U32 I = 0; I < Count; I++)
			MessageBoxW(nullptr, Devices[I].Name().c_str(), L"Volts", 0);
	}
}
#endif
