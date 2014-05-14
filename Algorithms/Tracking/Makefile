# External executables

# Where is the project root folder?
ROOT_PREFIX := ../../
# Includes and libraries
LIBS := $(shell pkg-config --libs-only-l opencv) -lmysqlcppconn -lutils -llog -ldatabase

# Define subprojects
SUB_PROJS := CovarianceTracking DummyTracker

# Simple executables (i.e. having only the cpp file as a dependency)
EXE_TARGETS := 

# Library
LIB_SHORT_NAME := track

# Precompiled object
PRECOMPILED_OBJECTS := 

# Include main makefile
include $(ROOT_PREFIX)subproj_makefile.mk
