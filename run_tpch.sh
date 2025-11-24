cat template.csv > raw_data_min_max.csv
cat template.csv > raw_data_cluster.csv
cat template.csv > raw_data_bloom.csv
./build/release/duckdb_min_max_no_additional_pruning_2 -f sql_commands_tpch.txt 2>> raw_data_min_max.csv
./build/release/duckdb_cluster_no_additional_pruning_2 -f sql_commands_tpch.txt 2>> raw_data_cluster.csv
./build/release/duckdb_bloom_no_additional_pruning_2 -f sql_commands_tpch.txt 2>> raw_data_bloom.csv