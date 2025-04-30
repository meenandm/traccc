/** TRACCC library, part of the ACTS project (R&D line)
 *
 * (c) 2021-2024 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

#pragma once

// Library include(s).
// Note: Using simplified includes for this test implementation
#include <cassert>

// VecMem include(s).
// Note: Using simplified versions in test_sparse_ccl_1d.cpp

namespace traccc::details {

/// Implementation of 1D Sparse CCL, following
/// [DOI: 10.1109/DASIP48288.2019.9049184]
///
/// Requires cells to be sorted in column major for optimal performance

/// Find root of the tree for entry @param e
///
/// @param labels an equivalence table
///
/// @return the root of @param e
///
TRACCC_HOST_DEVICE inline unsigned int find_root(
    const vecmem::device_vector<unsigned int>& labels, unsigned int e);

/// Create a union of two entries @param e1 and @param e2
///
/// @param labels an equivalence table
///
/// @return the least common ancestor of the entries
///
TRACCC_HOST_DEVICE inline unsigned int make_union(
    vecmem::device_vector<unsigned int>& labels, unsigned int e1,
    unsigned int e2);

/// Helper method to find adjacent cells in 1D
///
/// @param a the first cell
/// @param b the second cell
///
/// @return boolean to indicate 1D connectivity
///
template <typename T1, typename T2>
TRACCC_HOST_DEVICE inline bool is_adjacent_1d(const edm::silicon_cell<T1>& a,
                                            const edm::silicon_cell<T2>& b);

/// Helper method to define distance in 1D,
/// does not need abs, as channels are assumed sorted in
/// column major
///
/// @param a the first cell
/// @param b the second cell
///
/// @return boolean to indicate cells are far enough apart (optimization)
///
template <typename T1, typename T2>
TRACCC_HOST_DEVICE inline bool is_far_enough_1d(const edm::silicon_cell<T1>& a,
                                              const edm::silicon_cell<T2>& b);

/// Sparse 1D CCL algorithm
///
/// @param cells is the cell collection
/// @param labels is the vector of the output indices (to which cluster a cell
///               belongs to)
/// @return number of clusters
///
TRACCC_HOST_DEVICE inline unsigned int sparse_ccl_1d(
    const edm::silicon_cell_collection::const_device& cells,
    vecmem::device_vector<unsigned int>& labels);

}  // namespace traccc::details

// Include the implementation.
#include "traccc/clusterization/impl/sparse_ccl_1d.ipp"
