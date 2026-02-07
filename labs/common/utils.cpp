#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <random>
#include <algorithm>

namespace vs {

inline std::vector<std::size_t> make_shuffled_indices(std::size_t n, std::uint64_t seed) {
    std::vector<std::size_t> idx(n);
    for (std::size_t i = 0; i < n; ++i) idx[i] = i;
    std::mt19937_64 rng(seed);
    std::shuffle(idx.begin(), idx.end(), rng);
    return idx;
}

inline std::string csv_escape(std::string s) {
    // minimal CSV escaping: wrap in quotes if contains comma/quote/newline
    bool need = false;
    for (char c : s) if (c == ',' || c == '"' || c == '\n' || c == '\r') { need = true; break; }
    if (!need) return s;
    std::string out;
    out.reserve(s.size() + 2);
    out.push_back('"');
    for (char c : s) {
        if (c == '"') out.push_back('"');
        out.push_back(c);
    }
    out.push_back('"');
    return out;
}

} // namespace vs
