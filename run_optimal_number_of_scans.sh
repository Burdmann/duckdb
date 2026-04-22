dir="optimal"
mkdir $dir
rm test.db
cat template.csv > $dir/raw_data_sorted_range_seconds.csv
./build/release/duckdb_min_max_range -f sql_queries_sorted_range_seconds.sql 2>> $dir/raw_data_sorted_range_seconds.csv > /dev/null
rm test.db
cat template.csv > $dir/raw_data_sorted_range_minutes.csv
./build/release/duckdb_min_max_range -f sql_queries_sorted_range_minutes.sql 2>> $dir/raw_data_sorted_range_minutes.csv > /dev/null
rm test.db
cat template.csv > $dir/raw_data_sorted_range_hours.csv
./build/release/duckdb_min_max_range -f sql_queries_sorted_range_hours.sql 2>> $dir/raw_data_sorted_range_hours.csv > /dev/null
rm test.db
cat template.csv > $dir/raw_data_unsorted_range_seconds.csv
./build/release/duckdb_min_max_range -f sql_queries_unsorted_range_seconds.sql 2>> $dir/raw_data_unsorted_range_seconds.csv > /dev/null
rm test.db
cat template.csv > $dir/raw_data_unsorted_range_minutes.csv
./build/release/duckdb_min_max_range -f sql_queries_unsorted_range_minutes.sql 2>> $dir/raw_data_unsorted_range_minutes.csv > /dev/null
rm test.db
cat template.csv > $dir/raw_data_unsorted_range_hours.csv
./build/release/duckdb_min_max_range -f sql_queries_unsorted_range_hours.sql 2>> $dir/raw_data_unsorted_range_hours.csv > /dev/null
rm test.db
cat template.csv > $dir/raw_data_outliers_range_seconds.csv
./build/release/duckdb_min_max_range -f sql_queries_outliers_range_seconds.sql 2>> $dir/raw_data_outliers_range_seconds.csv > /dev/null
rm test.db
cat template.csv > $dir/raw_data_outliers_range_minutes.csv
./build/release/duckdb_min_max_range -f sql_queries_outliers_range_minutes.sql 2>> $dir/raw_data_outliers_range_minutes.csv > /dev/null
rm test.db
cat template.csv > $dir/raw_data_outliers_range_hours.csv
./build/release/duckdb_min_max_range -f sql_queries_outliers_range_hours.sql 2>> $dir/raw_data_outliers_range_hours.csv > /dev/null
rm test.db