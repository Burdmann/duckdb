dir="."
mkdir $dir/output
./build/release/duckdb_min_max -f sql_commands_tpch_1.txt 2> /dev/null > $dir/output/output_min_max.txt
./build/release/duckdb_cluster -f sql_commands_tpch_1.txt 2> /dev/null > $dir/output/output_cluster.txt
./build/release/duckdb_bloom -f sql_commands_tpch_1.txt 2> /dev/null > $dir/output/output_bloom.txt