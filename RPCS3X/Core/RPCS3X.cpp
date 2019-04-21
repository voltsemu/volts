
#include "RPCS3X.h"

#include "Render/Render.h" 
#include "Render/Null/Render.h"
#include <iostream>
using namespace RPCS3X;

int main()
{
    std::cout << "Thing" << std::endl;
    for (auto& I : RSX::Backends)
        std::cout << I->Name() << I->Description() << std::endl;

	return 0;
}
