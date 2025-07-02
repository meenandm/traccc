#pragma once

#include <vector>
#include <cstddef>
#include "traccc/clusterization/cluster_types.hpp"

namespace traccc {

class DBSCANGowerAlgorithm {
public:
    struct Params {
        double eps;
        size_t minPts;
    };

    // Accepts a collection of features (one per cell), returns cluster labels
    std::vector<int> cluster(
        const std::vector<std::vector<double>>& features,
        const Params& params) const;

private:
    double gower_distance(const std::vector<double>& a, const std::vector<double>& b) const;
};

} // namespace traccc
