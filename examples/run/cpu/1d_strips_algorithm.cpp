/** Example for processing 1D strip data using TRACCC library
 *
 * This example demonstrates how to read cell data from a CSV file,
 * filter only 1D strip cells, and perform clustering and measurement creation.
 *
 * (c) 2021-2025 CERN for the benefit of the ACTS project
 *
 * Mozilla Public License Version 2.0
 */

// Project include(s).
#include "traccc/clusterization/measurement_creation_algorithm.hpp"
#include "traccc/clusterization/sparse_ccl_algorithm.hpp"
#include "traccc/edm/measurement.hpp"
#include "traccc/edm/silicon_cell_collection.hpp"
#include "traccc/edm/silicon_cluster_collection.hpp"
#include "traccc/geometry/silicon_detector_description.hpp"
#include "io/include/traccc/io/read_cells.cpp"

// VecMem include(s).
#include <vecmem/memory/host_memory_resource.hpp>

// Standard library includes.
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // Check for correct number of arguments
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <path_to_csv_file>" << std::endl;
        return 1;
    }

    // Read the CSV file path from the command line arguments
    std::string csv_file = argv[1];

    // Memory resource
    vecmem::host_memory_resource resource;

    // Algorithms for clustering and measurement creation
    traccc::host::sparse_ccl_algorithm cc(resource);
    traccc::host::measurement_creation_algorithm mc(resource);

    // Cell collection to hold data from the CSV
    traccc::edm::silicon_cell_collection::host all_cells{resource};
    traccc::edm::silicon_cell_collection::host filtered_cells{resource};

    // Detector description
    traccc::silicon_detector_description::host dd{resource};
    dd.resize(1);

    try {
        // Read cells from the CSV file
        traccc::io::csv::read_cells(all_cells, csv_file, traccc::getDummyLogger().clone());
    } catch (const std::exception& e) {
        std::cerr << "Error reading CSV file: " << e.what() << std::endl;
        return 1;
    }

    // Filter only 1D strip cells
    for (const auto& cell : all_cells) {
        if (dd.dimensions().at(cell.module_index()) == 1) { // Check if the module is 1D
            filtered_cells.push_back(cell);
        }
    }

    if (filtered_cells.empty()) {
        std::cerr << "No 1D strip cells found in the CSV file." << std::endl;
        return 1;
    }

    // Perform clustering
    auto filtered_cells_data = vecmem::get_data(filtered_cells);
    auto clusters = cc(filtered_cells_data);

    if (clusters.empty()) {
        std::cerr << "Clustering failed to produce results." << std::endl;
        return 1;
    }

    // Perform measurement creation
    auto clusters_data = vecmem::get_data(clusters);
    auto dd_data = vecmem::get_data(dd);
    auto measurements = mc(filtered_cells_data, clusters_data, dd_data);

    if (measurements.empty()) {
        std::cerr << "Measurement creation failed to produce results." << std::endl;
        return 1;
    }

    // Output the results
    std::cout << "Number of clusters: " << clusters.size() << std::endl;
    std::cout << "Number of measurements: " << measurements.size() << std::endl;

    return 0;
}
