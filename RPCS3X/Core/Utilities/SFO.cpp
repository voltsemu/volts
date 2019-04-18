#include "SFO.h"

using namespace RPCS3X;
using namespace RPCS3X::SFO;

Value::~Value()
{
    if (Type == String || Type == Array)
        delete Str;
}

Variant<SFO, Error> SFO::Load(const String& Path)
{
    Header Stats;
}