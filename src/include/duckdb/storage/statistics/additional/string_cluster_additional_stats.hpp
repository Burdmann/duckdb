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
#include "duckdb/common/enums/expression_type.hpp"
#include "duckdb/common/types/string_type.hpp"
#include "duckdb/common/operator/comparison_operators.hpp"

namespace duckdb {

class StringClusterAdditionalStats : public AdditionalStats<string_t> {
private:
	constexpr static uint32_t MAX_STRING_MINMAX_SIZE = 8;
	constexpr static uint32_t MAX_NUMBER_OF_CLUSTERS = 2000;
	unsigned int cluster_count = 0;
	std::vector<string_t> min_values;
	std::vector<string_t> max_values;
	static bool ConstantExactRange(string_t min, string_t max, string_t constant) {
		return Equals::Operation(constant, min) && Equals::Operation(constant, max);
	}
	static bool ConstantValueInRange(string_t min, string_t max, string_t constant) {
		return !(LessThan::Operation(constant, min) || GreaterThan::Operation(constant, max));
	}
	static inline unsigned long long StringToLong(const string_t str) {
		unsigned long long res = 0;
		const char *data = str.GetData();
		for (int i = 0; i < str.GetSize(); i++) {
			res <<= 7;
			res += data[i];
		}
		return res;
	}

	static int StringValueComparison(const_data_ptr_t data, idx_t len, const_data_ptr_t comparison) {
		for (idx_t i = 0; i < len; i++) {
			if (data[i] < comparison[i]) {
				return -1;
			} else if (data[i] > comparison[i]) {
				return 1;
			}
		}
		return 0;
	}

	static string_t ConstructValue(string_t input) {
		data_t *data = (data_t *)input.GetData();
		idx_t size = input.GetSize();
		string_t res(MAX_STRING_MINMAX_SIZE);
		data_t *target = (data_t *)res.GetData();
		idx_t value_size = size > MAX_STRING_MINMAX_SIZE ? MAX_STRING_MINMAX_SIZE : size;
		memcpy(target, data, value_size);
		for (idx_t i = value_size; i < MAX_STRING_MINMAX_SIZE; i++) {
			target[i] = '\0';
		}
		return res;
	}

public:
	static inline const char *GetStaticName() {
		return "cluster";
	}
	inline StringClusterAdditionalStats(std::vector<string_t> &data) {
		this->name = GetStaticName();
		this->Initialise = &Initialise_implementation;
		this->Query = &Query_implementation;
		this->Size = &Size_implementation;
		this->Serialise = &Serialise_implementation;
		this->Deserialise = &Deserialise_implementation;
		this->Initialise(data, this);
	}

	inline static void Initialise_implementation(std::vector<string_t> &data, AdditionalStats<string_t> *stats) {
		int size = data.size();
		if (size == 0)
			return;

		StringClusterAdditionalStats *nstats = (StringClusterAdditionalStats *)stats;
		nstats->cluster_count = 0;

		// sort the data
		std::sort(data.begin(), data.end());

		// compute gap sizes
		std::vector<std::pair<unsigned long long, int>>
		    gaps; // first element of each pair stores the gap, second is the index
		for (int i = 0; i < size - 1; i++) {
			gaps.push_back({StringToLong(data[i + 1]) - StringToLong(data[i]), i});
		}

		// select the k largest gaps
		std::vector<int> idxs;
		std::sort(gaps.begin(), gaps.end(), std::greater<std::pair<unsigned long long, int>>());

		for (int i = 0; i < std::min((unsigned long)gaps.size(), (unsigned long)(MAX_NUMBER_OF_CLUSTERS - 1)); i++) {
			if (gaps[i].first == 0)
				break;
			idxs.push_back(gaps[i].second);
		}

		// save min/max ranges defined by those gaps in nstats
		std::sort(idxs.begin(), idxs.end());
		string_t start = ConstructValue(data[0]);
		for (int idx : idxs) {
			nstats->min_values.push_back(ConstructValue(start));
			nstats->max_values.push_back(ConstructValue(data[idx]));
			start = data[idx + 1];
			nstats->cluster_count++;
		}

		nstats->min_values.push_back(ConstructValue(start));
		nstats->max_values.push_back(ConstructValue(data.back()));
		nstats->cluster_count++;
	}

	inline static FilterPropagateResult Query_inner(string_t min_value, string_t max_value,
	                                                ExpressionType comparison_type, string_t constant) {
		auto data = const_data_ptr_cast(constant.GetData());
		idx_t size = constant.GetSize();

		int min_comp = StringValueComparison(data, MinValue((idx_t)MAX_STRING_MINMAX_SIZE, size),
		                                     (duckdb::data_t *)min_value.GetData());
		int max_comp = StringValueComparison(data, MinValue((idx_t)MAX_STRING_MINMAX_SIZE, size),
		                                     (duckdb::data_t *)max_value.GetData());
		switch (comparison_type) {
		case ExpressionType::COMPARE_EQUAL:
		case ExpressionType::COMPARE_NOT_DISTINCT_FROM:
			if (min_comp >= 0 && max_comp <= 0) {
				return FilterPropagateResult::NO_PRUNING_POSSIBLE;
			} else {
				return FilterPropagateResult::FILTER_ALWAYS_FALSE;
			}
		case ExpressionType::COMPARE_NOTEQUAL:
		case ExpressionType::COMPARE_DISTINCT_FROM:
			if (min_comp < 0 || max_comp > 0) {
				return FilterPropagateResult::FILTER_ALWAYS_TRUE;
			}
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		case ExpressionType::COMPARE_GREATERTHANOREQUALTO:
		case ExpressionType::COMPARE_GREATERTHAN:
			if (max_comp <= 0) {
				return FilterPropagateResult::NO_PRUNING_POSSIBLE;
			} else {
				return FilterPropagateResult::FILTER_ALWAYS_FALSE;
			}
		case ExpressionType::COMPARE_LESSTHAN:
		case ExpressionType::COMPARE_LESSTHANOREQUALTO:
			if (min_comp >= 0) {
				return FilterPropagateResult::NO_PRUNING_POSSIBLE;
			} else {
				return FilterPropagateResult::FILTER_ALWAYS_FALSE;
			}
		default:
			throw InternalException("Expression type not implemented for string statistics zone map");
		}
	}
	inline static FilterPropagateResult Query_implementation(AdditionalStats<string_t> *stats,
	                                                         ExpressionType comparison_type, string_t constant) {
		StringClusterAdditionalStats *nstats = (StringClusterAdditionalStats *)stats;
		if (nstats->cluster_count == 0)
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		for (int i = 0; i < nstats->min_values.size(); i++) {
			FilterPropagateResult result =
			    Query_inner(nstats->min_values[i], nstats->max_values[i], comparison_type, constant);
			if (result == FilterPropagateResult::FILTER_ALWAYS_TRUE) {
				return FilterPropagateResult::NO_PRUNING_POSSIBLE;
			} else if (result == FilterPropagateResult::NO_PRUNING_POSSIBLE)
				return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		}
		return FilterPropagateResult::FILTER_ALWAYS_FALSE;
	}
	inline static size_t Size_implementation(AdditionalStats<string_t> *stats) {
		StringClusterAdditionalStats *nstats = (StringClusterAdditionalStats *)stats;
		return 2 * (sizeof(string_t) + MAX_NUMBER_OF_CLUSTERS) * nstats->cluster_count + sizeof(*nstats);
	}
	inline static void Serialise_implementation(AdditionalStats<string_t> *stats) {
	}
	inline static void Deserialise_implementation(AdditionalStats<string_t> *stats) {
	}
};

} // namespace duckdb