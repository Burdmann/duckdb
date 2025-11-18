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
	constexpr static uint32_t MAX_NUMBER_OF_CLUSTERS = 2000;
	std::vector<T> min_values;
	std::vector<T> max_values;
	static bool ConstantExactRange(T min, T max, T constant) {
		return Equals::Operation(constant, min) && Equals::Operation(constant, max);
	}
	static bool ConstantValueInRange(T min, T max, T constant) {
		return !(LessThan::Operation(constant, min) || GreaterThan::Operation(constant, max));
	}

public:
	static constexpr char *static_name = "cluster";
	inline ClusterAdditionalStats(std::vector<T> &data) {
		this->name = static_name;
		this->Initialise = &Initialise_implementation;
		this->Query = &Query_implementation;
		this->Size = &Size_implementation;
		this->Serialise = &Serialise_implementation;
		this->Deserialise = &Deserialise_implementation;
		this->Initialise(data, this);
	}

	inline static void Initialise_implementation(std::vector<T> &data, AdditionalStats<T> *stats) {
		int size = data.size();
		if (size == 0)
			return;

		ClusterAdditionalStats<T> *nstats = static_cast<ClusterAdditionalStats<T> *>(stats);

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

		for (int i = 0; i < std::min((unsigned long)gaps.size(), (unsigned long)(MAX_NUMBER_OF_CLUSTERS - 1)); i++) {
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

	inline static idx_t FindLastIndexBeforePoint_Binary(std::vector<T> min_values, T constant) {
		idx_t lo = 0;
		idx_t hi = min_values.size() - 1;
		idx_t mid;
		while (lo < hi) {
			mid = (lo + hi + 1) / 2;
			if (min_values[mid] > constant) {
				hi = mid - 1;
			} else if (min_values[mid] < constant) {
				lo = mid;
			} else {
				// min_values[mid] = constant
				return mid;
			}
		}
		if (min_values[mid] > constant)
			return -1;
		return mid;
	}
	inline static idx_t FindLastIndexBeforePoint_Linear(std::vector<T> min_values, T constant) {
		for (int i = 0; i < min_values.size(); i++) {
			if (min_values[i] > constant) {
				return i - 1;
			}
		}
		return min_values.size() - 1;
	}

	inline static FilterPropagateResult Query_Equal(std::shared_ptr<ClusterAdditionalStats<T>> nstats, T constant) {
		int idx = FindLastIndexBeforePoint_Linear(nstats->min_values, constant);
		if (idx == -1) {
			return FilterPropagateResult::FILTER_ALWAYS_FALSE;
		} else if (nstats->min_values.size() == 1 &&
		           ConstantExactRange(nstats->min_values[idx], nstats->max_values[idx], constant)) {
			return FilterPropagateResult::FILTER_ALWAYS_TRUE;
		} else if (nstats->max_values[idx] >= constant) {
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		} else {
			return FilterPropagateResult::FILTER_ALWAYS_FALSE;
		}
	}

	inline static FilterPropagateResult Query_NotEqual(std::shared_ptr<ClusterAdditionalStats<T>> nstats, T constant) {
		return FilterPropagateResult::NO_PRUNING_POSSIBLE;
	}
	inline static FilterPropagateResult Query_GreaterThanEqual(std::shared_ptr<ClusterAdditionalStats<T>> nstats,
	                                                           T constant) {
		return FilterPropagateResult::NO_PRUNING_POSSIBLE;
	}
	inline static FilterPropagateResult Query_GreaterThan(std::shared_ptr<ClusterAdditionalStats<T>> nstats,
	                                                      T constant) {
		return FilterPropagateResult::NO_PRUNING_POSSIBLE;
	}
	inline static FilterPropagateResult Query_LessThanEqual(std::shared_ptr<ClusterAdditionalStats<T>> nstats,
	                                                        T constant) {
		return FilterPropagateResult::NO_PRUNING_POSSIBLE;
	}
	inline static FilterPropagateResult Query_LessThan(std::shared_ptr<ClusterAdditionalStats<T>> nstats, T constant) {
		return FilterPropagateResult::NO_PRUNING_POSSIBLE;
	}
	inline static FilterPropagateResult Query_implementation(std::shared_ptr<AdditionalStats<T>> stats,
	                                                         ExpressionType comparison_type, T constant) {
		std::shared_ptr<ClusterAdditionalStats<T>> nstats = std::static_pointer_cast<ClusterAdditionalStats<T>>(stats);
		switch (comparison_type) {
		case ExpressionType::COMPARE_EQUAL:
		case ExpressionType::COMPARE_NOT_DISTINCT_FROM:
			return Query_Equal(nstats, constant);
		case ExpressionType::COMPARE_NOTEQUAL:
		case ExpressionType::COMPARE_DISTINCT_FROM:
			return Query_NotEqual(nstats, constant);
		case ExpressionType::COMPARE_GREATERTHANOREQUALTO:
			return Query_GreaterThanEqual(nstats, constant);
		case ExpressionType::COMPARE_GREATERTHAN:
			return Query_GreaterThan(nstats, constant);
		case ExpressionType::COMPARE_LESSTHANOREQUALTO:
			return Query_LessThanEqual(nstats, constant);
		case ExpressionType::COMPARE_LESSTHAN:
			return Query_LessThan(nstats, constant);
		default:
			throw InternalException("Expression type in zonemap check not implemented");
		}
	}
	inline static size_t Size_implementation(std::shared_ptr<AdditionalStats<T>> stats) {
		std::shared_ptr<ClusterAdditionalStats<T>> nstats = std::static_pointer_cast<ClusterAdditionalStats<T>>(stats);
		return 2 * sizeof(T) * nstats->cluster_count + sizeof(*nstats);
	}
	inline static void Serialise_implementation(std::shared_ptr<AdditionalStats<T>> stats) {
	}
	inline static void Deserialise_implementation(std::shared_ptr<AdditionalStats<T>> stats) {
	}
};

} // namespace duckdb