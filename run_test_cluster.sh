dir="xperiments_results/test_cluster_sf100"
# mkdir -p $dir/processed
# cat template.csv > $dir/raw_data_cluster_100.csv
# cat template.csv > $dir/raw_data_cluster_200.csv
# cat template.csv > $dir/raw_data_cluster_500.csv
# cat template.csv > $dir/raw_data_cluster_1000.csv
# cat template.csv > $dir/raw_data_cluster_2000.csv
# cat template.csv > $dir/raw_data_cluster_5000.csv
# rm test.db
# ./build/release/duckdb_cluster_100 -f sql_queries_test_3.sql 2>> $dir/raw_data_cluster_100.csv
# rm test.db
# ./build/release/duckdb_cluster_200 -f sql_queries_test_3.sql 2>> $dir/raw_data_cluster_200.csv
# rm test.db
# ./build/release/duckdb_cluster_500 -f sql_queries_test_3.sql 2>> $dir/raw_data_cluster_500.csv
# rm test.db
# ./build/release/duckdb_cluster_1000 -f sql_queries_test_3.sql 2>> $dir/raw_data_cluster_1000.csv
# rm test.db
# ./build/release/duckdb_cluster_2000 -f sql_queries_test_3.sql 2>> $dir/raw_data_cluster_2000.csv
# rm test.db
# ./build/release/duckdb_cluster_5000 -f sql_queries_test_3.sql 2>> $dir/raw_data_cluster_5000.csv
# rm test.db
python3 data_processor.py 1 98 50 $dir/raw_data_cluster_100.csv $dir/raw_data_cluster_200.csv $dir/raw_data_cluster_500.csv $dir/raw_data_cluster_1000.csv $dir/raw_data_cluster_2000.csv $dir/raw_data_cluster_5000.csv
mv pruning.csv $dir/processed/
mv query_time.csv $dir/processed/
mv size.csv $dir/processed/
mv ingestion.csv $dir/processed/
python3 data_visualiser.py $dir/processed "Cluster comparison"