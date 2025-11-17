import duckdb
import sys

in_file = sys.argv[1]
out_file = sys.argv[2]

duckdb.execute(f"CREATE TABLE tbl AS SELECT * FROM '{in_file}';")

names = duckdb.execute("SELECT DISTINCT CAST(Data->'type' AS VARCHAR) FROM tbl WHERE type = 'END_INITIALISE_ADDITIONAL_STATS';").fetchall()
names = f"\"{";".join([tpl[0].replace('"','') for tpl in names])}\""

size = duckdb.execute("SELECT SUM(CAST(Data->'size' AS INTEGER)) FROM tbl WHERE type = 'END_INITIALISE_ADDITIONAL_STATS';").fetchone()[0]
# rowgroups_scanned = duckdb.execute("SELECT COUNT(DISTINCT CAST(Data->'partition' AS VARCHAR)) FROM tbl WHERE type = 'SCANNED_ROWS';").fetchone()[0]
rows_scanned = duckdb.execute("SELECT SUM(CAST(Data->'count' AS INTEGER)) FROM tbl WHERE type = 'SCANNED_ROWS';").fetchone()[0]
additional_stat_processing_time = duckdb.execute("SELECT ROUND(SUM(time-CAST(Data->'start_time' AS BIGINT))/1000000000,3) FROM tbl WHERE type = 'EVAL_ADDITIONAL_STATISTICS_END';").fetchone()[0]
stat_processing_time = duckdb.execute("SELECT ROUND(SUM(time-CAST(Data->'start_time' AS BIGINT))/1000000000,3) FROM tbl WHERE type = 'EVAL_STATISTICS_END';").fetchone()[0]
production_time = duckdb.execute("SELECT ROUND(SUM(time-CAST(Data->'start_time' AS BIGINT))/1000000000,3) FROM tbl WHERE type = 'END_INITIALISE_ADDITIONAL_STATS';").fetchone()[0]
ingestion_time = duckdb.execute("SELECT ROUND(SUM(time-CAST(Data->'start_time' AS BIGINT))/1000000000,3) FROM tbl WHERE type = 'APPEND_END';").fetchone()[0]

duckdb.execute("CREATE TABLE command_times AS SELECT CommandID AS id,MAX(time)-MIN(time) AS time_diff FROM tbl WHERE type = 'SQL_COMMAND_RUN_START' or type = 'SQL_COMMAND_RUN_END' GROUP BY CommandID;")
total_build_time = duckdb.execute("SELECT ROUND(time_diff/1000000000,3) FROM command_times WHERE id = 0;").fetchone()[0]
avg_total_query_time = duckdb.execute("SELECT ROUND(AVG(time_diff)/1000000000,3) FROM command_times WHERE id != 0;").fetchone()[0]



empty = True
try:
    with open(out_file,'r') as f:
        content = f.read()
        if len(content) > 0:
            empty = False
except:
    pass

with open(out_file,'a+') as f:
    if empty:
        f.write("name,size,rows_scanned,additional_stat_processing_time,stat_processing_time,production_time,ingestion_time,total_build_time,avg_query_time\n")
    f.write(f"{names},{size},{rows_scanned},{additional_stat_processing_time},{stat_processing_time},{production_time},{ingestion_time},{total_build_time},{avg_total_query_time}\n")