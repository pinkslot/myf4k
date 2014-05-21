DROP DATABASE IF EXISTS f4k_db;
CREATE DATABASE f4k_db;
use f4k_db;

CREATE TABLE roc (
dataset_name VARCHAR(63),
dataset_id INTEGER,
comments VARCHAR(63),
component_id INTEGER,
species_id INTEGER,
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id),
purpose VARCHAR(63) );

CREATE TABLE recog_roc_23b (
species_id INTEGER,
score DOUBLE,
FRR DOUBLE,
FAR DOUBLE );

CREATE TABLE event_types (
description VARCHAR(63),
event_type_id INTEGER );

CREATE TABLE ui_fish_occ_1h (
fish_id INTEGER,
stop_frame VARCHAR(63),
detection_certainty DOUBLE,
date_time DATETIME,
start_frame VARCHAR(63),
video_id VARCHAR(63),
component_id INTEGER,
camera_id INTEGER,
duration VARCHAR(63) );

CREATE TABLE connection_track (
ts DATETIME,
cmd VARCHAR(63),
connection_id INTEGER );

CREATE TABLE location (
country_en VARCHAR(63),
coordinates DOUBLE,
address_ch VARCHAR(63),
township_ch VARCHAR(63),
township_en VARCHAR(63),
latitude DOUBLE,
sea_depth DOUBLE,
location_id INTEGER,
city_ch VARCHAR(63),
longitude DOUBLE,
address_en VARCHAR(63),
country_ch VARCHAR(63),
city_en VARCHAR(63) );

CREATE TABLE event_secondary_object_detection (
event_id INTEGER,
so_detection_id INTEGER );

CREATE TABLE rocbin (
FRR DOUBLE,
ROC_id INTEGER,
score DOUBLE,
FAR DOUBLE );

CREATE TABLE performance_test (
progress INTEGER,
frames_with_errors INTEGER,
start_time DATETIME,
last_update DATETIME,
insert_time DATETIME,
machine_id INTEGER,
status VARCHAR(63),
end_time DATETIME,
job_id INTEGER,
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id),
video_id VARCHAR(63),
component_id INTEGER,
previous_processing INTEGER );

CREATE TABLE eval_result (
gt_dt_version INTEGER,
fish_id INTEGER,
gt_rec_version INTEGER,
is_a_fish BOOLEAN,
is_in_summary BOOLEAN,
gt_dt_certainty DOUBLE,
species_id INTEGER,
gt_rec_certainty DOUBLE );

CREATE TABLE test (
last_update DATETIME,
start_time DATETIME,
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id) );

CREATE TABLE tb_fish_species (
rec_certainty DOUBLE,
num INTEGER,
specie_id INTEGER,
fish_id INTEGER );

CREATE TABLE query_management (
last_update DATETIME,
data_percentage_error INTEGER,
query_percentage_successful INTEGER,
time_to_completion INTEGER,
data_end_date DATE,
data_end_time TIME,
query_start_process_time_by_wf DATETIME,
component_id_detection INTEGER,
query_percentage_completed INTEGER,
query_percentage_abandoned INTEGER,
data_start_date DATE,
video_location_videoNumber VARCHAR(63),
query_insert_time DATETIME,
data_start_time TIME,
query_id_by_wf INTEGER,
query_overall_status VARCHAR(63),
generated_by INTEGER,
query_percentage_pending INTEGER,
component_id_recognition INTEGER,
query_content VARCHAR(63) );

CREATE TABLE ui_fish_coocc (
fish_id INTEGER,
coocc_id INTEGER,
duration INTEGER );

CREATE TABLE species_corrected (
name VARCHAR(63),
species_id INTEGER );

CREATE TABLE reserved_video (
video_id VARCHAR(63),
reservation_time DATETIME,
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id) );

CREATE TABLE video_oi (
frame_width INTEGER,
length INTEGER,
date_time DATETIME,
video_file VARCHAR(63),
video_id VARCHAR(63),
camera_id INTEGER,
frame_rate INTEGER,
encoding VARCHAR(63),
frame_depth INTEGER,
frame_height INTEGER );

CREATE TABLE video_ground_truth (
date_time DATETIME,
frame_rate INTEGER,
frame_height INTEGER,
video_file VARCHAR(63),
video_id VARCHAR(63),
camera_id INTEGER,
length INTEGER,
frame_width INTEGER,
encoding VARCHAR(63),
frame_depth INTEGER );

CREATE TABLE maintain_log (
camera_id INTEGER,
maintain_finish_time DATE,
maintain_start_time DATE,
maintain_id INTEGER,
maintain_describe VARCHAR(63),
maintain_group VARCHAR(63) );

CREATE TABLE fish (
fish_id INTEGER,
detection_certainty DOUBLE,
tracking_certainty DOUBLE,
best_bb_cc BLOB,
best_video_id VARCHAR(63),
component_id INTEGER,
best_frame INTEGER );

CREATE TABLE hmm_fish_detection (
specie_id INTEGER,
video_id VARCHAR(63),
fish_id INTEGER,
component_id INTEGER,
bb_cc BLOB,
frame_id INTEGER,
detection_id INTEGER );

CREATE TABLE gt_object (
detection_id INTEGER,
timestamp DATETIME,
object_id INTEGER,
optional BOOLEAN,
ground_truth_id INTEGER,
type VARCHAR(63),
best_detection_id INTEGER,
frame_number INTEGER,
contour BLOB );

CREATE TABLE fish_detection (
timestamp DATETIME,
frame_id INTEGER,
fish_id INTEGER,
detection_id INTEGER,
detection_certainty DOUBLE,
tracking_certainty DOUBLE,
video_id VARCHAR(63),
bb_cc BLOB,
component_id INTEGER );

CREATE TABLE event (
event_certainty DOUBLE,
event_name VARCHAR(63),
start_frame INTEGER,
video_id VARCHAR(63),
event_type_id INTEGER,
event_id INTEGER,
end_frame INTEGER,
component_id INTEGER );

CREATE TABLE tmp (
species_id INTEGER,
fish_id INTEGER );

CREATE TABLE ui_fish_v2 (
detection_certainty DOUBLE,
component_id INTEGER,
date_time DATETIME,
video_id VARCHAR(63),
camera_id INTEGER,
tracking_certainty DOUBLE,
fish_id INTEGER );

CREATE TABLE cameras (
camera_angle INTEGER,
camera_lens VARCHAR(63),
video_number INTEGER,
depth INTEGER,
camera_id INTEGER,
location VARCHAR(63) );

CREATE TABLE gt_bad_detections (
video_id VARCHAR(63),
fish_id INTEGER,
timestamp DATETIME,
bb_cc BLOB,
component_id INTEGER,
frame_id INTEGER,
detection_certainty DOUBLE,
tracking_certainty DOUBLE,
detection_id INTEGER );

CREATE TABLE metadata (
notes VARCHAR(63),
allowed_values VARCHAR(63),
unit VARCHAR(63),
type VARCHAR(63),
missing_data_symbol VARCHAR(63),
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id),
name VARCHAR(63) );

CREATE TABLE ground_truth_det_tracking (
type VARCHAR(63),
date DATETIME,
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id),
best_ground_truth_id INTEGER,
video_id INTEGER,
quality_score DOUBLE,
agent_id INTEGER );

CREATE TABLE summary_camera_44 (
best_video_id VARCHAR(63),
rec_component_id INTEGER,
tracking_certainty DOUBLE,
det_certainty DOUBLE,
video_class INTEGER,
species_id INTEGER,
det_component_id INTEGER,
fish_id INTEGER,
date DATETIME,
rec_certainty DOUBLE );

CREATE TABLE event_fish_detection (
event_id INTEGER,
fd_detection_id INTEGER );

CREATE TABLE summary_camera_46 (
rec_component_id INTEGER,
fish_id INTEGER,
rec_certainty DOUBLE,
tracking_certainty DOUBLE,
date DATETIME,
det_component_id INTEGER,
best_video_id VARCHAR(63),
video_class INTEGER,
species_id INTEGER,
det_certainty DOUBLE );

CREATE TABLE summary_camera_40 (
det_component_id INTEGER,
det_certainty DOUBLE,
best_video_id VARCHAR(63),
fish_id INTEGER,
rec_component_id INTEGER,
tracking_certainty DOUBLE,
video_class INTEGER,
rec_certainty DOUBLE,
species_id INTEGER,
date DATETIME );

CREATE TABLE summary_camera_41 (
fish_id INTEGER,
best_video_id VARCHAR(63),
tracking_certainty DOUBLE,
species_id INTEGER,
det_component_id INTEGER,
video_class INTEGER,
date DATETIME,
rec_certainty DOUBLE,
rec_component_id INTEGER,
det_certainty DOUBLE );

CREATE TABLE summary_camera_42 (
det_component_id INTEGER,
fish_id INTEGER,
best_video_id VARCHAR(63),
det_certainty DOUBLE,
tracking_certainty DOUBLE,
rec_certainty DOUBLE,
rec_component_id INTEGER,
species_id INTEGER,
date DATETIME,
video_class INTEGER );

CREATE TABLE summary_camera_43 (
rec_component_id INTEGER,
det_component_id INTEGER,
det_certainty DOUBLE,
tracking_certainty DOUBLE,
date DATETIME,
rec_certainty DOUBLE,
video_class INTEGER,
species_id INTEGER,
best_video_id VARCHAR(63),
fish_id INTEGER );

CREATE TABLE fish_features (
data BLOB,
columns INTEGER,
feature_id INTEGER,
feature_certainty INTEGER,
rows INTEGER,
fish_id INTEGER );

CREATE TABLE video_class (
class_id INTEGER,
component_id INTEGER,
video_id VARCHAR(63),
certainty DOUBLE );

CREATE TABLE recog_roc_23b_cv (
FAR DOUBLE,
score DOUBLE,
species_id INTEGER,
FRR DOUBLE,
cross_validation INTEGER );

CREATE TABLE temporary_2 (
alg_name VARCHAR(63),
region VARCHAR(63),
gt_name VARCHAR(63),
max_fmeasure DOUBLE );

CREATE TABLE video_old (
frame_width INTEGER,
frame_height INTEGER,
video_id VARCHAR(63),
encoding VARCHAR(63),
video_file VARCHAR(63),
length INTEGER,
frame_rate INTEGER,
frame_depth INTEGER,
date_time DATETIME,
camera_id INTEGER );

CREATE TABLE software_component (
id_algorithm INTEGER,
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id, id_algorithm),
settings VARCHAR(63) );

CREATE TABLE trial (
rec_certainty DOUBLE,
fish_id INTEGER,
tr INTEGER,
specie_id INTEGER,
num INTEGER );

CREATE TABLE gt_evaluation_last (
pdr DOUBLE,
alg_name VARCHAR(63),
far DOUBLE,
pfp INTEGER,
size_threshold DOUBLE,
ground_truth_id INTEGER,
dr DOUBLE,
tp INTEGER,
pfn INTEGER,
pfar DOUBLE,
ptp INTEGER,
fn INTEGER,
video_id VARCHAR(63),
certainty_threshold DOUBLE,
fp INTEGER );

CREATE TABLE video_demos (
video_id VARCHAR(63),
path VARCHAR(63) );

CREATE TABLE recog_evaluation_23b (
score_species_26 DOUBLE,
score_species_27 DOUBLE,
score_species_32 DOUBLE,
score_species_17 DOUBLE,
score_species_1 DOUBLE,
label INTEGER,
score_species_10 DOUBLE,
score_species_38 DOUBLE,
score_species_19 DOUBLE,
score_species_4 DOUBLE,
score_species_7 DOUBLE,
score_species_11 DOUBLE,
score_species_8 DOUBLE,
score_species_21 DOUBLE,
score_species_24 DOUBLE,
score_species_33 DOUBLE,
score_species_34 DOUBLE,
score_species_2 DOUBLE,
score_species_3 DOUBLE,
score_species_9 DOUBLE,
score_species_37 DOUBLE,
prediction INTEGER,
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id),
score_species_5 DOUBLE,
file_name VARCHAR(63),
score_species_13 DOUBLE,
score_species_6 DOUBLE );

CREATE TABLE temporary_sc (
region VARCHAR(63),
optional VARCHAR(63),
alg_name VARCHAR(63),
max_fmeasure DOUBLE,
gt_name VARCHAR(63) );

CREATE TABLE performance_metric (
machine_id INTEGER,
calculate_range INTEGER,
average_time INTEGER,
max_time_video_id VARCHAR(63),
component_id INTEGER,
min_time_video_id VARCHAR(63),
min_time INTEGER,
average_waiting_time INTEGER,
max_time INTEGER,
num_videos_processed INTEGER,
last_update DATETIME );

CREATE TABLE features (
name VARCHAR(63),
type VARCHAR(63),
feature_id INTEGER );

CREATE TABLE summary_camera_37 (
det_certainty DOUBLE,
tracking_certainty DOUBLE,
best_video_id VARCHAR(63),
det_component_id INTEGER,
species_id INTEGER,
rec_component_id INTEGER,
date DATETIME,
rec_certainty DOUBLE,
fish_id INTEGER,
video_class INTEGER );

CREATE TABLE summary_camera_39 (
date DATETIME,
best_video_id VARCHAR(63),
det_component_id INTEGER,
species_id INTEGER,
rec_component_id INTEGER,
rec_certainty DOUBLE,
det_certainty DOUBLE,
fish_id INTEGER,
video_class INTEGER,
tracking_certainty DOUBLE );

CREATE TABLE summary_camera_38 (
species_id INTEGER,
best_video_id VARCHAR(63),
tracking_certainty DOUBLE,
rec_certainty DOUBLE,
det_component_id INTEGER,
rec_component_id INTEGER,
det_certainty DOUBLE,
video_class INTEGER,
date DATETIME,
fish_id INTEGER );

CREATE TABLE video_features (
rows INTEGER,
columns INTEGER,
video_id VARCHAR(63),
feature_id INTEGER,
data BLOB,
feature_certainty DOUBLE );

CREATE TABLE gt_keyframes (
start INTEGER,
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id),
ground_truth_id INTEGER,
end INTEGER );

CREATE TABLE ui_fish (
endframe INTEGER,
date_time DATETIME,
specie_id INTEGER,
camera_id INTEGER,
component_id INTEGER,
startframe INTEGER,
video_id VARCHAR(63),
detection_certainty DOUBLE,
best_frame INTEGER,
tracking_certainty DOUBLE,
fish_id INTEGER );

CREATE TABLE ui_fish_coocc_1h (
duration INTEGER,
coocc_id INTEGER,
fish_id INTEGER );

CREATE TABLE job_monitoring (
last_update DATETIME,
job_exe_conclusion VARCHAR(63),
job_exec_cmd VARCHAR(63),
job_queuing_start_time DATETIME,
time_to_completion INTEGER,
video_id VARCHAR(63),
video_timestamp DATETIME,
retry INTEGER,
query_id_by_wf VARCHAR(63),
data_output_condition VARCHAR(63),
scheduler_handling_status INTEGER,
all_done INTEGER,
job_exe_start_time DATETIME,
summary_errorcode VARCHAR(63),
job_exe_end_time DATETIME,
machine_group VARCHAR(63),
component_id INTEGER,
errorcode VARCHAR(63),
job_id INTEGER );

CREATE TABLE prova (
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id),
value INTEGER );

CREATE TABLE saved_videos (
video_id VARCHAR(63) );

CREATE TABLE temporary_op (
gt_name VARCHAR(63),
alg_name VARCHAR(63),
max_fmeasure DOUBLE,
optional VARCHAR(63),
region VARCHAR(63) );

CREATE TABLE support (
date VARCHAR(63),
camera_id INTEGER );

CREATE TABLE class (
class_id INTEGER,
certainty DOUBLE,
class_name VARCHAR(63) );

CREATE TABLE registration_registrationprofile (
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id),
activation_key VARCHAR(63) );

CREATE TABLE monitoring_parameter (
HJobPendingLimit INTEGER,
LJobPendingLimit INTEGER,
maxRunningTime INTEGER,
retryTimes INTEGER );

CREATE TABLE video_download_error (
video_id VARCHAR(63),
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id) );

CREATE TABLE environmental (
date_time DATETIME,
location VARCHAR(63),
wtemp DOUBLE,
pressure DOUBLE );

CREATE TABLE fish_species_cert (
fish_id INTEGER,
species_id INTEGER,
certainty DOUBLE,
component_id INTEGER,
occ INTEGER );

CREATE TABLE ground_truth (
agent_id INTEGER,
class VARCHAR(63),
video_id VARCHAR(63),
best_ground_truth_id INTEGER,
quality_score DOUBLE,
name VARCHAR(63),
type VARCHAR(63),
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id),
date DATETIME );

CREATE TABLE traj_species (
certainty DOUBLE,
specie_id INTEGER,
component_id INTEGER,
fish_id INTEGER );

CREATE TABLE secondary_objects (
object_id INTEGER,
component_id INTEGER,
best_frame INTEGER,
best_bb_cc BLOB,
stationary BOOLEAN,
type VARCHAR(63),
best_video_id VARCHAR(63) );

CREATE TABLE camera_info (
camera_id INTEGER,
camera_kind VARCHAR(63),
product_time DATE,
diaphragm DOUBLE,
product VARCHAR(63),
max_pixel INTEGER,
focal_length DOUBLE,
product_type VARCHAR(63),
zoom_multiple DOUBLE,
shoot_mode VARCHAR(63) );

CREATE TABLE results_open_sea (
avg DOUBLE,
name VARCHAR(63),
alg_name VARCHAR(63) );

CREATE TABLE video (
frame_width INTEGER,
date_time DATETIME,
length INTEGER,
camera_id INTEGER,
video_file VARCHAR(63),
frame_rate INTEGER,
encoding VARCHAR(63),
frame_depth INTEGER,
frame_height INTEGER,
video_id VARCHAR(63) );

CREATE TABLE temporary_mcr (
tracker VARCHAR(63),
region VARCHAR(63),
max_mcr DOUBLE,
gt_name VARCHAR(63),
optional VARCHAR(63),
alg_name VARCHAR(63) );

CREATE TABLE recog_evaluation_15b (
score_species_4 DOUBLE,
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id),
score_species_3 DOUBLE,
score_species_11 DOUBLE,
prediction INTEGER,
score_species_9 DOUBLE,
score_species_27 DOUBLE,
score_species_13 DOUBLE,
score_species_36 DOUBLE,
label INTEGER,
score_species_6 DOUBLE,
score_species_2 DOUBLE,
file_name VARCHAR(63),
score_species_8 DOUBLE,
score_species_5 DOUBLE,
score_species_10 DOUBLE,
score_species_1 DOUBLE,
score_species_7 DOUBLE,
score_species_26 DOUBLE );

CREATE TABLE recog_roc_15b (
score DOUBLE,
FAR DOUBLE,
FRR DOUBLE,
species_id INTEGER );

CREATE TABLE site (
wt_table_name VARCHAR(63),
site_alias VARCHAR(63),
event_id INTEGER,
site_id INTEGER,
camera_number VARCHAR(63),
estqblish_time DATE,
site_name VARCHAR(63),
location_id INTEGER,
contact VARCHAR(63),
cameraDescribe_id INTEGER );

CREATE TABLE recog_progress (
video0 INTEGER,
log_time DATETIME,
video4 INTEGER,
video5 INTEGER,
video7 INTEGER,
video6 INTEGER,
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id),
video2 INTEGER,
video1 INTEGER,
total_processed INTEGER,
video3 INTEGER );

CREATE TABLE detection_video_map (
detection_id INTEGER,
video_id VARCHAR(63) );

CREATE TABLE v_fish_species (
fish_id INTEGER,
num INTEGER,
rec_certainty DOUBLE,
specie_id INTEGER );

CREATE TABLE species (
species_id INTEGER,
name VARCHAR(63) );

CREATE TABLE secondary_object_detection (
timestamp DATETIME,
bb_cc BLOB,
frame_id INTEGER,
object_id INTEGER,
tracking_certainty DOUBLE,
detection_certainty DOUBLE,
detection_id INTEGER,
video_id VARCHAR(63) );

CREATE TABLE secondary_object_features (
object_id INTEGER,
columns INTEGER,
data BLOB,
feature_certainty INTEGER,
feature_id INTEGER,
rows INTEGER );

CREATE TABLE machine (
cache_size INTEGER,
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id),
ram INTEGER,
speed_cpu INTEGER,
number_cores INTEGER,
cpu_model VARCHAR(63) );

CREATE TABLE process_requests (
date_time DATETIME,
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id),
camera_id INTEGER,
updated_at DATETIME,
created_at DATETIME );

CREATE TABLE reserved_video_class (
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id),
reservation_time DATETIME,
video_id VARCHAR(63) );

CREATE TABLE temporary (
gt_name VARCHAR(63),
region VARCHAR(63),
optional VARCHAR(63),
max_fmeasure DOUBLE,
alg_name VARCHAR(63) );

CREATE TABLE temporary_cov (
alg_name VARCHAR(63),
max_fmeasure DOUBLE,
gt_name VARCHAR(63),
region VARCHAR(63) );

CREATE TABLE processed_videos (
component_id INTEGER,
start_time DATETIME,
previous_processing INTEGER,
progress INTEGER,
end_time DATETIME,
job_id INTEGER,
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id),
video_id VARCHAR(63),
last_update DATETIME,
status VARCHAR(63),
machine_id INTEGER,
frames_with_errors INTEGER,
insert_time DATETIME );

CREATE TABLE gt_evaluation (
tn DOUBLE,
fp DOUBLE,
pre DOUBLE,
component_id INTEGER,
fn DOUBLE,
certainty DOUBLE,
ground_truth_id INTEGER,
atm DOUBLE,
far DOUBLE,
ccr DOUBLE,
fmeasure DOUBLE,
cdr DOUBLE,
threshold DOUBLE,
dr DOUBLE,
tp DOUBLE,
optional VARCHAR(63),
rec DOUBLE );

CREATE TABLE fish_species (
specie_id INTEGER,
detection_id INTEGER,
certainty DOUBLE,
component_id INTEGER );

CREATE TABLE count_experiment_objects (
frame_number INTEGER,
type VARCHAR(63),
detection_id INTEGER,
optional BOOLEAN,
timestamp DATETIME,
count_experiment_id INTEGER,
best_detection_id INTEGER,
contour BLOB,
object_id INTEGER );

CREATE TABLE count_experiment_videos (
short_video_id VARCHAR(63),
video_id VARCHAR(63),
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id) );

CREATE TABLE algorithm (
ram INTEGER,
version VARCHAR(63),
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id),
execution_time DOUBLE,
description_execution_time VARCHAR(63),
name VARCHAR(63),
type VARCHAR(63),
input VARCHAR(63),
output VARCHAR(63),
description VARCHAR(63),
language VARCHAR(63) );

CREATE TABLE recog_roc_15b_cv (
cross_validation INTEGER,
FAR DOUBLE,
species_id INTEGER,
FRR DOUBLE,
score DOUBLE );

CREATE TABLE frame_class (
component_id INTEGER,
id INTEGER AUTO_INCREMENT,
PRIMARY KEY(id),
frame_id INTEGER,
class_id INTEGER,
video_id VARCHAR(63),
certainty DOUBLE );

CREATE TABLE ui_rule_param (
event_type_id INTEGER,
param VARCHAR(63) );

CREATE TABLE ui_fish_occ_v3 (
fish_id INTEGER,
detection_certainty DOUBLE,
start_frame VARCHAR(63),
date_time DATETIME,
component_id INTEGER,
duration VARCHAR(63),
stop_frame VARCHAR(63),
video_id VARCHAR(63),
camera_id INTEGER );

CREATE TABLE duplicate_videos (
encoding VARCHAR(63),
frame_rate INTEGER,
frame_height INTEGER,
frame_width INTEGER,
video_id VARCHAR(63),
frame_depth INTEGER,
date_time DATETIME,
video_file VARCHAR(63),
camera_id INTEGER,
length INTEGER );

CREATE TABLE gt_evaluation_region (
fmeasure DOUBLE,
fn DOUBLE,
optional VARCHAR(63),
far DOUBLE,
component_id INTEGER,
dr DOUBLE,
fp DOUBLE,
tp DOUBLE,
tn DOUBLE,
cdr DOUBLE,
atm DOUBLE,
threshold DOUBLE,
mcr DOUBLE,
region VARCHAR(63),
ground_truth_id INTEGER,
pre DOUBLE,
rec DOUBLE,
certainty DOUBLE );


insert into video ( `camera_id`, `date_time`, `encoding`, `frame_depth`, `frame_height`, `frame_rate`, `frame_width`, `length`, `video_file`, `video_id`) 
	values(0, 20140101, 'h-264', 256, 480, 8, 640, 30, '2.avi', '11');
