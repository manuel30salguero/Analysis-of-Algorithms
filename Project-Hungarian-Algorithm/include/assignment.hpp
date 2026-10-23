#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace lap {
using Cost = std::int64_t;
constexpr Cost max_abs_cost = 1000000000LL;
constexpr std::size_t max_dimension = 10000;

struct Matrix {
    std::size_t rows{}, cols{};
    std::vector<Cost> values;
    Matrix(std::size_t r = 0, std::size_t c = 0);
    Cost& operator()(std::size_t i, std::size_t j);
    Cost operator()(std::size_t i, std::size_t j) const;
};
struct Counters {
    std::uint64_t scans{}, expansions{}, positive_updates{};
};
struct Result {
    // -1 denotes an unmatched original row when rows > columns.
    std::vector<int> column_of_row;
    Cost cost{};
    // Potentials refer to the oriented problem with fewer rows than columns.
    std::vector<Cost> u, v;
    bool transposed{};
    Counters operations;
    std::size_t workspace_bytes{}; // Peak owned vector element capacity, input excluded.
};

Result hungarian(const Matrix& input, bool collect_counters = false);
Result greedy(const Matrix& input);
Result exhaustive(const Matrix& input); // Only dimensions <= 10.
bool valid_assignment(const Matrix& input, const Result& result);
bool valid_certificate(const Matrix& input, const Result& result);
Matrix generate(std::size_t rows, std::size_t cols,
                const std::string& family, std::uint64_t seed);
std::uint64_t fingerprint(const Matrix& matrix);
}
