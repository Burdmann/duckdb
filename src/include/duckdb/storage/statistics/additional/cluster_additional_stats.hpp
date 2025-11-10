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
#include "duckdb/common/operator/comparison_operators.hpp"
#include "duckdb/storage/statistics/base_statistics.hpp"
#include "duckdb/common/enums/expression_type.hpp"

namespace duckdb {

template <class T>
class ClusterAdditionalStats : public AdditionalStats<T> {
private:
	unsigned int cluster_count = 0;
	constexpr static uint32_t MAX_NUMBER_OF_CLUSTERS = 200;
	std::vector<T> min_values;
	std::vector<T> max_values;
	static bool ConstantExactRange(T min, T max, T constant) {
		return Equals::Operation(constant, min) && Equals::Operation(constant, max);
	}
	static bool ConstantValueInRange(T min, T max, T constant) {
		return !(LessThan::Operation(constant, min) || GreaterThan::Operation(constant, max));
	}
public:
	static constexpr char* name = "cluster";
	inline ClusterAdditionalStats(std::vector<T> &data) {
		this->Initialise = &Initialise_implementation;
		this->Query = &Query_implementation;
		this->Size = &Size_implementation;
		this->Serialise = &Serialise_implementation;
		this->Deserialise = &Deserialise_implementation;
		this->Initialise(data,this);
	}


	inline static void Initialise_implementation(std::vector<T> &data, AdditionalStats<T>* stats) {
		int size = data.size();
		if (size == 0)
			return;

		ClusterAdditionalStats<T>* nstats = static_cast<ClusterAdditionalStats<T>*>(stats);

		// sort the data
		std::sort(data.begin(), data.end());

		// compute gap sizes
		std::vector<std::pair<T, int>> gaps; // first element of each pair stores the gap, second is the index
		for (int i = 0; i < size - 1; i++) {
			gaps.push_back({data[i + 1] - data[i], i});
		}

		// select the k largest gaps
		T zero = (T)0;
		std::vector<int> idxs;
		std::sort(gaps.begin(), gaps.end(), std::greater<std::pair<T, int>>());

		for (int i = 0; i < std::min((unsigned long)gaps.size(), (unsigned long)(MAX_NUMBER_OF_CLUSTERS - 1));
		     i++) {
			if (gaps[i].first == zero)
				break;
			idxs.push_back(gaps[i].second);
		}

		// save min/max ranges defined by those gaps in nstats
		std::sort(idxs.begin(), idxs.end());
		T start = data[0];
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

	inline static FilterPropagateResult Query_inner(T min_value, T max_value, ExpressionType comparison_type, T constant) {
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
	inline static FilterPropagateResult Query_implementation(AdditionalStats<T>* stats, ExpressionType comparison_type, T constant) {
		ClusterAdditionalStats<T>* nstats = static_cast<ClusterAdditionalStats<T>*>(stats);
		for (int i = 0; i < nstats->min_values.size(); i++) {
			FilterPropagateResult result = Query_inner(nstats->min_values[i], nstats->max_values[i], comparison_type, constant);
			if (result == FilterPropagateResult::FILTER_ALWAYS_TRUE) {
				return FilterPropagateResult::FILTER_ALWAYS_TRUE;
			} else if (result == FilterPropagateResult::NO_PRUNING_POSSIBLE)
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		}
		return FilterPropagateResult::FILTER_ALWAYS_FALSE;
	}
	inline static size_t Size_implementation(AdditionalStats<T>* stats) {
		ClusterAdditionalStats<T>* nstats = static_cast<ClusterAdditionalStats<T>*>(stats);
		return 2*sizeof(T)*nstats->cluster_count;
	}
	inline static void Serialise_implementation(AdditionalStats<T>* stats) {
	}
	inline static void Deserialise_implementation(AdditionalStats<T>* stats) {
	}
};

} // namespace duckdb