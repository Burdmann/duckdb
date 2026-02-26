create table tbl as select * from './xperiments_results/longs/processed/query_time.csv';
create table tbl2 as select Name,Unit,1 as Query,mean("Min/max") as "Min/max",mean("Min/max clusters") as "Min/max clusters",mean("Bloom filter") as "Bloom filter",mean("Dictionary") as "Dictionary" from tbl GROUP BY (Name,Unit);
copy tbl2 to './xperiments_results/longs/processed/query_time2.csv';