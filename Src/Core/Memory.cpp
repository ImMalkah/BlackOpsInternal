#include "Memory.h"
#include <cstring>

namespace Memory
{
    uintptr_t FindAddress(uintptr_t baseAddress, const std::vector<unsigned int>& offsets)
    {
        uintptr_t addr = baseAddress;
        for (unsigned int offset : offsets)
        {
            __try
            {
                if (addr == 0)
                    return 0;
                addr = *reinterpret_cast<uintptr_t*>(addr);
                addr += offset;
            }
            __except (EXCEPTION_EXECUTE_HANDLER)
            {
                return 0;
            }
        }
        return addr;
    }

    bool Patch(void* dst, void* src, size_t size)
    {
        DWORD oldProtect;
        if (!VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect))
            return false;

        __try
        {
            std::memcpy(dst, src, size);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            VirtualProtect(dst, size, oldProtect, &oldProtect);
            return false;
        }

        VirtualProtect(dst, size, oldProtect, &oldProtect);
        return true;
    }

    bool Nop(void* dst, size_t size)
    {
        DWORD oldProtect;
        if (!VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldProtect))
            return false;

        __try
        {
            std::memset(dst, 0x90, size);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            VirtualProtect(dst, size, oldProtect, &oldProtect);
            return false;
        }

        VirtualProtect(dst, size, oldProtect, &oldProtect);
        return true;
    }
}
