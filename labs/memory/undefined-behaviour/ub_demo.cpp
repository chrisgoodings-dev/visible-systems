#include <iostream>
#include <vector>
#include <cstdint>

static volatile std::uint64_t sink = 0;

int main() {
    std::cout << "UB demos: run with sanitizers to observe failures.\n";

    // Demo 1: out-of-bounds read (UB)
    {
        std::vector<std::uint64_t> v(8);
        for (std::size_t i = 0; i < v.size(); ++i) v[i] = i + 1;

        // UB: v[8] is OOB
        sink ^= v[8];
        std::cout << "OOB read produced sink=" << sink << " (meaningless / UB)\n";
    }

    // Demo 2: use-after-free (UB)
    {
        auto* p = new std::uint64_t(42);
        delete p;
        // UB: dereference freed pointer
        sink ^= *p;
        std::cout << "UAF read produced sink=" << sink << " (meaningless / UB)\n";
    }

    // Demo 3: strict aliasing/type punning UB (depends on how you do it)
    {
        float f = 1.0f;
        // UB in standard C++ (type punning through unrelated pointer)
        auto* u = reinterpret_cast<std::uint32_t*>(&f);
        sink ^= *u;
        std::cout << "Type punning produced sink=" << sink << " (meaningless / UB)\n";
    }

    std::cout << "Done. If this didn't crash, that doesn't mean it's safe.\n";
    return 0;
}
