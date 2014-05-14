# External executables

# Where is the project root folder?
ROOT_PREFIX := ../
# Includes and libraries
LIBS := $(shell pkg-config --libs-only-l opencv) -lutils -llog -ldatabase

# Define subprojects
SUB_PROJS := DetectionTracking

# Simple executables (i.e. having only the cpp file as a dependency)
EXE_TARGETS := 

# Library
LIB_SHORT_NAME :=

# Precompiled objects
PRECOMPILED_OBJECTS :=

# Create rule for subprojects only
make_all: subprojects_make

clean_all: subprojects_clean

# Include main makefile
include $(ROOT_PREFIX)subproj_makefile.mk
