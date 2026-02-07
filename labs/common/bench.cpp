#pragma once
#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include <cmath>
#include <fstream>
#include <ctime>
#ifdef _WIN32
#include "metrics_win.hpp"
#endif

#include "utils.hpp"

namespace vs {

struct BenchResult {
    std::string name;
    std::size_t iterations{};
    double mean_ns{};
    double stdev_ns{};
    double min_ns{};
    double max_ns{};
};

struct RunMetadata {
    std::string suite;        // e.g. "memory.layout"
    std::string benchmark;    // e.g. "AoS_iterate_positions_only"
    std::string build_type;   // e.g. "Release"
    std::string compiler;     // e.g. "MSVC 19.xx"
    std::string timestamp_utc;
    std::string git_commit;   // optional (env var)
#ifdef _WIN32
    WinSystemInfo sys;
    WinCounters counters;     // per-benchmark counters for one run (optional)
#endif
};

// Minimal JSON writing without dependencies (safe for metadata + results)
inline std::string json_escape(const std::string& s) {
    std::string out;
    out.reserve(s.size() + 8);
    for (char c : s) {
        switch (c) {
            case '\\': out += "\\\\"; break;
            case '"':  out += "\\\""; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default: out += c; break;
        }
    }
    return out;
}

inline std::string utc_timestamp() {
    std::time_t t = std::time(nullptr);
    std::tm tm{};
#if defined(_WIN32)
    gmtime_s(&tm, &t);
#else
    gmtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

inline std::string get_env(const char* k) {
    if (!k) return {};
#if defined(_WIN32)
    char* buf = nullptr;
    size_t len = 0;
    if (_dupenv_s(&buf, &len, k) == 0 && buf) {
        std::string v(buf);
        free(buf);
        return v;
    }
    return {};
#else
    const char* v = std::getenv(k);
    return v ? std::string(v) : std::string{};
#endif
}

inline void write_json_result(const std::string& path,
                              const RunMetadata& meta,
                              const BenchResult& r) {
    std::ofstream os(path, std::ios::binary);
    os << "{\n";
    os << "  \"suite\": \"" << json_escape(meta.suite) << "\",\n";
    os << "  \"benchmark\": \"" << json_escape(meta.benchmark) << "\",\n";
    os << "  \"build_type\": \"" << json_escape(meta.build_type) << "\",\n";
    os << "  \"compiler\": \"" << json_escape(meta.compiler) << "\",\n";
    os << "  \"timestamp_utc\": \"" << json_escape(meta.timestamp_utc) << "\",\n";
    os << "  \"git_commit\": \"" << json_escape(meta.git_commit) << "\",\n";
#ifdef _WIN32
    os << "  \"system\": {\n";
    os << "    \"cpu_brand\": \"" << json_escape(meta.sys.cpu_brand) << "\",\n";
    os << "    \"logical_cores\": " << meta.sys.logical_cores << "\n";
    os << "  },\n";
    os << "  \"counters\": {\n";
    os << "    \"qpc_seconds\": " << qpc_seconds(meta.counters) << ",\n";
    os << "    \"thread_cycles\": " << thread_cycle_delta(meta.counters) << ",\n";
    os << "    \"working_set_bytes\": " << static_cast<std::uint64_t>(meta.counters.working_set_bytes) << ",\n";
    os << "    \"private_bytes\": " << static_cast<std::uint64_t>(meta.counters.private_bytes) << ",\n";
    os << "    \"page_fault_count\": " << meta.counters.page_fault_count << "\n";
    os << "  },\n";
#endif
    os << "  \"result\": {\n";
    os << "    \"iters\": " << r.iterations << ",\n";
    os << "    \"mean_ns\": " << r.mean_ns << ",\n";
    os << "    \"stdev_ns\": " << r.stdev_ns << ",\n";
    os << "    \"min_ns\": " << r.min_ns << ",\n";
    os << "    \"max_ns\": " << r.max_ns << "\n";
    os << "  }\n";
    os << "}\n";
}

inline double ns_since(const std::chrono::steady_clock::time_point& a,
                       const std::chrono::steady_clock::time_point& b) {
    return (double)std::chrono::duration_cast<std::chrono::nanoseconds>(b - a).count();
}

// Run `fn` repeatedly, measure each iteration duration.
// Use small warmup to stabilize caches / JIT (none in C++) / CPU state.
inline BenchResult bench(std::string name,
                         std::size_t warmup_iters,
                         std::size_t iters,
                         const std::function<void()>& fn) {
    for (std::size_t i = 0; i < warmup_iters; ++i) fn();

    std::vector<double> samples;
    samples.reserve(iters);

    for (std::size_t i = 0; i < iters; ++i) {
        const auto t0 = std::chrono::steady_clock::now();
        fn();
        const auto t1 = std::chrono::steady_clock::now();
        samples.push_back(ns_since(t0, t1));
    }

    const auto [min_it, max_it] = std::minmax_element(samples.begin(), samples.end());
    const double mean = std::accumulate(samples.begin(), samples.end(), 0.0) / (double)samples.size();

    double var = 0.0;
    for (double x : samples) {
        const double d = x - mean;
        var += d * d;
    }
    var /= (samples.size() > 1 ? (double)(samples.size() - 1) : 1.0);
    const double stdev = std::sqrt(var);

    return BenchResult{
        .name = std::move(name),
        .iterations = iters,
        .mean_ns = mean,
        .stdev_ns = stdev,
        .min_ns = *min_it,
        .max_ns = *max_it
    };
}

#ifdef _WIN32
inline BenchResult bench_with_counters(std::string name,
                                       std::size_t warmup_iters,
                                       std::size_t iters,
                                       const std::function<void()>& fn,
                                       WinCounters& outCounters) {
    for (std::size_t i = 0; i < warmup_iters; ++i) fn();

    counters_begin(outCounters);
    auto r = bench(std::move(name), 0, iters, fn); // bench() already times; we keep counters around full loop
    counters_end(outCounters);
    return r;
}
#endif


inline void print_csv_header(std::ostream& os) {
    os << "name,iters,mean_ns,stdev_ns,min_ns,max_ns\n";
}

inline void print_csv_row(std::ostream& os, const BenchResult& r) {
    os << csv_escape(r.name) << ","
       << r.iterations << ","
       << r.mean_ns << ","
       << r.stdev_ns << ","
       << r.min_ns << ","
       << r.max_ns << "\n";
}

} // namespace vs
