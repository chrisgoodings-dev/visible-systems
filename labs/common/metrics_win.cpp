#pragma once

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <psapi.h>
#include <intrin.h>

#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

namespace vs {

struct WinSystemInfo {
    std::string cpu_brand;
    std::uint32_t logical_cores = 0;
};

inline WinSystemInfo get_system_info() {
    WinSystemInfo info{};

    // CPU brand string
    int regs[4] = {0,0,0,0};
    char brand[49] = {};
    __cpuid(regs, 0x80000000);
    unsigned maxExt = static_cast<unsigned>(regs[0]);
    if (maxExt >= 0x80000004) {
        __cpuid(reinterpret_cast<int*>(regs), 0x80000002); std::memcpy(brand +  0, regs, 16);
        __cpuid(reinterpret_cast<int*>(regs), 0x80000003); std::memcpy(brand + 16, regs, 16);
        __cpuid(reinterpret_cast<int*>(regs), 0x80000004); std::memcpy(brand + 32, regs, 16);
        brand[48] = '\0';
        info.cpu_brand = brand;
    } else {
        info.cpu_brand = "Unknown CPU";
    }

    SYSTEM_INFO si{};
    GetSystemInfo(&si);
    info.logical_cores = static_cast<std::uint32_t>(si.dwNumberOfProcessors);
    return info;
}

struct WinCounters {
    // Time
    std::uint64_t qpc_start = 0;
    std::uint64_t qpc_end = 0;
    std::uint64_t qpc_freq = 0;

    // CPU cycles (thread)
    std::uint64_t cycles_start = 0;
    std::uint64_t cycles_end = 0;

    // Process memory stats snapshots (approximate; captured at end)
    SIZE_T working_set_bytes = 0;
    SIZE_T private_bytes = 0;

    // Page fault count (process-wide)
    DWORD page_fault_count = 0;
};

inline std::uint64_t qpc_now() {
    LARGE_INTEGER li{};
    QueryPerformanceCounter(&li);
    return static_cast<std::uint64_t>(li.QuadPart);
}

inline std::uint64_t qpc_freq() {
    LARGE_INTEGER li{};
    QueryPerformanceFrequency(&li);
    return static_cast<std::uint64_t>(li.QuadPart);
}

inline std::uint64_t thread_cycles() {
    ULONG64 cycles = 0;
    // Windows Vista+; per-thread cycle counter
    QueryThreadCycleTime(GetCurrentThread(), &cycles);
    return static_cast<std::uint64_t>(cycles);
}

inline void capture_process_memory(WinCounters& c) {
    PROCESS_MEMORY_COUNTERS_EX pmc{};
    pmc.cb = sizeof(pmc);
    if (GetProcessMemoryInfo(GetCurrentProcess(),
                             reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc),
                             sizeof(pmc))) {
        c.working_set_bytes = pmc.WorkingSetSize;
        c.private_bytes = pmc.PrivateUsage;
        c.page_fault_count = pmc.PageFaultCount;
    }
}

inline void counters_begin(WinCounters& c) {
    c.qpc_freq = qpc_freq();
    c.qpc_start = qpc_now();
    c.cycles_start = thread_cycles();
}

inline void counters_end(WinCounters& c) {
    c.cycles_end = thread_cycles();
    c.qpc_end = qpc_now();
    capture_process_memory(c);
}

inline double qpc_seconds(const WinCounters& c) {
    const auto ticks = (c.qpc_end - c.qpc_start);
    return c.qpc_freq ? static_cast<double>(ticks) / static_cast<double>(c.qpc_freq) : 0.0;
}

inline std::uint64_t thread_cycle_delta(const WinCounters& c) {
    return c.cycles_end - c.cycles_start;
}

} // namespace vs
#endif
