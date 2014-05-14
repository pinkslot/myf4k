--- INTRODUCTION ---

This package contains the code used for fish detection, fish tracking and video classification.

--- FISH DETECTION AND TRACKING

Requirements: OpenCV >= 2.4, MySQL C++ connector, Boost >= 1.53

To compile the package and the executables, you have to:
- edit the external_libs.mk file and specify the GCC flags which point to the OpenCV, MySQL and Boost include and library directories;
- run "make" on the root directory.

The package containes the following subdirectories and corresponding modules:
- 3rdParty: third-party libraries
- Algorithms: all relevant computer vision algorithms
- Context: describes the current state of processing
- DetectionSummary: creates images of all detected fish
- GaborFilter: implementation of Gabor filters
- Log: logging class
- MotionVector: wrapper around OpenCV's motion vector functions
- NewDatabase: MySQL ORM
- ProcessingResults: structure for storing detection/tracking results
- Tests: executables
- Utils: utility modules
- XMLConfig: reads XML configuration files
- XMLTDResults: reads/writes detection/tracking results from/to XML

The main fish detection/tracking executable is Tests/DetectionTracking/process_video. "process_video -h" returns the list of command-line arguments, however here are the most important ones:

   -v <path>,  --video <path>

   -x <path>,  --xml <path>
     XML result output file path

   --process-to <int>
     Process until given frame.

   --process-from <int>
     Process from given frame.

   --debug-to <int>
     Show debug window until given frame.

   --debug-from <int>
     Show debug window starting from given frame.

   -d,  --debug  (accepted multiple times)
     Print/show debug information (-d: show interesting frames only; -dd:
     show all frames)

   --frame_preproc <contrast_stretch|histeq>  (accepted multiple times)
     Frame pre-processing algorithm:
     - contrast_stretch: underwater-specific contrast stretching algorithm
     - histogram equalization
   
   --detection <agmm|apmm2|cb|gmm|im|vibe2|wb|ada|xbe|am|eigen|ggmm|mean|pm
      |wren|zgmm|ml|xml>
     Detection algorithm short name:
     - agmm/gmm: Adaptive Gaussian mixture model
     - apmm2: Adaptive Poisson mixture model
     - cb: Code-book
     - im: Intrinsic model
     - vibe2: ViBe
     - ada: Adaboost
     - xbe: multi-color-space version of ViBe
     - am: Adaptive median
     - eigen: Eigen background
     - ggmm: Grimson's GMM
     - mean: Temporal mean
     - pm: Temporal median
     - wren: Wren's Gaussian average
     - zgmm: Zivkovic's GMM
     - ml: Multi-layer
     - xml: read from XML file
   
   --post_processing <mrf|erode_dilate|morph>  (accepted multiple times)
     Post-processing algorithm after background/foreground estimation:
     - mrf: Markov random fields
     - erode_dilate: apply erosion and dilation
     - morph: apply erosion and dilation (user-defined)
   
   --blob_filtering <blob_size_filter|blob_smoothing|blob_certainty_filter>
      (accepted multiple times)
     Blob filtering algorithm:
     - blob_size_filter: filter by size
     - blob_smoothing: smooth blobs
     - blob_certainty_filter: filter by detection certainty
   
   --tracking <cov|dummy>
     Tracking algorithm short name:
     - cov: Covariance tracker
     - dummy: no tracking
   
   --detection_evaluation <bayes_det_eval|svm_det_eval>
     Detection evaluation algorithm:
     - bayes_det_eval: Bayes-based evaluation
     - svm_det_eval: SVM-based evaluation
   
   --tracking_evaluation <bayes_track_eval>
     Tracking evaluation algorithm

   --result_processing <avg_certainty|certainty_filter|lifetime_filter
      |size_filter|abs_size_filter|avg_size_filter|border_filter> 
      (accepted multiple times)
     Result processing algorithm:
     - avg_certainty: filter by average certainty
     - certainty_filter: filter by certainty
     - lifetime_filter: filter by number of appearances
     - size_filter: filter by size
     - abs_size_filter: filter by size (number of pixels, not percentage of frame)
     - avg_size_filter: filter by average size
     - border_filter: remove objects touching the border

Running the executable may take from 10 minutes (320x240@8 fps) to few hours (640x480@24 fps with detection/tracking certainty evaluation).

--- VIDEO CLASSIFICATION ---

Requirements: Matlab, MySQL C++ connector, ffmpeg, mediainfo

Compile mysql.cpp using mex.
It is also necessary to modify classify.m (row 2) in order to add to the PATH environment variable the ffmpeg/bin and mediainfo/bin directories.

To run the classifier, execute in Matlab the classify.m script, with the following arguments:
- training: path to classifier training data, use training_results/training_svm_png_50.mat
- classes_map: path class data, use classes_map.mat
- classification_lib: path to auxiliary script, use classification_lib.sh
- video_file_format: format string for input video file, e.g. path_to_video_dir/video_%s.flv (%s is the placeholder for the video_id)
- varargin: the list of video id separated by space.

The executable writes the result to a MySQL database (whose access configuration is specified in classify.m). The "video_class" (where the video class is stored) and "frame_class" (where the classification output for each single frame is stored) must exist, and can be created with the following MySQL queries:

CREATE TABLE `video_class` (`video_id` varchar(45) NOT NULL, `class_id` int(11) NOT NULL, `component_id` int(11) NOT NULL, `certainty` float DEFAULT NULL, PRIMARY KEY (`class_id`,`video_id`));
CREATE TABLE `frame_class` (`id` int(11) NOT NULL AUTO_INCREMENT, `video_id` varchar(45) NOT NULL, `frame_id` int(11) NOT NULL, `class_id` int(11) NOT NULL, `component_id` int(11) NOT NULL, `certainty` float DEFAULT NULL, PRIMARY KEY (`id`), UNIQUE KEY `video_id` (`video_id`,`frame_id`,`class_id`,`component_id`));

Running the executable on a 2.0 GHz 2-core CPU, 4 GB RAM takes about 7-10 seconds per video (without writing to the database, which depends on the database load).
