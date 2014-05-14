# External executables

# Where is the project root folder?
ROOT_PREFIX := ../../
# Includes and libraries
LIBS := $(shell pkg-config --libs-only-l opencv) -lutils -llog

# Define subprojects
SUB_PROJS := SVMDetectionEvaluation BayesDetectionEvaluation BayesTrackingEvaluation

# Simple executables (i.e. having only the cpp file as a dependency)
EXE_TARGETS := 

# Library
LIB_SHORT_NAME := eval

# Precompiled objects
PRECOMPILED_OBJECTS :=

# Include main makefile
include $(ROOT_PREFIX)subproj_makefile.mk
