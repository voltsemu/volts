#include "Memory.h"
#include "Meta/Macros.h"
#include "Core/Memory/Memory.h"

#if OS_WINDOWS
#   include <Windows.h>
#else
#   include <sys/mman.h>
#endif

namespace Volts::PS3
{
    Cthulhu::Byte* GlobalMemory = nullptr;

    // this allocates the global memory pointer as a 512 block of memory
    // this memory can be read to, written to and can also be executed
    // the memory is then 0 initialized to prevent any sort of undefined behaviour 
    // possibly happening
    void InitGlobalMemory()
    {
#if OS_WINDOWS
        GlobalMemory = (Cthulhu::Byte*)VirtualAlloc(nullptr, GlobalMemorySize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
        // this zeros memory for us
#else
        GlobalMemory = mmap(nullptr, GlobalMemorySize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, -1, 0);
        Memory::Zero(GlobalMemory, GlobalMemorySize);
        // zero the memory
#endif
    }

    // delete the global memory pool
    void CleanupGlobalMemory()
    {
#if OS_WINDOWS
        VirtualFree(GlobalMemory, GlobalMemorySize, MEM_DECOMMIT);
#else
        munmap(GlobalMemory, GlobalMemorySize);
#endif
    }
}