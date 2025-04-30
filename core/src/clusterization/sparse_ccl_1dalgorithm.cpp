#include "traccc/clusterization/sparse_ccl_1dalgorithm.hpp"

#include "traccc/clusterization/details/sparse_ccl_1d.hpp"
#include "traccc/sanity/contiguous_on.hpp"
#include "traccc/sanity/ordered_on.hpp"
#include "traccc/utils/projections.hpp"
#include "traccc/utils/relations.hpp"

#include <vecmem/containers/device_vector.hpp>
#include <vecmem/containers/vector.hpp>

namespace traccc::host {

sparse_ccl_1d_algorithm::sparse_ccl_1d_algorithm(
    vecmem::memory_resource& mr,
    std::unique_ptr<const Logger> logger)
  : messaging(std::move(logger)),
    m_mr(mr) {}

edm::silicon_cluster_collection::host sparse_ccl_1d_algorithm::operator()(
    const edm::silicon_cell_collection::const_view& cells_view) const {

  const edm::silicon_cell_collection::const_device cells{cells_view};

  assert(is_contiguous_on(cell_module_projection(), cells));

  auto channel0_only_order = [](const auto& cell1, const auto& cell2) {
    if (cell1.module_index() != cell2.module_index()) {
      return cell1.module_index() < cell2.module_index();
    }
    return cell1.channel0() < cell2.channel0();
  };

  assert(is_ordered_on(channel0_only_order, cells));

  vecmem::vector<unsigned int> cluster_indices{cells.size(), &(m_mr.get())};
  vecmem::device_vector<unsigned int> cluster_indices_device{
      vecmem::get_data(cluster_indices)};
  const unsigned int num_clusters =
      details::sparse_ccl_1d(cells, cluster_indices_device);

  edm::silicon_cluster_collection::host clusters{m_mr.get()};
  clusters.resize(num_clusters);

  for (unsigned int idx = 0; idx < cluster_indices.size(); ++idx) {
    clusters.cell_indices()[cluster_indices[idx]].push_back(idx);
  }

  return clusters;
}

}  // namespace traccc::host

