/** TRACCC library, part of the ACTS project (R&D line)
 *
 * (c) 2021-2024 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

#pragma once

#include <cassert>

namespace traccc::details {

/// Function used for retrieving the cell signal based on the module id
///
/// This function returns the signal itself, but can be extended to
/// apply any additional transformations based on the detector geometry.
TRACCC_HOST_DEVICE
inline scalar signal_cell_modelling(
    scalar signal_in, const silicon_detector_description::const_device&) {
    return signal_in;
}

/// Get the local position of a cell on a module
///
/// @param cell      The cell to get the position of
/// @param det_descr The (silicon) detector description
/// @return The local position of the cell
template <typename T>
TRACCC_HOST_DEVICE inline vector2 position_from_cell(
    const edm::silicon_cell<T>& cell,
    const silicon_detector_description::const_device& det_descr) {

    // The detector description for the module that the cell is on.
    const auto module_dd = det_descr.at(cell.module_index());
    // Calculate / construct the local cell position.
    return {module_dd.reference_x() +
                (scalar{0.5f} + static_cast<scalar>(cell.channel0())) *
                    module_dd.pitch_x(),
            module_dd.reference_y() +
                (scalar{0.5f} + static_cast<scalar>(cell.channel1())) *
                    module_dd.pitch_y()};
}

/// Function to calculate cluster properties such as mean position,
/// variance, and total weight of the cluster using a weighted variant
/// of Welford's algorithm for numerical stability.
///
/// @tparam T            Template type of the cluster
/// @param[in] cluster   The silicon cluster whose properties are to be calculated
/// @param[in] cells     Collection of silicon cells in the event
/// @param[in] det_descr The detector description providing necessary parameters
/// @param[out] mean     The mean position of the cluster, updated by the function
/// @param[out] var      The variance of the cluster position, updated by the function
/// @param[out] totalWeight The total weight of the cluster, updated by the function
///
/// This function iterates over the cell indices in the cluster and calculates
/// the mean position, variance, and total weight using the cell readout values
/// translated into weights. It considers only the cells with weights above a 
/// specified threshold. The mean and variance calculations are offset by the 
/// first cell's position for better numerical stability.

/// @brief Calculate the mean position, variance, and total weight of a cluster.
///
/// @details This function iterates over the cell indices in the cluster and
/// calculates the mean position, variance, and total weight using the cell
/// readout values translated into weights. It considers only the cells with
/// weights above a specified threshold. The mean and variance calculations
/// are offset by the first cell's position for better numerical stability.
///
/// @param cluster The silicon cluster whose properties are to be calculated
/// @param cells Collection of silicon cells in the event
/// @param det_descr The detector description providing necessary parameters
/// @param[out] mean The mean position of the cluster, updated by the function
/// @param[out] var The variance of the cluster position, updated by the function
/// @param[out] totalWeight The total weight of the cluster, updated by the function
template <typename T>
TRACCC_HOST_DEVICE inline void calc_cluster_properties(
    const edm::silicon_cluster<T>& cluster,
    const edm::silicon_cell_collection::const_device& cells,
    const silicon_detector_description::const_device& det_descr, point2& mean,
    point2& var, scalar& totalWeight) {

    point2 offset{0.f, 0.f};
    bool first_processed = false;

    // Loop over the cell indices of the cluster.
    for (const unsigned int cell_idx : cluster.cell_indices()) {

        // The cell object.
        const auto cell = cells.at(cell_idx);

        // Translate the cell readout value into a weight.
        const scalar weight =
            signal_cell_modelling(cell.activation(), det_descr);

        // Only consider cells over a minimum threshold.
        if (weight > det_descr.threshold().at(cell.module_index())) {

            // Update all output properties with this cell.
            totalWeight += weight;
            scalar weight_factor = weight / totalWeight;

            point2 cell_position = position_from_cell(cell, det_descr);

            if (!first_processed) {
                offset = cell_position;
                first_processed = true;
            }

            cell_position = cell_position - offset;

            const point2 diff_old = cell_position - mean;
            mean = mean + diff_old * weight_factor;
            const point2 diff_new = cell_position - mean;

            var[0] = (1.f - weight_factor) * var[0] +
                     weight_factor * (diff_old[0] * diff_new[0]);
            var[1] = (1.f - weight_factor) * var[1] +
                     weight_factor * (diff_old[1] * diff_new[1]);
        }
    }

    mean = mean + offset;
}

    /// Function used for filling the measurement objects in the measurement
    /// collection.
    ///
    /// @param[out] measurements Measurement collection where the measurement is
    ///                          to be filled
    /// @param[in] index     Index of the measurement object to fill
    /// @param[in] cluster   The silicon cluster to turn into a measurement
    /// @param[in] cells     All silicon cells in the event
    /// @param[in] det_descr Detector description
    ///
    /// This function iterates over the cell indices in the cluster and calculates
    /// the mean position, variance, and total weight using a weighted variant of
    /// Welford's algorithm. It considers only the cells with weights above a
    /// specified threshold. The mean and variance calculations are offset by the
    /// first cell's position for better numerical stability.
    ///
    /// To learn more about this algorithm please refer to:
    /// [1] https://doi.org/10.1080/00401706.1962.10490022
    /// [2] The Art of Computer Programming, Donald E. Knuth, second
    ///     edition, chapter 4.2.2.
    ///
    /// The function then fills the measurement object with the calculated
    /// properties.
template <typename T>
TRACCC_HOST_DEVICE inline void fill_measurement(
    measurement_collection_types::device& measurements,
    measurement_collection_types::device::size_type index,
    const edm::silicon_cluster<T>& cluster,
    const edm::silicon_cell_collection::const_device& cells,
    const silicon_detector_description::const_device& det_descr) {

    // To calculate the mean and variance with high numerical stability
    // we use a weighted variant of Welford's algorithm. This is a
    // single-pass online algorithm that works well for large numbers
    // of samples, as well as samples with very high values.
    //
    // To learn more about this algorithm please refer to:
    // [1] https://doi.org/10.1080/00401706.1962.10490022
    // [2] The Art of Computer Programming, Donald E. Knuth, second
    //     edition, chapter 4.2.2.

    // Security checks.
    assert(cluster.cell_indices().empty() == false);
    assert([&]() {
        const unsigned int module_idx =
            cells.module_index().at(cluster.cell_indices().front());
        for (const unsigned int cell_idx : cluster.cell_indices()) {
            if (cells.module_index().at(cell_idx) != module_idx) {
                return false;
            }
        }
        return true;
    }() == true);

    // Calculate the cluster properties
    scalar totalWeight = 0.f;
    point2 mean{0.f, 0.f}, var{0.f, 0.f};
    calc_cluster_properties(cluster, cells, det_descr, mean, var, totalWeight);

    assert(totalWeight > 0.f);

    // Access the measurement in question.
    measurement& m = measurements[index];

    // The index of the module the cluster is on.
    const unsigned int module_idx =
        cells.module_index().at(cluster.cell_indices().front());
    // The detector description for the module that the cluster is on.
    const auto module_dd = det_descr.at(module_idx);

    // Fill the measurement object.
    m.surface_link = module_dd.geometry_id();
    // normalize the cell position
    m.local = mean;

    // plus pitch^2 / 12
    const scalar pitch_x = module_dd.pitch_x();
    const scalar pitch_y = module_dd.pitch_y();
    m.variance = var + point2{pitch_x * pitch_x / static_cast<scalar>(12.),
                              pitch_y * pitch_y / static_cast<scalar>(12.)};

    // For the ambiguity resolution algorithm, give a unique measurement ID
    m.measurement_id = index;

    // Set the measurement dimensionality.
    m.meas_dim = module_dd.dimensions();
}

}  // namespace traccc::details
