bin_dir="build/release"
make clean
make STATS=EmptyAdditionalStats
mv $bin_dir/duckdb $bin_dir/duckdb_min_max
make STATS=ClusterAdditionalStats
mv $bin_dir/duckdb $bin_dir/duckdb_cluster
make STATS=BloomAdditionalStats
mv $bin_dir/duckdb $bin_dir/duckdb_bloom