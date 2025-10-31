This is a fork of the DuckDB repository.
The goal of this prject is to modify DuckDB to use statistics other than regulat min/max
and investigate the advantages and drawbacks presented by different statistics in real-world scenarios

# Development journey

The first step was to find places in the DuckDB code relating to the use of min/max statistics.
The files [row_group.cpp](src/storage/table/row_group.cpp) and [column_segment.cpp](src/storage/table/column_segment.cpp) were identified as being important, as they house the logic for row groups and segments (a partition within a row group), which must contain some of the min/max logic.
Upon further investigation, the files [base_statistic.cpp](src/storage/statistics/base_statistics.cpp) and especially [numeric_stats.cpp](src/storage/statistics/numeric_stats.cpp) were identified as being very important, as the CheckZoneMap function witin the latter contains most of the logic for using min/max statistics. It also contains functions Serialize and Deserialize wich pertain to writing and reading statistics, another important piece of functionality.

In order to add new statistics, one should create a new header file in [this folder](src/include/duckdb/storage/statistics/) and an implementatiopn file in [this folder](src/storage/statistics/). Furthermore, in the file [base_statistics.hpp](src/include/duckdb/storage/statistics/base_statistics.hpp). the enum StatisticsType should be updated to include the new statistic, and a struct from the new header file should be added as a friend struct. In the file [base_statistics.cpp](src/storage/statistics/base_statistics.cpp), there is also a function called GetStatsType, where the new statistics should be added in order to be used. Another important thing to remember, is that in the bottom of [base_statistics.hpp](src/include/duckdb/storage/statistics/base_statistics.hpp), instantiations of update functions should be created for the types interval_t and list_entry_t, as otherwise the system will not compile.

A change in the file [constant_filter.cpp](src/planner/filter/constant_filter.cpp) is also needed to make sure the correct CheckZoneMap function is called.

Another necessary addition is in the UpdateFilterStatistics function inside the file [propagate_filter.cpp](src/optimizer/statistics/operator/propagate_filter.cpp), where it checks whether the statistics contain the necessary data.

Finally, some statistics may need to be intialised with all the data for the corresponding partition at once. If this is the case, the logic for this should be added to [column_data.hpp](src/include/duckdb/storage/table/column_data.hpp). The ability to do this was achieved by the implementation of functions named "InitStats" and "FinishInitialiseStats" in various files throughout the code base.
If it suffices to update the new type of statistic with new values as they are appended (and therefore not all of the values at once), it ius enough to implement an update function on the statistics, and call it from the Append function in [fixed_size_uncompressed.cpp](src/storage/compression/fixed_size_uncompressed.cpp).