# External executables

# Where is the project root folder?
ROOT_PREFIX := ./
# Includes and libraries
LIBS := $(shell pkg-config --libs-only-l opencv) -lutils -llog

# Define subprojects
SUB_PROJS := 3rdParty Log XMLConfig NewDatabase ProcessingResults XMLTDResults Utils DetectionSummary MotionVector GaborFilter Algorithms Tests

# Simple executables (i.e. having only the cpp file as a dependency)
EXE_TARGETS := 

# Library
LIB_SHORT_NAME :=

# Precompiled objects
PRECOMPILED_OBJECTS :=

# Rule for subprojects
subproj_rule: subprojects_make

# Include main makefile
include $(ROOT_PREFIX)subproj_makefile.mk

# Target for adding scripts and XML files into bin/ directory
full-bin: subproj_rule
	@$(ECHO) "MKDIR\tbin"
	@mkdir -p $(ROOT_PREFIX)/bin
	@$(ECHO) "CP\tsw_configs_all"
	@rm -rf $(ROOT_PREFIX)/bin/sw_configs_all
	@cp -r $(ROOT_PREFIX)/Tests/DetectionTracking/sw_configs_all $(ROOT_PREFIX)/bin/
	@$(ECHO) "CP\tdb_config.xml"
	@cp $(ROOT_PREFIX)/Tests/DetectionTracking/db_config.xml $(ROOT_PREFIX)/bin/
	@$(ECHO) "CP\tscripts"
	@cp $(ROOT_PREFIX)/Tests/DetectionTracking/*.sh $(ROOT_PREFIX)/bin/
	@$(ECHO) "CP\tREADME.txt"
	@cp $(ROOT_PREFIX)/Tests/DetectionTracking/README.txt $(ROOT_PREFIX)/bin/
