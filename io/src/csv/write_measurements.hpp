/** TRACCC library, part of the ACTS project (R&D line)
 *
 * (c) 2024 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

#pragma once

// Project include(s).
#include "traccc/edm/measurement.hpp"

// System include(s).
#include <string_view>

namespace traccc::io::csv {

/// Write a measurement collection into a csv file.
///
/// @param filename is the name of the output file
/// @param measurements is the measurement collection to write
///
void write_measurements(
    std::string_view filename,
    traccc::measurement_collection_types::const_view measurements);

}  // namespace traccc::io::csv
