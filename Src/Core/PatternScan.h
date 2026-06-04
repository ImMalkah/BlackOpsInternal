#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <cstring>
#include <cstdlib>
#include <Psapi.h>

#pragma comment(lib, "psapi.lib")

namespace PatternScan
{
    // Scans a specific module for a given byte pattern
    inline std::uintptr_t Find(const char* moduleName, const char* pattern)
    {
        HMODULE hModule = GetModuleHandleA(moduleName);
        if (!hModule)
        {
            return 0;
        }

        MODULEINFO moduleInfo = {};
        if (!GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(moduleInfo)))
        {
            return 0;
        }

        // Parse signature string into a vector of bytes/wildcards
        std::vector<int> patternBytes;
        const char* start = pattern;
        const char* end = pattern + std::strlen(pattern);

        for (const char* current = start; current < end; ++current)
        {
            if (*current == ' ')
            {
                continue;
            }
            if (*current == '?')
            {
                ++current;
                if (*current == '?')
                {
                    ++current;
                }
                patternBytes.push_back(-1); // -1 represents wildcard
            }
            else
            {
                patternBytes.push_back(std::strtoul(current, const_cast<char**>(&current), 16));
            }
        }

        std::uintptr_t scanStart = reinterpret_cast<std::uintptr_t>(moduleInfo.lpBaseOfDll);
        std::uintptr_t scanEnd = scanStart + moduleInfo.SizeOfImage;
        size_t patternSize = patternBytes.size();

        if (patternSize == 0)
        {
            return 0;
        }

        // Loop through module memory
        for (std::uintptr_t i = scanStart; i < scanEnd - patternSize; ++i)
        {
            bool found = true;
            for (size_t j = 0; j < patternSize; ++j)
            {
                if (patternBytes[j] != -1 && *reinterpret_cast<std::uint8_t*>(i + j) != patternBytes[j])
                {
                    found = false;
                    break;
                }
            }
            if (found)
            {
                return i;
            }
        }
        return 0;
    }
}
