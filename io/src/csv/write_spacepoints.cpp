/** TRACCC library, part of the ACTS project (R&D line)
 *
 * (c) 2024 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

// Local include(s).
#include "write_spacepoints.hpp"

// System include(s).
#include <fstream>
#include <stdexcept>

namespace traccc::io::csv {

void write_spacepoints(
    std::string_view filename,
    edm::spacepoint_collection::const_view spacepoints_view) {

    // Open the file for writing.
    std::ofstream ofile(filename.data());
    if (!ofile.is_open()) {
        throw std::runtime_error("Could not open file " +
                                 std::string(filename));
    }

    // Create a device collection around the spacepoint view.
    const edm::spacepoint_collection::const_device spacepoints(spacepoints_view);

    // Write CSV header
    file << "spx,spy,spz\n";

  
 // Write the spacepoints as CSV
    for (edm::spacepoint_collection::const_device::size_type i = 0u;
         i < spacepoints.size(); ++i) {
        const auto sp = spacepoints.at(i);
        file << sp.x() << "," << sp.y() << "," << sp.z() << "\n";
    }
}

}  // namespace traccc::io::csv
