dir="xperiments_results/sorted_seconds"
# dir="."
mkdir -p $dir/processed
cat template.csv > $dir/raw_data_min_max.csv
cat template.csv > $dir/raw_data_cluster.csv
cat template.csv > $dir/raw_data_bloom.csv
cat template.csv > $dir/raw_data_dictionary.csv
rm test.db
./build/release/duckdb_min_max -f sql_queries_sorted_seconds.sql 2>> $dir/raw_data_min_max.csv > $dir/output_min_max.txt
rm test.db
./build/release/duckdb_cluster -f sql_queries_sorted_seconds.sql 2>> $dir/raw_data_cluster.csv > $dir/output_cluster.txt
rm test.db
./build/release/duckdb_bloom -f sql_queries_sorted_seconds.sql 2>> $dir/raw_data_bloom.csv > $dir/output_bloom.txt
rm test.db
./build/release/duckdb_dictionary -f sql_queries_sorted_seconds.sql 2>> $dir/raw_data_dictionary.csv > $dir/output_dictionary.txt
rm test.db
python3 data_processor.py 1 1000 0 $dir/raw_data_min_max.csv $dir/raw_data_cluster.csv $dir/raw_data_bloom.csv $dir/raw_data_dictionary.csv
mv pruning.csv $dir/processed/
mv query_time.csv $dir/processed/
mv size.csv $dir/processed/
mv ingestion.csv $dir/processed/