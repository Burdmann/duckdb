//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/storage/statistics/additional/empty_additional_stats.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include <functional>
#include "duckdb/storage/statistics/additional/additional_stats.hpp"
#include "duckdb/common/enums/filter_propagate_result.hpp"

namespace duckdb {

template <class T>
class EmptyAdditionalStats : public AdditionalStats<T> {
public:
	static inline const char *GetStaticName() {
		return "empty";
	}
	inline EmptyAdditionalStats(std::vector<T> &data) {
		this->name = GetStaticName();
		this->Initialise = &Initialise_implementation;
		this->Query = &Query_implementation;
		this->QueryRange = &QueryRange_implementation;
		this->Size = &Size_implementation;
		this->Serialise = &Serialise_implementation;
		this->Deserialise = &Deserialise_implementation;
		this->Initialise(data, this);
	}
	inline static void Initialise_implementation(std::vector<T> &data, AdditionalStats<T> *stats) {
	}
	inline static FilterPropagateResult Query_implementation(AdditionalStats<T> *stats, ExpressionType comparison_type,
	                                                         T constant) {
		return FilterPropagateResult::NO_PRUNING_POSSIBLE;
	}
	inline static FilterPropagateResult QueryRange_implementation(AdditionalStats<T> *stats, T start, T end) {
		return FilterPropagateResult::NO_PRUNING_POSSIBLE;
	}
	inline static size_t Size_implementation(AdditionalStats<T> *stats) {
		EmptyAdditionalStats<T> *nstats = (EmptyAdditionalStats<T> *)stats;
		return sizeof(*nstats);
	}
	inline static void Serialise_implementation(AdditionalStats<T> *stats) {
	}
	inline static void Deserialise_implementation(AdditionalStats<T> *stats) {
	}
};

} // namespace duckdb