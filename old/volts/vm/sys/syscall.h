#pragma once

#include <array>

namespace volts::vm
{
    using syscall_table_t = std::array<void*, 1024>;
    extern syscall_table_t syscall_table;
    syscall_table_t* get_syscall_table();
}