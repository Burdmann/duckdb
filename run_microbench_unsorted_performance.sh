dir="xperiments_results/unsorted_performance"
file="sql_queries_unsorted_performance.sql"
# dir="."
mkdir -p $dir/processed
# cat template.csv > $dir/raw_data_min_max.csv
# cat template.csv > $dir/raw_data_cluster.csv
# cat template.csv > $dir/raw_data_bloom.csv
# cat template.csv > $dir/raw_data_dictionary.csv
# cat template.csv > $dir/raw_data_cluster_1000.csv
# cat template.csv > $dir/raw_data_bloom_1000.csv
# cat template.csv > $dir/raw_data_dictionary_1000.csv
# rm test.db
# duckdb -f $file > $dir/output_correct.txt
# rm test.db
# ./build/release/duckdb_min_max_performance -f $file 2>> $dir/raw_data_min_max.csv > $dir/output_min_max.txt
# rm test.db
# ./build/release/duckdb_cluster_performance -f $file 2>> $dir/raw_data_cluster.csv > $dir/output_cluster.txt
# rm test.db
# ./build/release/duckdb_bloom_performance -f $file 2>> $dir/raw_data_bloom.csv > $dir/output_bloom.txt
# rm test.db
# ./build/release/duckdb_dictionary_performance -f $file 2>> $dir/raw_data_dictionary.csv > $dir/output_dictionary.txt
# rm test.db
# ./build/release/duckdb_cluster_1000_performance -f $file 2>> $dir/raw_data_cluster_1000.csv > $dir/output_cluster_1000.txt
# rm test.db
# ./build/release/duckdb_bloom_1000_performance -f $file 2>> $dir/raw_data_bloom_1000.csv > $dir/output_bloom_1000.txt
# rm test.db
# ./build/release/duckdb_dictionary_1000_performance -f $file 2>> $dir/raw_data_dictionary_1000.csv > $dir/output_dictionary_1000.txt
rm test.db
python3 data_processor_performance.py 1 1000 1 $dir/raw_data_min_max.csv $dir/raw_data_cluster.csv $dir/raw_data_bloom.csv $dir/raw_data_dictionary.csv $dir/raw_data_cluster_1000.csv $dir/raw_data_bloom_1000.csv $dir/raw_data_dictionary_1000.csv
rm test.db
mv query_time.csv $dir/processed/