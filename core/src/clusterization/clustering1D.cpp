#include <vector>
#include <iostream>
#include <algorithm>

// A simple structure to store the 1D cell hit information.
struct Cell {
    float position; // Position of the hit in 1D space
    int cluster_id; // Cluster ID after clustering
};

// Function to perform 1D clustering
void cluster_1d(std::vector<Cell>& cells, float distance_threshold) {
    if (cells.empty()) return;

    // Sort cells by position
    std::sort(cells.begin(), cells.end(), [](const Cell& a, const Cell& b) {
        return a.position < b.position;
    });

    int current_cluster_id = 0;
    cells[0].cluster_id = current_cluster_id; // Assign first cell to the first cluster

    // Iterate through the cells and group them into clusters based on the distance threshold
    for (size_t i = 1; i < cells.size(); ++i) {
        if ((cells[i].position - cells[i - 1].position) <= distance_threshold) {
            // If the current cell is close to the previous cell, assign it to the same cluster
            cells[i].cluster_id = current_cluster_id;
        } else {
            // Otherwise, start a new cluster
            current_cluster_id++;
            cells[i].cluster_id = current_cluster_id;
        }
    }
}

// Example usage
int main() {
    // Example 1D hit data
    std::vector<Cell> cells = {
        {1.0, -1}, {1.1, -1}, {2.5, -1}, {5.0, -1}, {5.1, -1}, {10.0, -1}
    };

    // Set the distance threshold for clustering
    float distance_threshold = 0.2; // Adjust this threshold based on your data characteristics

    // Run the clustering algorithm
    cluster_1d(cells, distance_threshold);

    // Output the results
    std::cout << "Clustered cells:\n";
    for (const auto& cell : cells) {
        std::cout << "Position: " << cell.position << " | Cluster ID: " << cell.cluster_id << '\n';
    }

    return 0;
}