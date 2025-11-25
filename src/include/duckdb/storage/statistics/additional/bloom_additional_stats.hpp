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
class BloomAdditionalStats : public AdditionalStats<T> {
private:
	// following numbers are chosen based on https://www.vldb.org/pvldb/vol12/p502-lang.pdf

	// for cache-sized blocks
	// constexpr static uint32_t K = 7;
	// constexpr static uint32_t BLOCK_COUNT = 625;
	// constexpr static uint32_t BLOCK_SIZE = 8; // the unit here is word lengths (64 bits)

	// for register-sized blocks
	constexpr static uint32_t K = 5;
	constexpr static uint32_t BLOCK_COUNT = 5000;
	constexpr static uint32_t BLOCK_SIZE = 1; // the unit here is word lengths (64 bits)
	uint64_t bit_array[BLOCK_COUNT * BLOCK_SIZE];

	static inline uint64_t *GetBlock(uint32_t h1, uint32_t h2, uint64_t *bit_array) {
		uint32_t block_idx = h1 % BLOCK_COUNT;
		uint32_t byte_idx = block_idx * BLOCK_SIZE;
		return bit_array + byte_idx;
	}
	static inline bool QueryUtil(T value, uint64_t *bit_array) {
		size_t h = std::hash<T> {}(value);
		uint32_t h1 = h & ((1ull << 32) - 1);
		uint32_t h2 = h >> 32;

		bool result = true;
		uint64_t *block = GetBlock(h1, h2, bit_array);
		for (int i = 1; i <= K; i++) {
			uint32_t bit_pos = (h1 + i * h2) % (64 * BLOCK_SIZE);
			uint64_t bit_idx = bit_pos % 64;
			uint64_t byte_idx = bit_pos / 64;
			result &= (block[byte_idx] >> bit_idx) & 1;
		}
		return result;
	}
	static inline void Insert(T new_value, uint64_t *bit_array) {
		size_t h = std::hash<T> {}(new_value);
		uint32_t h1 = h & ((1ull << 32) - 1);
		uint32_t h2 = h >> 32;

		// chosse block
		uint64_t *block = GetBlock(h1, h2, bit_array);

		// chosse k bits within the block to set
		for (int i = 1; i <= K; i++) {
			uint32_t bit_pos = (h1 + i * h2) % (64 * BLOCK_SIZE);
			uint64_t bit_idx = bit_pos % 64;
			uint64_t byte_idx = bit_pos / 64;
			block[byte_idx] |= (1 << bit_idx);
		}
	}

public:
	static constexpr char *static_name = "bloom";
	inline BloomAdditionalStats(std::vector<T> &data) {
		this->name = static_name;
		this->Initialise = &Initialise_implementation;
		this->Query = &Query_implementation;
		this->Size = &Size_implementation;
		this->Serialise = &Serialise_implementation;
		this->Deserialise = &Deserialise_implementation;
		this->Initialise(data, this);
	}

	inline static void Initialise_implementation(std::vector<T> &data, AdditionalStats<T> *stats) {
		BloomAdditionalStats<T> *nstats = static_cast<BloomAdditionalStats<T> *>(stats);
		for (T element : data) {
			Insert(element, nstats->bit_array);
		}
	}

	inline static FilterPropagateResult Query_implementation(AdditionalStats<T> *stats, ExpressionType comparison_type,
	                                                         T constant) {
		switch (comparison_type) {
		case ExpressionType::COMPARE_EQUAL:
		case ExpressionType::COMPARE_NOT_DISTINCT_FROM: {
			BloomAdditionalStats<T> *nstats = (BloomAdditionalStats<T>)stats;
			if (QueryUtil(constant, nstats->bit_array))
				return FilterPropagateResult::NO_PRUNING_POSSIBLE;

			return FilterPropagateResult::FILTER_ALWAYS_FALSE;
		}
		default:
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		}
	}
	inline static size_t Size_implementation(AdditionalStats<T> *stats) {
		BloomAdditionalStats<T> *nstats = (BloomAdditionalStats<T> *)stats;
		return sizeof(*nstats.get());
	}
	inline static void Serialise_implementation(AdditionalStats<T> *stats) {
	}
	inline static void Deserialise_implementation(AdditionalStats<T> *stats) {
	}
};

} // namespace duckdb