//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/storage/statistics/util.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once
#include "duckdb/common/types.hpp"
#include <ctime>

namespace duckdb {
    class Util {
        public:
            static uint64_t session_id;
            static uint64_t command_count;

            static inline char* GetTime() {
                std::time_t time = std::time(nullptr);
                return std::ctime(&time);
            } 
    };

} // namespace duckdb
