# External executables

# Where is the project root folder?
ROOT_PREFIX := ../../../
# Includes and libraries
LIBS := -leval -lgabor -lmotion_vector -ldet_summary -lutils -lalgorithms -lxmltd -lpr  -lnewdb -lutils -lcvblob -lsigar $(shell pkg-config --libs-only-l opencv) -lmysqlcppconn -lticppd -lxmlconfig -lboost_thread -llog -lemon

# Define subprojects
SUB_PROJS := 

# Simple executables (i.e. having only the cpp file as a dependency)
EXE_TARGETS := bayes

# Library
LIB_SHORT_NAME := bayes

# Precompiled objects
PRECOMPILED_OBJECTS :=

# Include main makefile
include $(ROOT_PREFIX)subproj_makefile.mk
