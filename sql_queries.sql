CREATE TABLE store(s_store_sk INTEGER, s_store_id VARCHAR, s_rec_start_date DATE, s_rec_end_date DATE, s_closed_date_sk INTEGER, s_store_name VARCHAR, s_number_employees INTEGER, s_floor_space INTEGER, s_hours VARCHAR, s_manager VARCHAR, s_market_id INTEGER, s_geography_class VARCHAR, s_market_desc VARCHAR, s_market_manager VARCHAR, s_division_id INTEGER, s_division_name VARCHAR, s_company_id INTEGER, s_company_name VARCHAR, s_street_number VARCHAR, s_street_name VARCHAR, s_street_type VARCHAR, s_suite_number VARCHAR, s_city VARCHAR, s_county VARCHAR, s_state VARCHAR, s_zip VARCHAR, s_country VARCHAR, s_gmt_offset DECIMAL(5,2), s_tax_percentage DECIMAL(5,2));;
CREATE TABLE time_dim(t_time_sk INTEGER, t_time_id VARCHAR, t_time INTEGER, t_hour INTEGER, t_minute INTEGER, t_second INTEGER, t_am_pm VARCHAR, t_shift VARCHAR, t_sub_shift VARCHAR, t_meal_time VARCHAR);;
CREATE TABLE household_demographics(hd_demo_sk INTEGER, hd_income_band_sk INTEGER, hd_buy_potential VARCHAR, hd_dep_count INTEGER, hd_vehicle_count INTEGER);;
COPY store FROM 'tpcds_medium/store.csv' (FORMAT 'csv', delimiter ',', header 1);
COPY time_dim FROM 'tpcds_medium/time_dim.csv' (FORMAT 'csv', delimiter ',', header 1);
COPY household_demographics FROM 'tpcds_medium/household_demographics.csv' (FORMAT 'csv', delimiter ',', header 1);
SELECT count(*) from store where s_store_name = 'ese';
SELECT count(*) from time_dim where t_hour = 20;
SELECT count(*) from household_demographics where hd_dep_count = 7;