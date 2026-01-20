import duckdb
import sys

NUM_TABLES = int(sys.argv[1])
NUM_QUERIES = int(sys.argv[2])
NUM_ITERATIONS = int(sys.argv[3])
in_files = sys.argv[4:]

NUM_STATS = len(in_files)
QUERIES_START = 2*NUM_TABLES+NUM_QUERIES+2

times = [[] for _ in range(NUM_STATS)]
rows_scanned = [[] for _ in range(NUM_STATS)]
sizes = [[] for _ in range(NUM_STATS)]
ingestion_times = [[] for _ in range(NUM_STATS)]
tables = [[] for _ in range(NUM_STATS)]

duckdb.execute("CREATE TABLE Result_time (Query INT, min_max_time FLOAT, cluster_time FLOAT, bloom_time FLOAT);")
duckdb.execute("CREATE TABLE Result_pruning (Query INT, min_max_rows_scanned FLOAT, cluster_rows_scanned FLOAT, bloom_rows_scanned FLOAT);")
duckdb.execute("CREATE TABLE Result_size (Table_name VARCHAR, min_max_size INT64, cluster_size INT64, bloom_size INT64);")
duckdb.execute("CREATE TABLE Result_ingestion (Table_name VARCHAR, min_max_ingestion_time FLOAT, cluster_ingestion_time FLOAT, bloom_ingestion_time FLOAT);")
for idx,in_file in enumerate(in_files):
    duckdb.execute(f"CREATE TABLE tbl AS SELECT * FROM read_csv('{in_file}',max_line_size=100000000);")
    duckdb.execute("CREATE TABLE times AS SELECT start_time,end_time,t1.CommandID AS CommandID FROM (SELECT Time as end_time,CommandID FROM tbl WHERE type = 'SQL_COMMAND_RUN_END') t1 JOIN (SELECT Time as start_time,CommandID FROM tbl WHERE type = 'SQL_COMMAND_RUN_START') t2 on t1.CommandID = t2.commandID;")

    for i in range(NUM_TABLES):
        sizes[idx].append(duckdb.execute(f"SELECT COALESCE(SUM(CAST(Data->'size' AS INTEGER)),0) FROM tbl WHERE type = 'END_INITIALISE_ADDITIONAL_STATS' AND CommandID = {NUM_TABLES+i};").fetchone()[0])
        ingestion_times[idx].append(duckdb.execute(f"SELECT ROUND((end_time-start_time)/1000000000.0,5) FROM times WHERE CommandID = {NUM_TABLES+i}").fetchone()[0])
        tables[idx].append(duckdb.execute(f"SELECT CAST(Data->'command' AS VARCHAR) FROM tbl WHERE type='SQL_COMMAND_RUN_START' AND CommandID = {NUM_TABLES+i}").fetchone()[0].split()[1])

    for i in range(NUM_QUERIES):
        times[idx].append(duckdb.execute(f"SELECT ROUND(MEDIAN(end_time-start_time)/1000000000.0,5) FROM times WHERE CommandID >= {QUERIES_START+NUM_ITERATIONS*i} AND CommandID < {QUERIES_START+NUM_ITERATIONS*(1+i)};").fetchone()[0])
        rows_scanned[idx].append(duckdb.execute(f"SELECT ROUND(SUM(CAST(Data->'count' AS INTEGER))/1000000.0,5) AS val FROM tbl WHERE type = 'SCANNED_ROWS' and CommandID = {2*NUM_TABLES+1+i};").fetchone()[0])

    duckdb.execute(f"DROP TABLE tbl;")
    duckdb.execute(f"DROP TABLE times;")

for i in range(NUM_QUERIES):
    duckdb.execute(f"INSERT INTO Result_time VALUES ({i+1},{",".join([str(time[i]) for time in times])});")
    duckdb.execute(f"INSERT INTO Result_pruning VALUES ({i+1},{",".join([str(scanned[i]) for scanned in rows_scanned])});")
for i in range(NUM_TABLES):
    if (len(set([tables[j][i] for j in range(NUM_STATS)]))) != 1:
        raise "tables are not in the same order in given input files"
    duckdb.execute(f"INSERT INTO Result_size VALUES ('{tables[0][i]}',{",".join([str(size[i]) for size in sizes])});")
    duckdb.execute(f"INSERT INTO Result_ingestion VALUES ('{tables[0][i]}',{",".join([str(time[i]) for time in ingestion_times])});")

duckdb.execute("COPY Result_time TO 'query_time.csv';")
duckdb.execute("COPY Result_pruning TO 'pruning.csv';")
duckdb.execute("COPY Result_size TO 'size.csv';")
duckdb.execute("COPY Result_ingestion TO 'ingestion.csv';")