/** TRACCC library, part of the ACTS project (R&D line)
 *
 * (c) 2024 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

// Local include(s).
#include "write_measurements.hpp"

// System include(s).
#include <fstream>

namespace traccc::io::csv {

void write_measurements(
    std::string_view filename,
    traccc::measurement_collection_types::const_view measurements_view) {

    // Open the output file.
    std::ofstream file(filename.data());
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " +
                                 std::string(filename));
    }

    // Create a device collection around the measurement view.
    traccc::measurement_collection_types::const_device measurements(
        measurements_view);

    // Write the measurements in .csv format.
    file << "surface_link,measurement_id,local0,local1,var_local0,var_local1\n";

    for (const traccc::measurement& mm : measurements) {

        // Print debug info
        // std::cout << "Surface Link: " << mm.surface_link
        //         << ", Measurement ID: " << mm.measurement_id
        //         << ", Local Coordinates: (" << mm.local[0] << ", " << mm.local[1] << ")"
        //         << ", Variance: (" << mm.variance[0] << ", " << mm.variance[1] << ")"
        //         << ", Cluster Link: " << mm.cluster_link
        //         << ", Measurement Dimension: " << mm.meas_dim << "\n";
 
        file << mm.surface_link << ',' << mm.measurement_id << ',' 
             << mm.local[0] << ',' << mm.local[1] << ',' 
             << mm.variance[0] << ',' << mm.variance[1] << '\n';
    }
}

}  // namespace traccc::io::csv
