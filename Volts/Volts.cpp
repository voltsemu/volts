
#include "Volts.h"

#include "PS3/Util/Decrypt/UNSELF.h"
#include "PS3/VM/Interpreter/PPUInterpreter.h"
#include "PS3/VM/Render/Frame.h"
//#include "PS3/VM/Render/Vulkan/Render.h"
//#include "PS3/VM/Render/DirectX12/Render.h"
#include "PS3/VM/Render/Backends.h"

#if OS_WINDOWS
#	include <WinBase.h>
#	include <shellapi.h>
#endif

using namespace Volts;
using namespace Volts::PS3;
using namespace Cthulhu;

//static auto* VKSingleton = new RSX::Vulkan();

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
		MessageBox(nullptr, Render->Name(), "Volts", 0);
		if(!Render->RequiresDevice())
			continue;

		MessageBox(nullptr, "Here", "Volts", 0);

		U32 Count = 0;
		MessageBox(nullptr, "Here3", "Volts", 0);
		Render->Init();
		MessageBox(nullptr, "Here4", "Volts", 0);
		RSX::RenderDevice* Devices = Render->Devices(Count);
		MessageBox(nullptr, "Here2", "Volts", 0);
		MessageBox(nullptr, (std::string("Count ") + std::to_string(Count)).c_str(), Render->Name(), 0);
		for(U32 I = 0; I < Count; I++)
			MessageBox(nullptr, Devices[I].Name(), "Volts", 0);
	}
}
#endif
