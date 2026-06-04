#pragma once
#include <Windows.h>
#include <vector>

namespace Memory
{
    // Resolves a base address and offset list to the target address
    uintptr_t FindAddress(uintptr_t baseAddress, const std::vector<unsigned int>& offsets);

    // Safely reads memory using Structured Exception Handling (SEH)
    template <typename T>
    inline T Read(uintptr_t address, const T& fallback = T{})
    {
        __try
        {
            if (address == 0)
                return fallback;
            return *reinterpret_cast<T*>(address);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            return fallback;
        }
    }

    // Safely writes memory using SEH
    template <typename T>
    inline bool Write(uintptr_t address, const T& value)
    {
        __try
        {
            if (address == 0)
                return false;
            *reinterpret_cast<T*>(address) = value;
            return true;
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            return false;
        }
    }

    // Patches write-protected memory (e.g. game instructions)
    bool Patch(void* dst, void* src, size_t size);

    // Replaces instructions with NOPs (0x90)
    bool Nop(void* dst, size_t size);
}
