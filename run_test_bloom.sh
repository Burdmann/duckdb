dir="xperiments_results/test_bloom_sf100"
mkdir -p $dir/processed
cat template.csv > $dir/raw_data_bloom_200.csv
cat template.csv > $dir/raw_data_bloom_500.csv
cat template.csv > $dir/raw_data_bloom_1000.csv
cat template.csv > $dir/raw_data_bloom_2000.csv
cat template.csv > $dir/raw_data_bloom_5000.csv
cat template.csv > $dir/raw_data_bloom_10000.csv
rm test.db
./build/release/duckdb_bloom_200 -f sql_queries_test_3.sql 2>> $dir/raw_data_bloom_200.csv
rm test.db
./build/release/duckdb_bloom_500 -f sql_queries_test_3.sql 2>> $dir/raw_data_bloom_500.csv
rm test.db
./build/release/duckdb_bloom_1000 -f sql_queries_test_3.sql 2>> $dir/raw_data_bloom_1000.csv
rm test.db
./build/release/duckdb_bloom_2000 -f sql_queries_test_3.sql 2>> $dir/raw_data_bloom_2000.csv
rm test.db
./build/release/duckdb_bloom_5000 -f sql_queries_test_3.sql 2>> $dir/raw_data_bloom_5000.csv
rm test.db
./build/release/duckdb_bloom_10000 -f sql_queries_test_3.sql 2>> $dir/raw_data_bloom_10000.csv
rm test.db
python3 data_processor.py 1 98 50 $dir/raw_data_bloom_200.csv $dir/raw_data_bloom_500.csv $dir/raw_data_bloom_1000.csv $dir/raw_data_bloom_2000.csv $dir/raw_data_bloom_5000.csv $dir/raw_data_bloom_10000.csv
mv pruning.csv $dir/processed/
mv query_time.csv $dir/processed/
mv size.csv $dir/processed/
mv ingestion.csv $dir/processed/
python3 data_visualiser.py $dir/processed "Bloom filter comparison"