
#include "Volts.h"

#include "PS3/Util/Decrypt/UNSELF.h"
#include "PS3/Util/PUP.h"
#include "Core/Files/TAR.h"
#include "Core/SIMD/SIMD.h"

using namespace Volts;
using namespace Volts::PS3;
using namespace Cthulhu;

// entry point, nothing here yet as right now we're just testing very basic parts
int main(int argc, const char** argv)
{
	LogLevel = Level::Trace;
	//Cthulhu::FileSystem::BufferedFile F = String(argv[1]);
	//PUP::Format A(&F);

	/*if(A.Parse())
	{
		auto U = A.GetFile(PUP::UpdateFiles);
        LOGF_DEBUG(TAR, "Bin = %u %u %u %u %u", U.GetData()[0], U.GetData()[1], U.GetData()[2], U.GetData()[3], U.GetData()[4]);

		TAR::Format TARFile(&U);
		for(auto& Name : TARFile.Filenames())
		{
			LOGF_DEBUG(A, "%s", Name.CStr());
			auto Data = TARFile.GetFile(Name);
		}
	}*/
}
