dir="xperiments_results/array"
mkdir $dir
cat template.csv > $dir/raw_data_min_max.csv
cat template.csv > $dir/raw_data_cluster.csv
cat template.csv > $dir/raw_data_bloom.csv
./build/release/duckdb_min_max -f sql_commands_tpch.txt 2>> $dir/raw_data_min_max.csv
./build/release/duckdb_cluster -f sql_commands_tpch.txt 2>> $dir/raw_data_cluster.csv
./build/release/duckdb_bloom -f sql_commands_tpch.txt 2>> $dir/raw_data_bloom.csv
python3 data_processor_tpch.py $dir/raw_data_min_max.csv $dir/raw_data_cluster.csv $dir/raw_data_bloom.csv