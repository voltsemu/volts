#pragma once

#include <map>
#include <string>
#include <variant>

namespace RPCS3X
{
    using U8 = unsigned char;
    using U16 = unsigned short;
    using U32 = unsigned int;
    using U64 = unsigned long long;

    using I8 = signed char;
    using I16 = signed short;
    using I32 = signed int;
    using I64 = signed long long;

    using F32 = float;
    using F64 = double;

    using String = std::string;
    
    template<typename K, typename V>
    using Map = std::map<K, V>;

    template<typename T, typename E>
    using Result = std::result<T, E>;

    template<typename... T>
    using Variant = std::variant<T...>;
}