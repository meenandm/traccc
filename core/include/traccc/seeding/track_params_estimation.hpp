/** TRACCC library, part of the ACTS project (R&D line)
 *
 * (c) 2021-2025 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

#pragma once

// Library include(s).
#include "traccc/edm/measurement.hpp"
#include "traccc/edm/seed_collection.hpp"
#include "traccc/edm/spacepoint_collection.hpp"
#include "traccc/edm/track_parameters.hpp"
#include "traccc/utils/algorithm.hpp"
#include "traccc/utils/messaging.hpp"

// VecMem include(s).
#include <vecmem/memory/memory_resource.hpp>

// System include(s).
#include <functional>

namespace traccc::host {

/// Track parameter estimation algorithm
///
/// Transcribed from Acts/Seeding/EstimateTrackParamsFromSeed.hpp.
///
class track_params_estimation
    : public algorithm<bound_track_parameters_collection_types::host(
          const measurement_collection_types::const_view&,
          const edm::spacepoint_collection::const_view&,
          const edm::seed_collection::const_view&, const vector3&,
          const std::array<traccc::scalar, traccc::e_bound_size>&)>,
      public messaging {

    public:
    /// Constructor for track_params_estimation
    ///
    /// @param mr is the memory resource
    track_params_estimation(
        vecmem::memory_resource& mr,
        std::unique_ptr<const Logger> logger = getDummyLogger().clone());

    /// Callable operator for track_params_esitmation
    ///
    /// @param measurements All measurements of the event
    /// @param spacepoints All spacepoints of the event
    /// @param seeds The reconstructed track seeds of the event
    /// @param bfield (Temporary) Magnetic field vector
    /// @param stddev standard deviation for setting the covariance (Default
    /// value from arXiv:2112.09470v1)
    /// @return A vector of bound track parameters
    ///
    output_type operator()(
        const measurement_collection_types::const_view& measurements,
        const edm::spacepoint_collection::const_view& spacepoints,
        const edm::seed_collection::const_view& seeds, const vector3& bfield,
        const std::array<traccc::scalar, traccc::e_bound_size>& stddev = {
            0.02f * traccc::unit<traccc::scalar>::mm,
            0.03f * traccc::unit<traccc::scalar>::mm,
            1.f * traccc::unit<traccc::scalar>::degree,
            1.f * traccc::unit<traccc::scalar>::degree,
            0.01f / traccc::unit<traccc::scalar>::GeV,
            1.f * traccc::unit<traccc::scalar>::ns}) const override;

    private:
    /// The memory resource to use in the algorithm
    std::reference_wrapper<vecmem::memory_resource> m_mr;
};  // class track_params_estimation

}  // namespace traccc::host
