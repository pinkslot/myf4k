# External executables

# Where is the project root folder?
ROOT_PREFIX := ../../../
# Includes and libraries
LIBS := $(shell pkg-config --libs-only-l opencv)

# Define subprojects
SUB_PROJS :=

# Simple executables (i.e. having only the cpp file as a dependency)
EXE_TARGETS := main prova

# Library
LIB_SHORT_NAME := detection_vibe2

# Precompiled objects
PRECOMPILED_OBJECTS := 
ifeq ($(shell uname -m), x86_64)
	PRECOMPILED_OBJECTS += vibe-background.obj-64
else
	PRECOMPILED_OBJECTS += vibe-background.obj-32
endif

# Include main makefile
include $(ROOT_PREFIX)subproj_makefile.mk
