dir="xperiments_results/longs"
# dir="."
mkdir -p $dir/processed
cat template.csv > $dir/raw_data_min_max.csv
cat template.csv > $dir/raw_data_cluster.csv
cat template.csv > $dir/raw_data_bloom.csv
cat template.csv > $dir/raw_data_dictionary.csv
rm test.db
./build/release/duckdb_min_max -f sql_queries_longs.sql 2>> $dir/raw_data_min_max.csv
rm test.db
./build/release/duckdb_cluster -f sql_queries_longs.sql 2>> $dir/raw_data_cluster.csv
rm test.db
./build/release/duckdb_bloom -f sql_queries_longs.sql 2>> $dir/raw_data_bloom.csv
rm test.db
./build/release/duckdb_dictionary -f sql_queries_longs.sql 2>> $dir/raw_data_dictionary.csv
rm test.db
python3 data_processor.py 1 100 100 $dir/raw_data_min_max.csv $dir/raw_data_cluster.csv $dir/raw_data_bloom.csv $dir/raw_data_dictionary.csv
mv pruning.csv $dir/processed/
mv query_time.csv $dir/processed/
mv size.csv $dir/processed/
mv ingestion.csv $dir/processed/