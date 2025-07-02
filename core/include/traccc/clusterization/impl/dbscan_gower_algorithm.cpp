#include "traccc/clusterization/impl/dbscan_gower_algorithm.hpp"
#include <queue>
#include <cmath>

namespace traccc {

double DBSCANGowerAlgorithm::gower_distance(const std::vector<double>& a, const std::vector<double>& b) const {
    double sum = 0.0;
    double range = static_cast<double>(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        sum += std::abs(a[i] - b[i]); // Assume normalized features
    }
    return sum / range;
}

std::vector<int> DBSCANGowerAlgorithm::cluster(
    const std::vector<std::vector<double>>& features,
    const Params& params) const
{
    size_t n = features.size();
    std::vector<int> labels(n, -1); // -1: unclassified, 0: noise, >0: cluster id
    int cluster_id = 1;

    for (size_t i = 0; i < n; ++i) {
        if (labels[i] != -1) continue;

        // Find neighbors
        std::vector<size_t> neighbors;
        for (size_t j = 0; j < n; ++j) {
            if (gower_distance(features[i], features[j]) <= params.eps)
                neighbors.push_back(j);
        }
        if (neighbors.size() < params.minPts) {
            labels[i] = 0; // Mark as noise
            continue;
        }
        // Expand cluster
        labels[i] = cluster_id;
        std::queue<size_t> q;
        for (auto nidx : neighbors) q.push(nidx);

        while (!q.empty()) {
            size_t idx = q.front(); q.pop();
            if (labels[idx] == 0) labels[idx] = cluster_id;
            if (labels[idx] != -1) continue;
            labels[idx] = cluster_id;

            // Find neighbors
            std::vector<size_t> n_neighbors;
            for (size_t j = 0; j < n; ++j) {
                if (gower_distance(features[idx], features[j]) <= params.eps)
                    n_neighbors.push_back(j);
            }
            if (n_neighbors.size() >= params.minPts) {
                for (auto nidx : n_neighbors) q.push(nidx);
            }
        }
        ++cluster_id;
    }
    return labels;
}

} // namespace traccc
