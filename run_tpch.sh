cat template.csv > raw_data_min_max.csv
cat template.csv > raw_data_cluster.csv
cat template.csv > raw_data_bloom.csv
./build/release/duckdb_min_max_fixed -f sql_commands_tpch.txt 2>> raw_data_min_max.csv
./build/release/duckdb_cluster_fixed -f sql_commands_tpch.txt 2>> raw_data_cluster.csv
./build/release/duckdb_bloom_fixed -f sql_commands_tpch.txt 2>> raw_data_bloom.csv
python3 data_processor_tpch.py raw_data_min_max.csv raw_data_cluster.csv raw_data_bloom.csv