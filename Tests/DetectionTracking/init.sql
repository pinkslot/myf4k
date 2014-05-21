DROP DATABASE IF EXISTS f4k_db;
CREATE DATABASE f4k_db;
use f4k_db;

CREATE TABLE roc (
species_id INTEGER,
dataset_name VARCHAR(63),
id INTEGER,
PRIMARY KEY(id),
comments VARCHAR(63),
purpose VARCHAR(63),
dataset_id INTEGER,
component_id INTEGER );

CREATE TABLE recog_roc_23b (
score DOUBLE,
FAR DOUBLE,
species_id INTEGER,
FRR DOUBLE );

CREATE TABLE event_types (
description VARCHAR(63),
event_type_id INTEGER );

CREATE TABLE ui_fish_occ_1h (
start_frame VARCHAR(63),
duration VARCHAR(63),
fish_id INTEGER,
camera_id INTEGER,
detection_certainty DOUBLE,
component_id INTEGER,
date_time DATETIME,
stop_frame VARCHAR(63),
video_id VARCHAR(63) );

CREATE TABLE connection_track (
ts DATETIME,
connection_id INTEGER,
cmd VARCHAR(63) );

CREATE TABLE location (
latitude DOUBLE,
country_ch VARCHAR(63),
address_ch VARCHAR(63),
city_ch VARCHAR(63),
city_en VARCHAR(63),
location_id INTEGER,
address_en VARCHAR(63),
longitude DOUBLE,
country_en VARCHAR(63),
sea_depth DOUBLE,
coordinates DOUBLE,
township_ch VARCHAR(63),
township_en VARCHAR(63) );

CREATE TABLE event_secondary_object_detection (
event_id INTEGER,
so_detection_id INTEGER );

CREATE TABLE rocbin (
ROC_id INTEGER,
FAR DOUBLE,
FRR DOUBLE,
score DOUBLE );

CREATE TABLE performance_test (
progress INTEGER,
last_update DATETIME,
start_time DATETIME,
component_id INTEGER,
machine_id INTEGER,
end_time DATETIME,
insert_time DATETIME,
frames_with_errors INTEGER,
job_id INTEGER,
previous_processing INTEGER,
video_id VARCHAR(63),
id INTEGER,
PRIMARY KEY(id),
status VARCHAR(63) );

CREATE TABLE eval_result (
species_id INTEGER,
gt_rec_certainty DOUBLE,
is_in_summary BOOLEAN,
gt_rec_version INTEGER,
is_a_fish BOOLEAN,
fish_id INTEGER,
gt_dt_certainty DOUBLE,
gt_dt_version INTEGER );

CREATE TABLE test (
last_update DATETIME,
start_time DATETIME,
id INTEGER,
PRIMARY KEY(id) );

CREATE TABLE tb_fish_species (
num INTEGER,
rec_certainty DOUBLE,
fish_id INTEGER,
specie_id INTEGER );

CREATE TABLE query_management (
query_percentage_successful INTEGER,
query_percentage_abandoned INTEGER,
generated_by INTEGER,
query_id_by_wf INTEGER,
component_id_recognition INTEGER,
query_content VARCHAR(63),
data_start_time TIME,
time_to_completion INTEGER,
query_percentage_completed INTEGER,
query_percentage_pending INTEGER,
last_update DATETIME,
query_insert_time DATETIME,
query_overall_status VARCHAR(63),
video_location_videoNumber VARCHAR(63),
data_end_date DATE,
query_start_process_time_by_wf DATETIME,
component_id_detection INTEGER,
data_end_time TIME,
data_start_date DATE,
data_percentage_error INTEGER );

CREATE TABLE traj_species (
certainty DOUBLE,
specie_id INTEGER,
fish_id INTEGER,
component_id INTEGER );

CREATE TABLE species_corrected (
species_id INTEGER,
name VARCHAR(63) );

CREATE TABLE reserved_video (
video_id VARCHAR(63),
id INTEGER,
PRIMARY KEY(id),
reservation_time DATETIME );

CREATE TABLE video_oi (
date_time DATETIME,
frame_height INTEGER,
length INTEGER,
video_file VARCHAR(63),
encoding VARCHAR(63),
camera_id INTEGER,
video_id VARCHAR(63),
frame_depth INTEGER,
frame_rate INTEGER,
frame_width INTEGER );

CREATE TABLE video_ground_truth (
length INTEGER,
encoding VARCHAR(63),
frame_rate INTEGER,
frame_depth INTEGER,
frame_width INTEGER,
camera_id INTEGER,
video_id VARCHAR(63),
frame_height INTEGER,
date_time DATETIME,
video_file VARCHAR(63) );

CREATE TABLE maintain_log (
maintain_id INTEGER,
maintain_start_time DATE,
maintain_finish_time DATE,
camera_id INTEGER,
maintain_describe VARCHAR(63),
maintain_group VARCHAR(63) );

CREATE TABLE fish (
tracking_certainty DOUBLE,
best_video_id VARCHAR(63),
best_bb_cc BLOB,
component_id INTEGER,
best_frame INTEGER,
fish_id INTEGER,
detection_certainty DOUBLE );

CREATE TABLE hmm_fish_detection (
video_id VARCHAR(63),
bb_cc BLOB,
specie_id INTEGER,
detection_id INTEGER,
fish_id INTEGER,
component_id INTEGER,
frame_id INTEGER );

CREATE TABLE ui_fish (
video_id VARCHAR(63),
fish_id INTEGER,
detection_certainty DOUBLE,
camera_id INTEGER,
specie_id INTEGER,
startframe INTEGER,
component_id INTEGER,
endframe INTEGER,
date_time DATETIME,
tracking_certainty DOUBLE,
best_frame INTEGER );

CREATE TABLE fish_detection (
detection_certainty DOUBLE,
detection_id INTEGER,
video_id VARCHAR(63),
fish_id INTEGER,
component_id INTEGER,
bb_cc BLOB,
tracking_certainty DOUBLE,
timestamp DATETIME,
frame_id INTEGER );

CREATE TABLE event (
start_frame INTEGER,
event_id INTEGER,
event_type_id INTEGER,
event_certainty DOUBLE,
event_name VARCHAR(63),
video_id VARCHAR(63),
component_id INTEGER,
end_frame INTEGER );

CREATE TABLE tmp (
fish_id INTEGER,
species_id INTEGER );

CREATE TABLE ui_fish_v2 (
tracking_certainty DOUBLE,
video_id VARCHAR(63),
camera_id INTEGER,
component_id INTEGER,
date_time DATETIME,
fish_id INTEGER,
detection_certainty DOUBLE );

CREATE TABLE cameras (
location VARCHAR(63),
camera_id INTEGER,
camera_lens VARCHAR(63),
depth INTEGER,
camera_angle INTEGER,
video_number INTEGER );

CREATE TABLE gt_bad_detections (
video_id VARCHAR(63),
timestamp DATETIME,
detection_id INTEGER,
fish_id INTEGER,
component_id INTEGER,
tracking_certainty DOUBLE,
frame_id INTEGER,
bb_cc BLOB,
detection_certainty DOUBLE );

CREATE TABLE metadata (
notes VARCHAR(63),
type VARCHAR(63),
unit VARCHAR(63),
id INTEGER,
PRIMARY KEY(id),
missing_data_symbol VARCHAR(63),
allowed_values VARCHAR(63),
name VARCHAR(63) );

CREATE TABLE ground_truth_det_tracking (
type VARCHAR(63),
id INTEGER,
PRIMARY KEY(id),
video_id INTEGER,
best_ground_truth_id INTEGER,
quality_score DOUBLE,
date DATETIME,
agent_id INTEGER );

CREATE TABLE summary_camera_44 (
det_component_id INTEGER,
date DATETIME,
det_certainty DOUBLE,
best_video_id VARCHAR(63),
species_id INTEGER,
rec_certainty DOUBLE,
video_class INTEGER,
fish_id INTEGER,
tracking_certainty DOUBLE,
rec_component_id INTEGER );

CREATE TABLE event_fish_detection (
event_id INTEGER,
fd_detection_id INTEGER );

CREATE TABLE summary_camera_46 (
video_class INTEGER,
rec_certainty DOUBLE,
fish_id INTEGER,
date DATETIME,
best_video_id VARCHAR(63),
det_component_id INTEGER,
species_id INTEGER,
det_certainty DOUBLE,
rec_component_id INTEGER,
tracking_certainty DOUBLE );

CREATE TABLE summary_camera_40 (
fish_id INTEGER,
det_component_id INTEGER,
species_id INTEGER,
best_video_id VARCHAR(63),
rec_certainty DOUBLE,
video_class INTEGER,
rec_component_id INTEGER,
date DATETIME,
det_certainty DOUBLE,
tracking_certainty DOUBLE );

CREATE TABLE summary_camera_41 (
rec_component_id INTEGER,
species_id INTEGER,
det_certainty DOUBLE,
tracking_certainty DOUBLE,
det_component_id INTEGER,
rec_certainty DOUBLE,
fish_id INTEGER,
video_class INTEGER,
date DATETIME,
best_video_id VARCHAR(63) );

CREATE TABLE summary_camera_42 (
det_certainty DOUBLE,
rec_component_id INTEGER,
tracking_certainty DOUBLE,
best_video_id VARCHAR(63),
det_component_id INTEGER,
date DATETIME,
rec_certainty DOUBLE,
species_id INTEGER,
fish_id INTEGER,
video_class INTEGER );

CREATE TABLE summary_camera_43 (
det_component_id INTEGER,
det_certainty DOUBLE,
rec_certainty DOUBLE,
tracking_certainty DOUBLE,
date DATETIME,
species_id INTEGER,
rec_component_id INTEGER,
best_video_id VARCHAR(63),
video_class INTEGER,
fish_id INTEGER );

CREATE TABLE fish_features (
rows INTEGER,
fish_id INTEGER,
feature_id INTEGER,
columns INTEGER,
feature_certainty INTEGER,
data BLOB );

CREATE TABLE video_class (
certainty DOUBLE,
video_id VARCHAR(63),
class_id INTEGER,
component_id INTEGER );

CREATE TABLE video_demos (
video_id VARCHAR(63),
path VARCHAR(63) );

CREATE TABLE temporary_2 (
max_fmeasure DOUBLE,
gt_name VARCHAR(63),
region VARCHAR(63),
alg_name VARCHAR(63) );

CREATE TABLE video_old (
frame_depth INTEGER,
video_id VARCHAR(63),
video_file VARCHAR(63),
camera_id INTEGER,
frame_rate INTEGER,
encoding VARCHAR(63),
date_time DATETIME,
length INTEGER,
frame_height INTEGER,
frame_width INTEGER );

CREATE TABLE software_component (
id INTEGER,
PRIMARY KEY(id),
id_algorithm INTEGER,
settings VARCHAR(63) );

CREATE TABLE trial (
fish_id INTEGER,
tr INTEGER,
num INTEGER,
rec_certainty DOUBLE,
specie_id INTEGER );

CREATE TABLE gt_evaluation_last (
far DOUBLE,
pfn INTEGER,
alg_name VARCHAR(63),
pfp INTEGER,
fn INTEGER,
dr DOUBLE,
ptp INTEGER,
certainty_threshold DOUBLE,
pdr DOUBLE,
pfar DOUBLE,
ground_truth_id INTEGER,
video_id VARCHAR(63),
tp INTEGER,
size_threshold DOUBLE,
fp INTEGER );

CREATE TABLE recog_evaluation_23b (
prediction INTEGER,
score_species_38 DOUBLE,
score_species_34 DOUBLE,
score_species_4 DOUBLE,
score_species_6 DOUBLE,
score_species_9 DOUBLE,
score_species_1 DOUBLE,
file_name VARCHAR(63),
score_species_5 DOUBLE,
score_species_19 DOUBLE,
score_species_27 DOUBLE,
score_species_2 DOUBLE,
score_species_8 DOUBLE,
score_species_21 DOUBLE,
score_species_11 DOUBLE,
score_species_7 DOUBLE,
score_species_24 DOUBLE,
score_species_37 DOUBLE,
score_species_33 DOUBLE,
score_species_17 DOUBLE,
score_species_26 DOUBLE,
score_species_13 DOUBLE,
id INTEGER,
PRIMARY KEY(id),
label INTEGER,
score_species_10 DOUBLE,
score_species_32 DOUBLE,
score_species_3 DOUBLE );

CREATE TABLE temporary_sc (
max_fmeasure DOUBLE,
region VARCHAR(63),
optional VARCHAR(63),
alg_name VARCHAR(63),
gt_name VARCHAR(63) );

CREATE TABLE performance_metric (
last_update DATETIME,
max_time INTEGER,
machine_id INTEGER,
average_time INTEGER,
calculate_range INTEGER,
max_time_video_id VARCHAR(63),
average_waiting_time INTEGER,
min_time_video_id VARCHAR(63),
min_time INTEGER,
num_videos_processed INTEGER,
component_id INTEGER );

CREATE TABLE features (
feature_id INTEGER,
type VARCHAR(63),
name VARCHAR(63) );

CREATE TABLE summary_camera_37 (
rec_certainty DOUBLE,
tracking_certainty DOUBLE,
species_id INTEGER,
rec_component_id INTEGER,
fish_id INTEGER,
best_video_id VARCHAR(63),
video_class INTEGER,
det_component_id INTEGER,
det_certainty DOUBLE,
date DATETIME );

CREATE TABLE summary_camera_39 (
det_certainty DOUBLE,
rec_component_id INTEGER,
video_class INTEGER,
species_id INTEGER,
tracking_certainty DOUBLE,
rec_certainty DOUBLE,
fish_id INTEGER,
date DATETIME,
best_video_id VARCHAR(63),
det_component_id INTEGER );

CREATE TABLE summary_camera_38 (
rec_certainty DOUBLE,
date DATETIME,
rec_component_id INTEGER,
fish_id INTEGER,
det_component_id INTEGER,
tracking_certainty DOUBLE,
species_id INTEGER,
det_certainty DOUBLE,
best_video_id VARCHAR(63),
video_class INTEGER );

CREATE TABLE temporary_mcr (
tracker VARCHAR(63),
max_mcr DOUBLE,
optional VARCHAR(63),
region VARCHAR(63),
gt_name VARCHAR(63),
alg_name VARCHAR(63) );

CREATE TABLE video_features (
feature_id INTEGER,
data BLOB,
video_id VARCHAR(63),
feature_certainty DOUBLE,
rows INTEGER,
columns INTEGER );

CREATE TABLE gt_keyframes (
id INTEGER,
PRIMARY KEY(id),
ground_truth_id INTEGER,
end INTEGER,
start INTEGER );

CREATE TABLE gt_object (
object_id INTEGER,
contour BLOB,
ground_truth_id INTEGER,
best_detection_id INTEGER,
optional BOOLEAN,
timestamp DATETIME,
detection_id INTEGER,
type VARCHAR(63),
frame_number INTEGER );

CREATE TABLE job_monitoring (
errorcode VARCHAR(63),
machine_group VARCHAR(63),
scheduler_handling_status INTEGER,
job_exec_cmd VARCHAR(63),
time_to_completion INTEGER,
retry INTEGER,
job_exe_end_time DATETIME,
data_output_condition VARCHAR(63),
job_exe_conclusion VARCHAR(63),
query_id_by_wf VARCHAR(63),
last_update DATETIME,
job_exe_start_time DATETIME,
summary_errorcode VARCHAR(63),
component_id INTEGER,
video_timestamp DATETIME,
job_id INTEGER,
job_queuing_start_time DATETIME,
video_id VARCHAR(63),
all_done INTEGER );

CREATE TABLE prova (
id INTEGER,
PRIMARY KEY(id),
value INTEGER );

CREATE TABLE saved_videos (
video_id VARCHAR(63) );

CREATE TABLE temporary_op (
optional VARCHAR(63),
alg_name VARCHAR(63),
max_fmeasure DOUBLE,
gt_name VARCHAR(63),
region VARCHAR(63) );

CREATE TABLE support (
camera_id INTEGER,
date VARCHAR(63) );

CREATE TABLE ui_fish_coocc_1h (
fish_id INTEGER,
coocc_id INTEGER,
duration INTEGER );

CREATE TABLE registration_registrationprofile (
activation_key VARCHAR(63),
id INTEGER,
PRIMARY KEY(id) );

CREATE TABLE monitoring_parameter (
HJobPendingLimit INTEGER,
maxRunningTime INTEGER,
LJobPendingLimit INTEGER,
retryTimes INTEGER );

CREATE TABLE video_download_error (
id INTEGER,
PRIMARY KEY(id),
video_id VARCHAR(63) );

CREATE TABLE environmental (
pressure DOUBLE,
date_time DATETIME,
wtemp DOUBLE,
location VARCHAR(63) );

CREATE TABLE count_experiment_videos (
short_video_id VARCHAR(63),
video_id VARCHAR(63),
id INTEGER,
PRIMARY KEY(id) );

CREATE TABLE ground_truth (
type VARCHAR(63),
date DATETIME,
video_id VARCHAR(63),
id INTEGER,
PRIMARY KEY(id),
agent_id INTEGER,
name VARCHAR(63),
quality_score DOUBLE,
best_ground_truth_id INTEGER,
class VARCHAR(63) );

CREATE TABLE ui_fish_coocc (
fish_id INTEGER,
duration INTEGER,
coocc_id INTEGER );

CREATE TABLE secondary_objects (
component_id INTEGER,
object_id INTEGER,
best_frame INTEGER,
best_video_id VARCHAR(63),
best_bb_cc BLOB,
type VARCHAR(63),
stationary BOOLEAN );

CREATE TABLE fish_species (
component_id INTEGER,
detection_id INTEGER,
certainty DOUBLE,
specie_id INTEGER );

CREATE TABLE camera_info (
product_time DATE,
product VARCHAR(63),
product_type VARCHAR(63),
max_pixel INTEGER,
diaphragm DOUBLE,
camera_id INTEGER,
zoom_multiple DOUBLE,
camera_kind VARCHAR(63),
shoot_mode VARCHAR(63),
focal_length DOUBLE );

CREATE TABLE results_open_sea (
alg_name VARCHAR(63),
avg DOUBLE,
name VARCHAR(63) );

CREATE TABLE video (
video_file VARCHAR(63),
length INTEGER,
frame_rate INTEGER,
frame_height INTEGER,
encoding VARCHAR(63),
video_id VARCHAR(63),
camera_id INTEGER,
frame_width INTEGER,
date_time DATETIME,
frame_depth INTEGER );

CREATE TABLE recog_roc_23b_cv (
FAR DOUBLE,
score DOUBLE,
FRR DOUBLE,
cross_validation INTEGER,
species_id INTEGER );

CREATE TABLE recog_evaluation_15b (
score_species_3 DOUBLE,
score_species_2 DOUBLE,
file_name VARCHAR(63),
score_species_8 DOUBLE,
score_species_26 DOUBLE,
score_species_1 DOUBLE,
score_species_7 DOUBLE,
label INTEGER,
id INTEGER,
PRIMARY KEY(id),
score_species_10 DOUBLE,
score_species_13 DOUBLE,
score_species_9 DOUBLE,
score_species_5 DOUBLE,
score_species_27 DOUBLE,
score_species_36 DOUBLE,
score_species_6 DOUBLE,
prediction INTEGER,
score_species_11 DOUBLE,
score_species_4 DOUBLE );

CREATE TABLE recog_roc_15b (
FRR DOUBLE,
FAR DOUBLE,
score DOUBLE,
species_id INTEGER );

CREATE TABLE site (
site_name VARCHAR(63),
site_alias VARCHAR(63),
location_id INTEGER,
wt_table_name VARCHAR(63),
event_id INTEGER,
site_id INTEGER,
estqblish_time DATE,
camera_number VARCHAR(63),
cameraDescribe_id INTEGER,
contact VARCHAR(63) );

CREATE TABLE recog_progress (
video7 INTEGER,
video0 INTEGER,
total_processed INTEGER,
id INTEGER,
PRIMARY KEY(id),
video3 INTEGER,
video4 INTEGER,
video6 INTEGER,
video2 INTEGER,
video5 INTEGER,
log_time DATETIME,
video1 INTEGER );

CREATE TABLE detection_video_map (
video_id VARCHAR(63),
detection_id INTEGER );

CREATE TABLE v_fish_species (
num INTEGER,
rec_certainty DOUBLE,
fish_id INTEGER,
specie_id INTEGER );

CREATE TABLE species (
species_id INTEGER,
name VARCHAR(63) );

CREATE TABLE secondary_object_detection (
frame_id INTEGER,
bb_cc BLOB,
timestamp DATETIME,
tracking_certainty DOUBLE,
detection_id INTEGER,
video_id VARCHAR(63),
detection_certainty DOUBLE,
object_id INTEGER );

CREATE TABLE secondary_object_features (
object_id INTEGER,
feature_id INTEGER,
feature_certainty INTEGER,
rows INTEGER,
columns INTEGER,
data BLOB );

CREATE TABLE machine (
cache_size INTEGER,
speed_cpu INTEGER,
number_cores INTEGER,
id INTEGER,
PRIMARY KEY(id),
cpu_model VARCHAR(63),
ram INTEGER );

CREATE TABLE process_requests (
created_at DATETIME,
date_time DATETIME,
camera_id INTEGER,
id INTEGER,
PRIMARY KEY(id),
updated_at DATETIME );

CREATE TABLE reserved_video_class (
reservation_time DATETIME,
id INTEGER,
PRIMARY KEY(id),
video_id VARCHAR(63) );

CREATE TABLE temporary (
optional VARCHAR(63),
max_fmeasure DOUBLE,
gt_name VARCHAR(63),
region VARCHAR(63),
alg_name VARCHAR(63) );

CREATE TABLE temporary_cov (
region VARCHAR(63),
gt_name VARCHAR(63),
alg_name VARCHAR(63),
max_fmeasure DOUBLE );

CREATE TABLE processed_videos (
last_update DATETIME,
end_time DATETIME,
video_id VARCHAR(63),
status VARCHAR(63),
progress INTEGER,
frames_with_errors INTEGER,
insert_time DATETIME,
previous_processing INTEGER,
start_time DATETIME,
id INTEGER,
PRIMARY KEY(id),
machine_id INTEGER,
component_id INTEGER,
job_id INTEGER );

CREATE TABLE gt_evaluation (
cdr DOUBLE,
tn DOUBLE,
pre DOUBLE,
fmeasure DOUBLE,
fp DOUBLE,
fn DOUBLE,
rec DOUBLE,
tp DOUBLE,
certainty DOUBLE,
far DOUBLE,
optional VARCHAR(63),
component_id INTEGER,
threshold DOUBLE,
atm DOUBLE,
ccr DOUBLE,
ground_truth_id INTEGER,
dr DOUBLE );

CREATE TABLE class (
class_id INTEGER,
class_name VARCHAR(63),
certainty DOUBLE );

CREATE TABLE count_experiment_objects (
optional BOOLEAN,
timestamp DATETIME,
best_detection_id INTEGER,
object_id INTEGER,
detection_id INTEGER,
type VARCHAR(63),
count_experiment_id INTEGER,
contour BLOB,
frame_number INTEGER );

CREATE TABLE fish_species_cert (
component_id INTEGER,
certainty DOUBLE,
species_id INTEGER,
fish_id INTEGER,
occ INTEGER );

CREATE TABLE algorithm (
version VARCHAR(63),
name VARCHAR(63),
description VARCHAR(63),
input VARCHAR(63),
ram INTEGER,
type VARCHAR(63),
id INTEGER,
PRIMARY KEY(id),
execution_time DOUBLE,
output VARCHAR(63),
language VARCHAR(63),
description_execution_time VARCHAR(63) );

CREATE TABLE recog_roc_15b_cv (
cross_validation INTEGER,
species_id INTEGER,
FRR DOUBLE,
FAR DOUBLE,
score DOUBLE );

CREATE TABLE frame_class (
id INTEGER,
PRIMARY KEY(id),
frame_id INTEGER,
video_id VARCHAR(63),
certainty DOUBLE,
component_id INTEGER,
class_id INTEGER );

CREATE TABLE ui_rule_param (
param VARCHAR(63),
event_type_id INTEGER );

CREATE TABLE ui_fish_occ_v3 (
detection_certainty DOUBLE,
start_frame VARCHAR(63),
duration VARCHAR(63),
date_time DATETIME,
stop_frame VARCHAR(63),
camera_id INTEGER,
fish_id INTEGER,
component_id INTEGER,
video_id VARCHAR(63) );

CREATE TABLE duplicate_videos (
frame_rate INTEGER,
frame_depth INTEGER,
video_id VARCHAR(63),
frame_height INTEGER,
date_time DATETIME,
length INTEGER,
video_file VARCHAR(63),
frame_width INTEGER,
encoding VARCHAR(63),
camera_id INTEGER );

CREATE TABLE gt_evaluation_region (
region VARCHAR(63),
fn DOUBLE,
far DOUBLE,
rec DOUBLE,
optional VARCHAR(63),
fmeasure DOUBLE,
cdr DOUBLE,
certainty DOUBLE,
tp DOUBLE,
ground_truth_id INTEGER,
dr DOUBLE,
mcr DOUBLE,
atm DOUBLE,
fp DOUBLE,
pre DOUBLE,
component_id INTEGER,
threshold DOUBLE,
tn DOUBLE );

insert into video ( `camera_id`, `date_time`, `encoding`, `frame_depth`, `frame_height`, `frame_rate`, `frame_width`, `length`, `video_file`, `video_id`) 
	values(0, 20140101, 'h-264', 256, 480, 8, 640, 30, '2.avi', '11');
