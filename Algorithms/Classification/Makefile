# External executables

# Where is the project root folder?
ROOT_PREFIX := ../../
# Includes and libraries
LIBS := -leval -lgabor -lmotion_vector -ldet_summary -lutils -lalgorithms -lxmltd -lpr -llog -lnewdb -lutils -lcvblob -lsigar $(shell pkg-config --libs opencv) -lmysqlcppconn -lticppd -lxmlconfig -lboost_thread -lboost_system -lboost_filesystem -llemon -lsvm -lrt

# Define subprojects
SUB_PROJS :=

# Simple executables (i.e. having only the cpp file as a dependency)
EXE_TARGETS := prova

# Library
LIB_SHORT_NAME := classif

# Precompiled objects
PRECOMPILED_OBJECTS := 

# Include main makefile
include $(ROOT_PREFIX)subproj_makefile.mk
