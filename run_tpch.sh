dir="xperiments_results/tpch"
# dir="."
mkdir -p $dir/processed
cat template.csv > $dir/raw_data_min_max.csv
cat template.csv > $dir/raw_data_cluster.csv
cat template.csv > $dir/raw_data_bloom.csv
./build/release/duckdb_min_max -f sql_queries_tpch.sql 2>> $dir/raw_data_min_max.csv
./build/release/duckdb_cluster -f sql_queries_tpch.sql 2>> $dir/raw_data_cluster.csv
./build/release/duckdb_bloom -f sql_queries_tpch.sql 2>> $dir/raw_data_bloom.csv
python3 data_processor.py 8 22 80 $dir/raw_data_min_max.csv $dir/raw_data_cluster.csv $dir/raw_data_bloom.csv
mv pruning.csv $dir/processed/
mv query_time.csv $dir/processed/
mv size.csv $dir/processed/
mv ingestion.csv $dir/processed/