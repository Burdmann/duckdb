//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/storage/statistics/additional/additional_stats.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#define NUMERIC_STATS EmptyAdditionalStats
#define STRING_STATS  EmptyAdditionalStats

namespace duckdb {
class EmptyAdditionalStats;

template <class T>
class AdditionalStats {
public:
	AdditionalStats(std::vector<T> &data) {
		Initialise(*this, data);
	}
	// init
	virtual static void Initialise(AdditionalStats &stat, std::vector<T> &data);
	// query
	virtual FilterPropagateResult Query();
	// size
	virtual size_t Size();
	// serialise
	virtual void Serialise();
	// deserialise
	virtual void Deserialise();
};

} // namespace duckdb