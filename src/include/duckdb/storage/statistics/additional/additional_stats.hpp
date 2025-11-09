//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/storage/statistics/additional/additional_stats.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#define ADDITIONAL_NUMERIC_STATS EmptyAdditionalStats
#define ADDITIONAL_STRING_STATS  EmptyAdditionalStats

namespace duckdb {
template <class T>
class EmptyAdditionalStats;

template <class T>
class AdditionalStats {
public:
	AdditionalStats(std::vector<T> &data) {
		this->Initialise(data);
	}
	// init
	void Initialise(std::vector<T> &data);
	// query
	FilterPropagateResult Query();
	// size
	size_t Size();
	// serialise
	void Serialise();
	// deserialise
	void Deserialise();
};

} // namespace duckdb