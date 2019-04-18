#include "SFO.h"

using namespace RPCS3X;
using namespace RPCS3X::SFO;

Value::~Value()
{
    if (Type == Format::String || Type == Format::Array)
        delete Str;
}

Result<RPCS3X::SFO::SFO, Error> RPCS3X::SFO::SFO::Load(const String& Path)
{
    Header Stats;
}