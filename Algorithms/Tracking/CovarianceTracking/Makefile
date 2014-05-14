# External executables

# Where is the project root folder?
ROOT_PREFIX := ../../../
# Includes and libraries
LIBS := -lutils -lalgorithms -lxmltd -lpr -llog -lutils -lcvblob $(shell pkg-config --libs opencv) -lticppd -lboost_thread -lrt

# Define subprojects
SUB_PROJS :=

# Simple executables (i.e. having only the cpp file as a dependency)
EXE_TARGETS := test_tracker prova

# Library
LIB_SHORT_NAME := track_cov

# Precompiled objects
PRECOMPILED_OBJECTS :=

# Include main makefile
include $(ROOT_PREFIX)subproj_makefile.mk
