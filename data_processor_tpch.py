import duckdb
import sys

in_files = sys.argv[1:]

times = [[] for _ in range(3)]
compile_times = [[] for _ in range(3)]
running_times = [[] for _ in range(3)]
rows_scanned = [[] for _ in range(3)]

NUM_ITERATIONS = 80

duckdb.execute("CREATE TABLE Result_time (Query INT, min_max_time FLOAT, cluster_time FLOAT, bloom_time FLOAT);")
duckdb.execute("CREATE TABLE Result_compile_time (Query INT, min_max_time FLOAT, cluster_time FLOAT, bloom_time FLOAT);")
duckdb.execute("CREATE TABLE Result_running_time (Query INT, min_max_time FLOAT, cluster_time FLOAT, bloom_time FLOAT);")
duckdb.execute("CREATE TABLE Result_pruning (Query INT, min_max_rows_scanned FLOAT, cluster_rows_scanned FLOAT, bloom_rows_scanned FLOAT);")
for idx,in_file in enumerate(in_files):
    duckdb.execute(f"CREATE TABLE tbl AS SELECT * FROM '{in_file}';")
    duckdb.execute("CREATE TABLE first_scan AS SELECT min(Time) as first_scan_time,CommandID from tbl where Type = 'SCANNED_FIRST_ROW' group by CommandID; ")
    duckdb.execute("CREATE TABLE times AS SELECT start_time,first_scan_time,end_time,t1.CommandID FROM ((SELECT Time as end_time,CommandID FROM tbl WHERE type = 'SQL_COMMAND_RUN_END') t1 JOIN (SELECT Time as start_time,CommandID FROM tbl WHERE type = 'SQL_COMMAND_RUN_START') t2 on t1.CommandID = t2.commandID) join first_scan on t1.CommandID = first_scan.CommandID;")
    duckdb.execute("create table timings as select first_scan_time-start_time as compile_time,end_time-first_scan_time as running_time,CommandID from times;")

    for i in range(22):
        times[idx].append(duckdb.execute(f"SELECT ROUND(MEDIAN(end_time-start_time)/1000000000.0,3) FROM times WHERE CommandID >= {8+NUM_ITERATIONS*i} AND CommandID < {8+NUM_ITERATIONS*(1+i)};").fetchone()[0])
        compile_times[idx].append(duckdb.execute(f"SELECT ROUND(MEDIAN(compile_time)/1000000000.0,3) FROM timings WHERE CommandID >= {8+NUM_ITERATIONS*i} AND CommandID < {8+NUM_ITERATIONS*(1+i)};").fetchone()[0])
        running_times[idx].append(duckdb.execute(f"SELECT ROUND(MEDIAN(running_time)/1000000000.0,3) FROM timings WHERE CommandID >= {8+NUM_ITERATIONS*i} AND CommandID < {8+NUM_ITERATIONS*(1+i)};").fetchone()[0])
        rows_scanned[idx].append(duckdb.execute(f"SELECT ROUND(MEDIAN(CAST(Data->'count' AS INTEGER))/1000000.0,2) FROM tbl WHERE type = 'SCANNED_ROWS' and CommandID >= {8+NUM_ITERATIONS*i} AND CommandID < {8+NUM_ITERATIONS*(1+i)};").fetchone()[0])
        

    duckdb.execute(f"DROP TABLE tbl;")
    duckdb.execute(f"DROP TABLE times;")
    duckdb.execute(f"DROP TABLE first_scan;")
    duckdb.execute(f"DROP TABLE timings;")

for i in range(22):
    duckdb.execute(f"INSERT INTO Result_time VALUES ({i+1},{",".join([str(time[i]) for time in times])});")
    duckdb.execute(f"INSERT INTO Result_compile_time VALUES ({i+1},{",".join([str(time[i]) for time in compile_times])});")
    duckdb.execute(f"INSERT INTO Result_running_time VALUES ({i+1},{",".join([str(time[i]) for time in running_times])});")
    duckdb.execute(f"INSERT INTO Result_pruning VALUES ({i+1},{",".join([str(scanned[i]) for scanned in rows_scanned])});")

duckdb.execute("COPY Result_time TO 'times_tpch.csv';")
duckdb.execute("COPY Result_compile_time TO 'compile_times_tpch.csv';")
duckdb.execute("COPY Result_running_time TO 'running_times_tpch.csv';")
duckdb.execute("COPY Result_pruning TO 'pruning_tpch.csv';")