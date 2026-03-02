dir="correctness_test_tpch"
mkdir $dir
./build/release/duckdb_min_max -f sql_queries_tpch_correctness.sql 2> /dev/null > $dir/output_min_max.txt
./build/release/duckdb_cluster -f sql_queries_tpch_correctness.sql 2> /dev/null > $dir/output_cluster.txt
./build/release/duckdb_bloom -f sql_queries_tpch_correctness.sql 2> /dev/null > $dir/output_bloom.txt