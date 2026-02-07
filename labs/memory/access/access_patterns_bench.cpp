#include <iostream>
#include <vector>
#include <cstdint>

#include "bench.hpp"
#include "utils.hpp"

namespace {
volatile std::uint64_t usink = 0;
}

int main(int argc, char** argv) {
    std::size_t N = 32'000'000; // bytes-ish scale via uint64_t elements
    std::size_t iters = 12;
    std::size_t stride = 16;    // in elements, not bytes
    std::uint64_t seed = 12345;

    if (argc > 1) N = std::stoull(argv[1]);
    if (argc > 2) iters = std::stoull(argv[2]);
    if (argc > 3) stride = std::stoull(argv[3]);
    if (argc > 4) seed = std::stoull(argv[4]);

    std::vector<std::uint64_t> data(N);
    for (std::size_t i = 0; i < N; ++i) data[i] = (std::uint64_t)i * 11400714819323198485ULL;

    const auto rnd = vs::make_shuffled_indices(N, seed);

    std::cout << "access_patterns_bench N=" << N << " iters=" << iters << " stride=" << stride << "\n";
    vs::print_csv_header(std::cout);

    // Sequential
    {
        auto r = vs::bench("sequential_read", 3, iters, [&] {
            std::uint64_t acc = 0;
            for (std::size_t i = 0; i < N; ++i) acc ^= data[i];
            usink ^= acc;
        });
        vs::print_csv_row(std::cout, r);
    }

    // Random
    {
        auto r = vs::bench("random_read", 3, iters, [&] {
            std::uint64_t acc = 0;
            for (std::size_t i = 0; i < N; ++i) acc ^= data[rnd[i]];
            usink ^= acc;
        });
        vs::print_csv_row(std::cout, r);
    }

    // Strided
    {
        auto r = vs::bench("strided_read", 3, iters, [&] {
            std::uint64_t acc = 0;
            for (std::size_t i = 0; i < N; i += stride) acc ^= data[i];
            usink ^= acc;
        });
        vs::print_csv_row(std::cout, r);
    }

    std::cerr << "usink=" << usink << "\n";
    return 0;
}
