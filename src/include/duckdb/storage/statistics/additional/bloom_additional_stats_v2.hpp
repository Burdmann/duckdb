//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/storage/statistics/additional/bloom_additional_stats_v2.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include <functional>
#include "duckdb/storage/statistics/additional/additional_stats.hpp"
#include "duckdb/common/enums/filter_propagate_result.hpp"
#include <unordered_set>

namespace duckdb {

template <class T>
class BloomAdditionalStats2 : public AdditionalStats<T> {
private:
	// following numbers are chosen based on https://www.vldb.org/pvldb/vol12/p502-lang.pdf

	// for cache-sized blocks
	// constexpr static uint32_t K = 7;
	// constexpr static uint32_t BLOCK_COUNT = 625;
	// constexpr static uint32_t BLOCK_SIZE = 8; // the unit here is word lengths (64 bits)

	// for register-sized blocks
	constexpr static uint32_t K = 3;
	constexpr static uint32_t BLOCK_COUNT = 1;
	constexpr static uint32_t MAX_BLOCK_SIZE = 400; // the unit here is word lengths (64 bits)
	constexpr static uint32_t TARGET_BITS_PER_KEY = 3;
	constexpr static uint32_t MIN_BITS_PER_KEY = 1;

	std::vector<uint64_t> bit_array;
	uint32_t size;
	bool overfull = false;

	// https://github.com/PeterScott/murmur3
	static inline uint64_t rotl64(uint64_t x, int8_t r) {
		return (x << r) | (x >> (64 - r));
	}
	static inline uint64_t fmix64(uint64_t k) {
		k ^= k >> 33;
		k *= 0xff51afd7ed558ccdLLU;
		k ^= k >> 33;
		k *= 0xc4ceb9fe1a85ec53LLU;
		k ^= k >> 33;

		return k;
	}
	static inline uint64_t MurmurHash3_x64_128(const void *key, const int len, const uint32_t seed) {
		const uint8_t *data = (const uint8_t *)key;
		const int nblocks = len / 16;
		int i;

		uint64_t h1 = seed;
		uint64_t h2 = seed;

		uint64_t c1 = 0x87c37b91114253d5LLU;
		uint64_t c2 = 0x4cf5ad432745937fLLU;

		//----------
		// body

		const uint64_t *blocks = (const uint64_t *)(data);

		for (i = 0; i < nblocks; i++) {
			uint64_t k1 = blocks[i * 2 + 0];
			uint64_t k2 = blocks[i * 2 + 1];

			k1 *= c1;
			k1 = rotl64(k1, 31);
			k1 *= c2;
			h1 ^= k1;

			h1 = rotl64(h1, 27);
			h1 += h2;
			h1 = h1 * 5 + 0x52dce729;

			k2 *= c2;
			k2 = rotl64(k2, 33);
			k2 *= c1;
			h2 ^= k2;

			h2 = rotl64(h2, 31);
			h2 += h1;
			h2 = h2 * 5 + 0x38495ab5;
		}

		//----------
		// tail

		const uint8_t *tail = (const uint8_t *)(data + nblocks * 16);

		uint64_t k1 = 0;
		uint64_t k2 = 0;

		switch (len & 15) {
		case 15:
			k2 ^= (uint64_t)(tail[14]) << 48;
		case 14:
			k2 ^= (uint64_t)(tail[13]) << 40;
		case 13:
			k2 ^= (uint64_t)(tail[12]) << 32;
		case 12:
			k2 ^= (uint64_t)(tail[11]) << 24;
		case 11:
			k2 ^= (uint64_t)(tail[10]) << 16;
		case 10:
			k2 ^= (uint64_t)(tail[9]) << 8;
		case 9:
			k2 ^= (uint64_t)(tail[8]) << 0;
			k2 *= c2;
			k2 = rotl64(k2, 33);
			k2 *= c1;
			h2 ^= k2;

		case 8:
			k1 ^= (uint64_t)(tail[7]) << 56;
		case 7:
			k1 ^= (uint64_t)(tail[6]) << 48;
		case 6:
			k1 ^= (uint64_t)(tail[5]) << 40;
		case 5:
			k1 ^= (uint64_t)(tail[4]) << 32;
		case 4:
			k1 ^= (uint64_t)(tail[3]) << 24;
		case 3:
			k1 ^= (uint64_t)(tail[2]) << 16;
		case 2:
			k1 ^= (uint64_t)(tail[1]) << 8;
		case 1:
			k1 ^= (uint64_t)(tail[0]) << 0;
			k1 *= c1;
			k1 = rotl64(k1, 31);
			k1 *= c2;
			h1 ^= k1;
		};

		//----------
		// finalization

		h1 ^= len;
		h2 ^= len;

		h1 += h2;
		h2 += h1;

		h1 = fmix64(h1);
		h2 = fmix64(h2);

		h1 += h2;
		h2 += h1;

		return h1;
	}

	static inline size_t hash(T value) {
		size_t h = std::hash<T> {}(value);
		return MurmurHash3_x64_128(&h, sizeof(size_t), 1);
	}

	static inline uint64_t *GetBlock(uint32_t h1, uint32_t h2, uint64_t *bit_array, uint32_t size) {
		return bit_array;
	}
	static inline bool QueryUtil(T value, uint64_t *bit_array, uint32_t size) {
		size_t h = hash(value);
		uint32_t h1 = h & ((1ull << 32) - 1);
		uint32_t h2 = h >> 32;

		bool result = true;
		uint64_t *block = GetBlock(h1, h2, bit_array, size);
		for (int i = 1; i <= K; i++) {
			uint32_t bit_pos = (h1 + i * h2) % (64 * size);
			uint64_t bit_idx = bit_pos % 64;
			uint64_t byte_idx = bit_pos / 64;
			result = result && ((block[byte_idx] >> bit_idx) & 1);
		}
		return result;
	}
	static inline void Insert(T new_value, uint64_t *bit_array, uint32_t size) {
		size_t h = hash(new_value);
		uint32_t h1 = h & ((1ull << 32) - 1);
		uint32_t h2 = h >> 32;

		// chosse block
		uint64_t *block = GetBlock(h1, h2, bit_array, size);

		// chosse k bits within the block to set
		for (int i = 1; i <= K; i++) {
			uint32_t bit_pos = (h1 + i * h2) % (64 * size);
			uint64_t bit_idx = bit_pos % 64;
			uint64_t byte_idx = bit_pos / 64;
			block[byte_idx] |= (1ull << bit_idx);
		}
	}

public:
	static inline const char *GetStaticName() {
		return "bloom v2";
	}
	inline BloomAdditionalStats2(std::vector<T> &data) {
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
		BloomAdditionalStats2<T> *nstats = (BloomAdditionalStats2<T> *)stats;
		std::unordered_set<T> dictionary;
		for (T element : data) {
			dictionary.insert(element);
		}

		if (dictionary.size() * MIN_BITS_PER_KEY > MAX_BLOCK_SIZE * BLOCK_COUNT * 64) {
			nstats->overfull = true;
		}
		nstats->size =
		    std::min((unsigned int)dictionary.size() * TARGET_BITS_PER_KEY / 64, MAX_BLOCK_SIZE * BLOCK_COUNT);
		nstats->bit_array.resize(nstats->size);

		for (T element : data) {
			Insert(element, nstats->bit_array.data(), nstats->size);
		}
	}

	inline static FilterPropagateResult Query_implementation(AdditionalStats<T> *stats, ExpressionType comparison_type,
	                                                         T constant) {
		switch (comparison_type) {
		case ExpressionType::COMPARE_EQUAL:
		case ExpressionType::COMPARE_NOT_DISTINCT_FROM: {
			BloomAdditionalStats2<T> *nstats = (BloomAdditionalStats2<T> *)stats;
			if (nstats->overfull || QueryUtil(constant, nstats->bit_array.data(), nstats->size))
				return FilterPropagateResult::NO_PRUNING_POSSIBLE;

			return FilterPropagateResult::FILTER_ALWAYS_FALSE;
		}
		default:
			return FilterPropagateResult::NO_PRUNING_POSSIBLE;
		}
	}

	inline static FilterPropagateResult QueryRange_implementation(AdditionalStats<T> *stats, T start, T end) {
		return FilterPropagateResult::NO_PRUNING_POSSIBLE;
	}

	inline static size_t Size_implementation(AdditionalStats<T> *stats) {
		BloomAdditionalStats2<T> *nstats = (BloomAdditionalStats2<T> *)stats;
		return sizeof(*nstats) + 64 * nstats->size;
	}
	inline static void Serialise_implementation(AdditionalStats<T> *stats) {
	}
	inline static void Deserialise_implementation(AdditionalStats<T> *stats) {
	}
};

} // namespace duckdb