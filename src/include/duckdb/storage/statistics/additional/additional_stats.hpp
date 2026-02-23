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
// #include "duckdb/common/serializer/serializer.hpp"
// #include "duckdb/common/serializer/deserializer.hpp"

namespace duckdb {
template <class T>
class EmptyAdditionalStats;
template <class T>
class ClusterAdditionalStats;
template <class T>
class BloomAdditionalStats;
template <class T>
class BloomAdditionalStats2;
template <class T>
class AlwaysPruneAdditionalStats;
template <class T>
class DictionaryAdditionalStats;

enum class ADDITIONAL_STATS_TYPE : uint8_t {
	ERROR = 0,
	EMPTY = 1,
	CLUSTER = 2,
	BLOOM = 3,
	BLOOM2 = 4,
	DICTIONARY = 5,
	ALWAYS_PRUNE = 6,
};

template <class T>
class AdditionalStats {
public:
	std::function<void(std::vector<T> &, AdditionalStats<T> *)> Initialise;
	std::function<FilterPropagateResult(AdditionalStats<T> *, ExpressionType, T)> Query;
	std::function<FilterPropagateResult(AdditionalStats<T> *, T, T)> QueryRange;
	std::function<size_t(AdditionalStats<T> *)> Size;
	std::function<void(AdditionalStats<T> *, Serializer &)> Serialise;
	std::function<void(AdditionalStats<T> *, Deserializer &)> Deserialise;
	static inline const char *GetStaticName() {
		return "error";
	}
	const char *name;
	ADDITIONAL_STATS_TYPE type;

	AdditionalStats() {
		this->name = GetStaticName();
		type = ADDITIONAL_STATS_TYPE::ERROR;
	}

	static inline void SerialiseStats(Serializer &serializer, AdditionalStats<T> *astats) {
		serializer.WriteProperty(1000, "additional_stats_type", (uint8_t)((AdditionalStats<uint32_t> *)astats)->type);
		astats->Serialise(astats, serializer);
	}
	static inline void DeserialiseStats(Deserializer &deserializer, AdditionalStats<T> *astats) {
		auto additional_stats_type =
		    (ADDITIONAL_STATS_TYPE)deserializer.template ReadProperty<uint8_t>(1000, "additional_stats_type");
		switch (additional_stats_type) {
		case ADDITIONAL_STATS_TYPE::ERROR:
			/* code */
			break;
		case ADDITIONAL_STATS_TYPE::EMPTY:
			/* code */
			break;
		case ADDITIONAL_STATS_TYPE::CLUSTER:
			/* code */
			break;
		case ADDITIONAL_STATS_TYPE::BLOOM:
			/* code */
			break;
		case ADDITIONAL_STATS_TYPE::BLOOM2:
			/* code */
			break;
		case ADDITIONAL_STATS_TYPE::DICTIONARY:
			/* code */
			break;
		case ADDITIONAL_STATS_TYPE::ALWAYS_PRUNE:
			/* code */
			break;
		}
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