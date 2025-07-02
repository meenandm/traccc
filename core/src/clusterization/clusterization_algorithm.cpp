/** TRACCC library, part of the ACTS project (R&D line)
 *
 * (c) 2022-2024 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

// Library include(s).
#include "traccc/clusterization/clusterization_algorithm.hpp"

namespace traccc::host {

clusterization_algorithm::clusterization_algorithm(
    vecmem::memory_resource& mr, std::unique_ptr<const Logger> logger)
    : messaging(logger->clone()),
      m_cc(mr, logger->cloneWithSuffix("CclAlg")),
      m_mc(mr, logger->cloneWithSuffix("MeasurementCreationAlg")),
      m_mr(mr) {}

clusterization_algorithm::output_type clusterization_algorithm::operator()(
    const edm::silicon_cell_collection::const_view& cells_view,
    const silicon_detector_description::const_view& dd_view) const {

    if (m_useGowerDbscan) {
        // 1. Extract features from cells_view (implement this utility)
        std::vector<std::vector<double>> features = extract_features_for_gower(cells_view);

        // 2. Set DBSCAN params
        DBSCANGowerAlgorithm::Params params{.eps=0.3, .minPts=5};

        // 3. Run clustering
        auto labels = m_dbscanGower.cluster(features, params);

        // 4. Convert labels to your expected clusters_data format
        auto clusters_data = convert_labels_to_clusters(labels, cells_view);

        // 5. Continue as before
        return m_mc(cells_view, clusters_data, dd_view);
    } else {
        // Default (existing) flow
        const sparse_ccl_algorithm::output_type clusters = m_cc(cells_view);
        const auto clusters_data = vecmem::get_data(clusters);
        return m_mc(cells_view, clusters_data, dd_view);
    }
}
}

}  // namespace traccc::host
