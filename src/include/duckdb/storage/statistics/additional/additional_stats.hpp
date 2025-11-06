//===----------------------------------------------------------------------===//
//                         DuckDB
//
// duckdb/storage/statistics/additional/additional_stats.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

namespace duckdb {

template <class T>
class AdditionalStat {
    // init
    virtual void Initialise(std::vector<T> data);
    // query
    virtual FilterPropagateResult Query();
    // size
    virtual size_t Size();
    // serialise
    virtual void Serialise();
    // deserialise
    virtual void Deserialise();
};

} // namespace duckdb