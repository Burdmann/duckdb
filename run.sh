cat template.csv > raw_data_min_max.csv
cat template.csv > raw_data_cluster.csv
cat template.csv > raw_data_bloom.csv
./build/release/duckdb_min_max -f sql_commands.txt 2>> raw_data_min_max.csv
./build/release/duckdb_cluster -f sql_commands.txt 2>> raw_data_cluster.csv
./build/release/duckdb_bloom -f sql_commands.txt 2>> raw_data_bloom.csv
> processed_data_2.csv
python3 data_processor.py raw_data_min_max.csv processed_data_2.csv
python3 data_processor.py raw_data_cluster.csv processed_data_2.csv
python3 data_processor.py raw_data_bloom.csv processed_data_2.csv