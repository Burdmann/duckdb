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
#include "duckdb/common/operator/comparison_operators.hpp"
#include "duckdb/storage/statistics/base_statistics.hpp"
#include "duckdb/common/enums/expression_type.hpp"

#include "duckdb/common/serializer/serializer.hpp"
#include "duckdb/common/serializer/deserializer.hpp"

namespace duckdb {

// to allow use of dictionaries only where there are few enough items in a partition
constexpr static uint32_t MAX_NUMBER_OF_ITEMS = 58;

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
		this->type = ADDITIONAL_STATS_TYPE::DICTIONARY;
	}
	inline static void Initialise_implementation(std::vector<T> &data, AdditionalStats<T> *stats) {
		DictionaryAdditionalStats<T> *nstats = (DictionaryAdditionalStats<T> *)stats;
		for (T item : data) {
			nstats->dictionary.insert(item);
			if (nstats->dictionary.size() > MAX_NUMBER_OF_ITEMS)
				break;
		}
		if (nstats->dictionary.size() > MAX_NUMBER_OF_ITEMS) {
			nstats->overfull = true;
			nstats->dictionary.clear();
			nstats->dictionary.rehash(1);
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
	inline static void Serialise_implementation(AdditionalStats<T> *stats, Serializer &serializer) {
		DictionaryAdditionalStats<T> *nstats = (DictionaryAdditionalStats<T> *)stats;
		serializer.WriteProperty(1001, "dictionary:overfull", nstats->overfull);
		serializer.WriteProperty(1002, "dictionary:size", nstats->dictionary.size());
		for (T item : nstats->dictionary) {
			serializer.WriteProperty(1003, "dictionary:item", item);
		}
	}
	inline static void Deserialise_implementation(AdditionalStats<T> *stats, Deserializer &deserializer) {
		DictionaryAdditionalStats<T> *nstats = (DictionaryAdditionalStats<T> *)stats;
		nstats->overfull = deserializer.template ReadProperty<bool>(1001, "dictionary:overfull");
		auto size = deserializer.template ReadProperty<unsigned int>(1002, "dictionary:size");
		for (int i = 0; i < size; i++) {
			auto item = deserializer.template ReadProperty<T>(1003, "dictionary:item");
			nstats->dictionary.insert(item);
		}
	}
};

template <>
class DictionaryAdditionalStats<string_t> : public AdditionalStats<string_t> {
private:
	bool overfull = false;
	std::unordered_set<string_t> dictionary;

public:
	static inline const char *GetStaticName() {
		return "dictionary";
	}
	inline DictionaryAdditionalStats(std::vector<string_t> &data) {
		this->name = GetStaticName();
		this->Initialise = &Initialise_implementation;
		this->Query = &Query_implementation;
		this->QueryRange = &QueryRange_implementation;
		this->Size = &Size_implementation;
		this->Serialise = &Serialise_implementation;
		this->Deserialise = &Deserialise_implementation;
		this->Initialise(data, this);
		this->type = ADDITIONAL_STATS_TYPE::DICTIONARY;
	}
	inline static void Initialise_implementation(std::vector<string_t> &data, AdditionalStats<string_t> *stats) {
		DictionaryAdditionalStats<string_t> *nstats = (DictionaryAdditionalStats<string_t> *)stats;
		for (string_t item : data) {
			nstats->dictionary.insert(item);
			if (nstats->dictionary.size() > MAX_NUMBER_OF_ITEMS)
				break;
		}
		if (nstats->dictionary.size() > MAX_NUMBER_OF_ITEMS) {
			nstats->overfull = true;
			nstats->dictionary.clear();
			nstats->dictionary.rehash(1);
		}
	}
	inline static FilterPropagateResult Query_implementation(AdditionalStats<string_t> *stats,
	                                                         ExpressionType comparison_type, string_t constant) {
		DictionaryAdditionalStats<string_t> *nstats = (DictionaryAdditionalStats<string_t> *)stats;
		if (nstats->overfull || nstats->dictionary.find(constant) != nstats->dictionary.end())
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		return FilterPropagateResult::FILTER_ALWAYS_FALSE;
	}
	inline static FilterPropagateResult QueryRange_implementation(AdditionalStats<string_t> *stats, string_t start,
	                                                              string_t end) {
		return FilterPropagateResult::NO_PRUNING_POSSIBLE;
	}
	inline static size_t Size_implementation(AdditionalStats<string_t> *stats) {
		DictionaryAdditionalStats<string_t> *nstats = (DictionaryAdditionalStats<string_t> *)stats;
		return sizeof(*nstats) + nstats->dictionary.bucket_count() * (sizeof(void *)) +
		       nstats->dictionary.size() * sizeof(string_t);
	}
	inline static void Serialise_implementation(AdditionalStats<string_t> *stats, Serializer &serializer) {
		DictionaryAdditionalStats<string_t> *nstats = (DictionaryAdditionalStats<string_t> *)stats;
	}
	inline static void Deserialise_implementation(AdditionalStats<string_t> *stats, Deserializer &deserializer) {
		DictionaryAdditionalStats<string_t> *nstats = (DictionaryAdditionalStats<string_t> *)stats;
	}
};

} // namespace duckdb