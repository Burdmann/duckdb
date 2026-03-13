dir=$1
file=$2
# dir="."
mkdir -p $dir/processed
cat template.csv > $dir/raw_data_cluster_10.csv
cat template.csv > $dir/raw_data_bloom_10.csv
cat template.csv > $dir/raw_data_dictionary_10.csv
rm test.db
./build/release/duckdb_cluster_10_performance -f $file 2>> $dir/raw_data_cluster_10.csv > $dir/output_cluster_10.txt
rm test.db
./build/release/duckdb_bloom_10_performance -f $file 2>> $dir/raw_data_bloom_10.csv > $dir/output_bloom_10.txt
rm test.db
./build/release/duckdb_dictionary_10_performance -f $file 2>> $dir/raw_data_dictionary_10.csv > $dir/output_dictionary_10.txt
rm test.db
python3 data_processor_performance.py 1 1000 1 $dir/raw_data_min_max.csv $dir/raw_data_cluster_10.csv $dir/raw_data_bloom_10.csv $dir/raw_data_dictionary_10.csv $dir/raw_data_cluster.csv $dir/raw_data_bloom.csv $dir/raw_data_dictionary.csv $dir/raw_data_cluster_1000.csv $dir/raw_data_bloom_1000.csv $dir/raw_data_dictionary_1000.csv
rm test.db
mv query_time.csv $dir/processed/