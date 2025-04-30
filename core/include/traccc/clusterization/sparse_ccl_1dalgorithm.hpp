/** TRACCC library, part of the ACTS project (R&D line)
 *
 * (c) 2022-2024 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

#pragma once

// Library include(s).
#include "traccc/edm/silicon_cell_collection.hpp"
#include "traccc/edm/silicon_cluster_collection.hpp"
#include "traccc/utils/algorithm.hpp"
#include "traccc/utils/messaging.hpp"

// VecMem include(s).
#include <vecmem/memory/memory_resource.hpp>

namespace traccc::host {

/// Host-side implementation of the 1D sparse connected-component labeling algorithm
///
/// This is a 1D version of the original SparseCCL algorithm. It performs clustering
/// based only on the channel0 dimension, ignoring channel1.
///
/// @note The implementation assumes that cells are sorted by module and then by channel0
///
class sparse_ccl_1d_algorithm:

public algorithm<edm::silicon_cluster_collection::host(
          const edm::silicon_cell_collection::const_view&)>,
      public messaging {
    
    /// Constructor with a memory resource and a logger
    ///
    /// @param mr The memory resource to use for all internal allocations
    /// @param logger The logger to use for all logging
    ///
    sparse_ccl_1d_algorithm(vecmem::memory_resource& mr,
                         std::unique_ptr<const Logger> logger = getDummyLogger().clone());
    /// Perform 1D clustering on a collection of cells
    ///
    /// @param cells_view The collection of cells to cluster
    /// @return A container of cell indices, grouped by cluster 
    ///
    output_type operator()(
        const edm::silicon_cell_collection::const_view& cells_view) const override;

private:
   	 /// Memory resource used by the algorithm
    std::reference_wrapper<vecmem::memory_resource> m_mr;
};

}  // namespace traccc::host
