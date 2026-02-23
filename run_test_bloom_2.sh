dir="xperiments_results/test_bloom_2_sf100"
mkdir -p $dir/processed
cat template.csv > $dir/raw_data_bloom_register_50.csv
cat template.csv > $dir/raw_data_bloom_register_100.csv
cat template.csv > $dir/raw_data_bloom_register_200.csv
cat template.csv > $dir/raw_data_bloom_register_400.csv
cat template.csv > $dir/raw_data_bloom_classic_50.csv
cat template.csv > $dir/raw_data_bloom_classic_100.csv
cat template.csv > $dir/raw_data_bloom_classic_200.csv
cat template.csv > $dir/raw_data_bloom_classic_400.csv
rm test.db
./build/release/duckdb_bloom_register_50 -f sql_queries_test_3.sql 2>> $dir/raw_data_bloom_register_50.csv
rm test.db
./build/release/duckdb_bloom_register_100 -f sql_queries_test_3.sql 2>> $dir/raw_data_bloom_register_100.csv
rm test.db
./build/release/duckdb_bloom_register_200 -f sql_queries_test_3.sql 2>> $dir/raw_data_bloom_register_200.csv
rm test.db
./build/release/duckdb_bloom_register_400 -f sql_queries_test_3.sql 2>> $dir/raw_data_bloom_register_400.csv
rm test.db
./build/release/duckdb_bloom_classic_50 -f sql_queries_test_3.sql 2>> $dir/raw_data_bloom_classic_50.csv
rm test.db
./build/release/duckdb_bloom_classic_100 -f sql_queries_test_3.sql 2>> $dir/raw_data_bloom_classic_100.csv
rm test.db
./build/release/duckdb_bloom_classic_200 -f sql_queries_test_3.sql 2>> $dir/raw_data_bloom_classic_200.csv
rm test.db
./build/release/duckdb_bloom_classic_400 -f sql_queries_test_3.sql 2>> $dir/raw_data_bloom_classic_400.csv
rm test.db
python3 data_processor.py 1 98 50 $dir/raw_data_bloom_register_50.csv $dir/raw_data_bloom_register_100.csv $dir/raw_data_bloom_register_200.csv $dir/raw_data_bloom_register_400.csv $dir/raw_data_bloom_classic_50.csv $dir/raw_data_bloom_classic_100.csv $dir/raw_data_bloom_classic_200.csv $dir/raw_data_bloom_classic_400.csv
rm test.db
mv pruning.csv $dir/processed/
mv query_time.csv $dir/processed/
mv size.csv $dir/processed/
mv ingestion.csv $dir/processed/
python3 data_visualiser.py $dir/processed "Bloom filter comparison"