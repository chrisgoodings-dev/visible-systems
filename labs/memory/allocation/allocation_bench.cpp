#include <iostream>
#include <vector>
#include <memory>
#include <cstdint>

#include "bench.hpp"

namespace {

struct Node {
    std::uint64_t a, b, c, d;
};

struct Pool {
    std::vector<Node> buf;
    std::size_t head = 0;

    explicit Pool(std::size_t capacity) : buf(capacity) {}

    Node* alloc() {
        if (head >= buf.size()) return nullptr;
        return &buf[head++];
    }

    void reset() { head = 0; }
};

volatile std::uint64_t sink = 0;

void touch(Node& n, std::uint64_t i) {
    n.a = i;
    n.b = i * 3;
    n.c = i ^ 0x9e3779b97f4a7c15ULL;
    n.d = n.a + n.b + n.c;
    sink ^= n.d;
}

} // namespace

int main(int argc, char** argv) {
    std::size_t N = 200000;
    std::size_t iters = 30;
    if (argc > 1) N = std::stoull(argv[1]);
    if (argc > 2) iters = std::stoull(argv[2]);

    std::cout << "allocation_bench N=" << N << " iters=" << iters << "\n";
    vs::print_csv_header(std::cout);

    // Stack: create a local Node repeatedly
    {
        auto r = vs::bench("stack_local_object", 5, iters, [&] {
            for (std::size_t i = 0; i < N; ++i) {
                Node n{};
                touch(n, (std::uint64_t)i);
            }
        });
        vs::print_csv_row(std::cout, r);
    }

    // Heap: allocate and delete each time
    {
        auto r = vs::bench("heap_new_delete", 3, iters, [&] {
            for (std::size_t i = 0; i < N; ++i) {
                Node* p = new Node{};
                touch(*p, (std::uint64_t)i);
                delete p;
            }
        });
        vs::print_csv_row(std::cout, r);
    }

    // Heap batched: allocate many then delete many (reduced allocator churn in loop body)
    {
        auto r = vs::bench("heap_batched_new_delete", 3, iters, [&] {
            std::vector<Node*> ptrs;
            ptrs.reserve(N);
            for (std::size_t i = 0; i < N; ++i) {
                ptrs.push_back(new Node{});
            }
            for (std::size_t i = 0; i < N; ++i) {
                touch(*ptrs[i], (std::uint64_t)i);
            }
            for (auto* p : ptrs) delete p;
        });
        vs::print_csv_row(std::cout, r);
    }

    // Pool: fixed backing storage
    {
        Pool pool(N);
        auto r = vs::bench("pool_fixed_buffer", 5, iters, [&] {
            pool.reset();
            for (std::size_t i = 0; i < N; ++i) {
                Node* p = pool.alloc();
                if (!p) break;
                touch(*p, (std::uint64_t)i);
            }
        });
        vs::print_csv_row(std::cout, r);
    }

    // Prevent optimizing everything away
    std::cerr << "sink=" << sink << "\n";
    return 0;
}
