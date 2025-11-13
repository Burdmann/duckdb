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
	constexpr static uint32_t MAX_NUMBER_OF_CLUSTERS = 200;
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

public:
	static constexpr char *static_name = "cluster";
	inline StringClusterAdditionalStats(std::vector<string_t> &data) {
		this->name = static_name;
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

		StringClusterAdditionalStats *nstats = static_cast<StringClusterAdditionalStats *>(stats);
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
		string_t start = data[0];
		for (int idx : idxs) {
			nstats->min_values.push_back(start);
			nstats->max_values.push_back(data[idx]);
			start = data[idx + 1];
			nstats->cluster_count++;
		}

		nstats->min_values.push_back(start);
		nstats->max_values.push_back(data.back());
		nstats->cluster_count++;
	}

	inline static FilterPropagateResult Query_inner(string_t min_value, string_t max_value,
	                                                ExpressionType comparison_type, string_t constant) {
		switch (comparison_type) {
		case ExpressionType::COMPARE_EQUAL:
		case ExpressionType::COMPARE_NOT_DISTINCT_FROM:
			if (ConstantExactRange(min_value, max_value, constant)) {
				return FilterPropagateResult::FILTER_ALWAYS_TRUE;
			}
			if (ConstantValueInRange(min_value, max_value, constant)) {
				return FilterPropagateResult::NO_PRUNING_POSSIBLE;
			}
			return FilterPropagateResult::FILTER_ALWAYS_FALSE;
		case ExpressionType::COMPARE_NOTEQUAL:
		case ExpressionType::COMPARE_DISTINCT_FROM:
			if (!ConstantValueInRange(min_value, max_value, constant)) {
				return FilterPropagateResult::FILTER_ALWAYS_TRUE;
			} else if (ConstantExactRange(min_value, max_value, constant)) {
				// corner case of a cluster with one numeric equal to the target constant
				return FilterPropagateResult::FILTER_ALWAYS_FALSE;
			}
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		case ExpressionType::COMPARE_GREATERTHANOREQUALTO:
			// GreaterThanEquals::Operation(X, C)
			// this can be true only if max(X) >= C
			// if min(X) >= C, then this is always true
			if (GreaterThanEquals::Operation(min_value, constant)) {
				return FilterPropagateResult::FILTER_ALWAYS_TRUE;
			} else if (GreaterThanEquals::Operation(max_value, constant)) {
				return FilterPropagateResult::NO_PRUNING_POSSIBLE;
			} else {
				return FilterPropagateResult::FILTER_ALWAYS_FALSE;
			}
		case ExpressionType::COMPARE_GREATERTHAN:
			// GreaterThan::Operation(X, C)
			// this can be true only if max(X) > C
			// if min(X) > C, then this is always true
			if (GreaterThan::Operation(min_value, constant)) {
				return FilterPropagateResult::FILTER_ALWAYS_TRUE;
			} else if (GreaterThan::Operation(max_value, constant)) {
				return FilterPropagateResult::NO_PRUNING_POSSIBLE;
			} else {
				return FilterPropagateResult::FILTER_ALWAYS_FALSE;
			}
		case ExpressionType::COMPARE_LESSTHANOREQUALTO:
			// LessThanEquals::Operation(X, C)
			// this can be true only if min(X) <= C
			// if max(X) <= C, then this is always true
			if (LessThanEquals::Operation(max_value, constant)) {
				return FilterPropagateResult::FILTER_ALWAYS_TRUE;
			} else if (LessThanEquals::Operation(min_value, constant)) {
				return FilterPropagateResult::NO_PRUNING_POSSIBLE;
			} else {
				return FilterPropagateResult::FILTER_ALWAYS_FALSE;
			}
		case ExpressionType::COMPARE_LESSTHAN:
			// LessThan::Operation(X, C)
			// this can be true only if min(X) < C
			// if max(X) < C, then this is always true
			if (LessThan::Operation(max_value, constant)) {
				return FilterPropagateResult::FILTER_ALWAYS_TRUE;
			} else if (LessThan::Operation(min_value, constant)) {
				return FilterPropagateResult::NO_PRUNING_POSSIBLE;
			} else {
				return FilterPropagateResult::FILTER_ALWAYS_FALSE;
			}
		default:
			throw InternalException("Expression type in zonemap check not implemented");
		}
	}
	inline static FilterPropagateResult Query_implementation(AdditionalStats<string_t> *stats,
	                                                         ExpressionType comparison_type, string_t constant) {
		StringClusterAdditionalStats *nstats = static_cast<StringClusterAdditionalStats *>(stats);
		for (int i = 0; i < nstats->min_values.size(); i++) {
			FilterPropagateResult result =
			    Query_inner(nstats->min_values[i], nstats->min_values[i], comparison_type, constant);
			if (result == FilterPropagateResult::FILTER_ALWAYS_TRUE) {
				return FilterPropagateResult::FILTER_ALWAYS_TRUE;
			} else if (result == FilterPropagateResult::NO_PRUNING_POSSIBLE)
				return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		}
		return FilterPropagateResult::FILTER_ALWAYS_FALSE;
	}
	inline static size_t Size_implementation(AdditionalStats<string_t> *stats) {
		StringClusterAdditionalStats *nstats = static_cast<StringClusterAdditionalStats *>(stats);
		return 2 * sizeof(string_t) * nstats->cluster_count + sizeof(*stats);
	}
	inline static void Serialise_implementation(AdditionalStats<string_t> *stats) {
	}
	inline static void Deserialise_implementation(AdditionalStats<string_t> *stats) {
	}
};

} // namespace duckdb