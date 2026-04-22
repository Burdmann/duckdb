dir="optimal"
mkdir $dir
rm test.db
cat template.csv > $dir/raw_data_sorted.csv
./build/release/duckdb_min_max -f sql_queries_sorted.sql 2>> $dir/raw_data_sorted.csv > /dev/null
rm test.db
cat template.csv > $dir/raw_data_unsorted.csv
./build/release/duckdb_min_max -f sql_queries_unsorted.sql 2>> $dir/raw_data_unsorted.csv > /dev/null
rm test.db
cat template.csv > $dir/raw_data_outliers.csv
./build/release/duckdb_min_max -f sql_queries_outliers.sql 2>> $dir/raw_data_outliers.csv > /dev/null
rm test.db
cat template.csv > $dir/raw_data_cardinality_100.csv
./build/release/duckdb_min_max -f sql_queries_cardinality_100.sql 2>> $dir/raw_data_cardinality_100.csv > /dev/null
rm test.db
cat template.csv > $dir/raw_data_cardinality_1000.csv
./build/release/duckdb_min_max -f sql_queries_cardinality_1000.sql 2>> $dir/raw_data_cardinality_1000.csv > /dev/null
rm test.db
cat template.csv > $dir/raw_data_cardinality_10000.csv
./build/release/duckdb_min_max -f sql_queries_cardinality_10000.sql 2>> $dir/raw_data_cardinality_10000.csv > /dev/null
rm test.db
cat template.csv > $dir/raw_data_cardinality_100000.csv
./build/release/duckdb_min_max -f sql_queries_cardinality_100000.sql 2>> $dir/raw_data_cardinality_100000.csv > /dev/null
rm test.db