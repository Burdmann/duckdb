//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/storage/statistics/additional/empty_additional_stats.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/storage/statistics/additional/additional_stats.hpp"
#include "duckdb/common/enums/filter_propagate_result.hpp"

namespace duckdb {

template <class T>
class EmptyAdditionalStats : public AdditionalStats<T> {
public:
	using AdditionalStats<T>::AdditionalStats;
	static void Initialise(AdditionalStats<T> &stats, std::vector<T> &data) {
		printf("Init Empty stats\n");
	}
	FilterPropagateResult Query() {
		return FilterPropagateResult::NO_PRUNING_POSSIBLE;
	}
	size_t Size() {
		return 0;
	}
	void Serialise() {
	}
	void Deserialise() {
	}
};

} // namespace duckdb