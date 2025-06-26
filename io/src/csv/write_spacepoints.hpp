/** TRACCC library, part of the ACTS project (R&D line)
 *
 * (c) 2024 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

#pragma once

// Project include(s).
#include "traccc/edm/spacepoint_collection.hpp"

// System include(s).
#include <string_view>

namespace traccc::io::csv {

/// Write a  spacepoints to a CSV file.
/// @param filename is the name of the output file
/// @param spacepoints_view View over the spacepoints to write.
///
void write_spacepoints(
    std::string_view filename,
    edm::spacepoint_collection::const_view spacepoints_view);

}  // namespace traccc::io::csv
