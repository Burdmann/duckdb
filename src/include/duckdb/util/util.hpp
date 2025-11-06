//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/util/util.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once
#include "duckdb/common/types.hpp"
// #include <ctime>
#include <chrono>

namespace duckdb {
class Util {
public:
	static uint64_t session_id;
	static uint64_t command_count;

	static inline std::chrono::nanoseconds GetTime() {
		auto duration = std::chrono::high_resolution_clock::now().time_since_epoch();
		return std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
	}
};

} // namespace duckdb
