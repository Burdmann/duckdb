dir="."
mkdir $dir/output
cat template.csv > $dir/raw_data_min_max.csv
cat template.csv > $dir/raw_data_cluster.csv
cat template.csv > $dir/raw_data_bloom.csv
./build/release/duckdb_min_max_hashmap -f sql_commands_tpch_1.txt 2>> $dir/raw_data_min_max.csv > $dir/output/output_min_max.txt
./build/release/duckdb_cluster_hashmap -f sql_commands_tpch_1.txt 2>> $dir/raw_data_cluster.csv > $dir/output/output_cluster.txt
./build/release/duckdb_bloom_hashmap -f sql_commands_tpch_1.txt 2>> $dir/raw_data_bloom.csv > $dir/output/output_bloom.txt