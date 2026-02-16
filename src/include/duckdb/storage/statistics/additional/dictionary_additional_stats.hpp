//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/storage/statistics/additional/dictionary_additional_stats.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include <functional>
#include <unordered_set>
#include "duckdb/storage/statistics/additional/additional_stats.hpp"
#include "duckdb/common/enums/filter_propagate_result.hpp"

namespace duckdb {

// to allow use of dictionaries only where there are few enough items in a partition
constexpr static uint32_t MAX_NUMBER_OF_ITEMS = 1000;

template <class T>
class DictionaryAdditionalStats : public AdditionalStats<T> {
private:
	bool overfull = false;
	std::unordered_set<T> dictionary;

public:
	static inline const char *GetStaticName() {
		return "dictionary";
	}
	inline DictionaryAdditionalStats(std::vector<T> &data) {
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
		DictionaryAdditionalStats<T> *nstats = (DictionaryAdditionalStats<T> *)stats;
		for (T item : data) {
			nstats->dictionary.insert(item);
		}
		if (nstats->dictionary.size() > MAX_NUMBER_OF_ITEMS) {
			nstats->overfull = true;
			nstats->dictionary.clear();
		}
	}
	inline static FilterPropagateResult Query_implementation(AdditionalStats<T> *stats, ExpressionType comparison_type,
	                                                         T constant) {
		DictionaryAdditionalStats<T> *nstats = (DictionaryAdditionalStats<T> *)stats;
		if (nstats->overfull || nstats->dictionary.find(constant) != nstats->dictionary.end())
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		return FilterPropagateResult::FILTER_ALWAYS_FALSE;
	}
	inline static FilterPropagateResult QueryRange_implementation(AdditionalStats<T> *stats, T start, T end) {
		return FilterPropagateResult::NO_PRUNING_POSSIBLE;
	}
	inline static size_t Size_implementation(AdditionalStats<T> *stats) {
		DictionaryAdditionalStats<T> *nstats = (DictionaryAdditionalStats<T> *)stats;
		return sizeof(*nstats) + nstats->dictionary.bucket_count() * (sizeof(void *)) +
		       nstats->dictionary.size() * sizeof(T);
	}
	inline static void Serialise_implementation(AdditionalStats<T> *stats) {
	}
	inline static void Deserialise_implementation(AdditionalStats<T> *stats) {
	}
};

} // namespace duckdb