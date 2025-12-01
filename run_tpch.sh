dir="xperiments_results/array"
# dir="."
mkdir $dir
mkdir $dir/processed
# cat template.csv > $dir/raw_data_min_max.csv
# cat template.csv > $dir/raw_data_cluster.csv
# cat template.csv > $dir/raw_data_bloom.csv
# ./build/release/duckdb_min_max_hashmap -f sql_commands_tpch.txt 2>> $dir/raw_data_min_max.csv
# ./build/release/duckdb_cluster_hashmap -f sql_commands_tpch.txt 2>> $dir/raw_data_cluster.csv
# ./build/release/duckdb_bloom_hashmap -f sql_commands_tpch.txt 2>> $dir/raw_data_bloom.csv
python3 data_processor_tpch.py $dir/raw_data_min_max.csv $dir/raw_data_cluster.csv $dir/raw_data_bloom.csv
mv pruning_tpch.csv $dir/processed/
mv times_tpch.csv $dir/processed/
mv compile_times_tpch.csv $dir/processed/
mv running_times_tpch.csv $dir/processed/