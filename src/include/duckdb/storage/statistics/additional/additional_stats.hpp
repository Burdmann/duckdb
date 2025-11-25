//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/storage/statistics/additional/additional_stats.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include <functional>
#include <memory>

#define ADDITIONAL_NUMERIC_STATS EmptyAdditionalStats
#define ADDITIONAL_STRING_STATS  EmptyAdditionalStats<string_t>

namespace duckdb {
template <class T>
class EmptyAdditionalStats;
template <class T>
class ClusterAdditionalStats;
class StringClusterAdditionalStats;
template <class T>
class BloomAdditionalStats;

template <class T>
class AdditionalStats {
public:
	std::function<void(std::vector<T> &, AdditionalStats<T> *)> Initialise;
	std::function<FilterPropagateResult(AdditionalStats<T> *, ExpressionType, T)> Query;
	std::function<size_t(AdditionalStats<T> *)> Size;
	std::function<void(AdditionalStats<T> *)> Serialise;
	std::function<void(AdditionalStats<T> *)> Deserialise;
	static constexpr char *static_name = "error";
	const char *name;

	AdditionalStats() {
		this->name = static_name;
	}
	// init
	// void Initialise(std::vector<T> &data) {

	// }
	// query
	// FilterPropagateResult Query() {

	// }
	// size
	// size_t Size() {

	// }
	// serialise
	// void Serialise() {

	// }
	// deserialise
	// void Deserialise() {

	// }
};

} // namespace duckdb