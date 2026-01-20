cat template.csv > raw_data_min_max.csv
cat template.csv > raw_data_cluster.csv
cat template.csv > raw_data_bloom.csv
./build/release/duckdb_min_max -f sql_commands_2.sql 2>> raw_data_min_max.csv
./build/release/duckdb_cluster -f sql_commands_2.sql 2>> raw_data_cluster.csv
./build/release/duckdb_bloom -f sql_commands_2.sql 2>> raw_data_bloom.csv
> $1
python3 data_processor.py raw_data_min_max.csv $1
python3 data_processor.py raw_data_cluster.csv $1
python3 data_processor.py raw_data_bloom.csv $1