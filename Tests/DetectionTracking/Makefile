# External executables

# Where is the project root folder?
ROOT_PREFIX := ../../

# Includes and libraries
LIBS := -leval -lgabor -lmotion_vector -ldet_summary -lutils -lalgorithms -lxmltd -lpr -llog -lnewdb -lutils -lcvblob -lsigar $(shell pkg-config --libs opencv) -lmysqlcppconn -lticppd -lxmlconfig -lboost_thread -lboost_system -lboost_filesystem -llemon -lsvm -lrt

# Define subprojects
SUB_PROJS :=

# Simple executables (i.e. having only the cpp file as a dependency)
EXE_TARGETS := process_video

# Library
LIB_SHORT_NAME :=

# Precompiled objects
PRECOMPILED_OBJECTS :=

# Target for skipping library and making only executables
#exe_make: tests

# Include main makefile
include $(ROOT_PREFIX)subproj_makefile.mk

# Create release executable
release: ${EXE_TARGETS}
	@echo RELEASE
	@mkdir -p release
	@for file in ${EXE_TARGETS} ; do echo CP $$file release/ ; cp $$file release/ ; done

# Clean masks and objects
clean:	clean-png

clean-dep: clean-png

clean-png:
	@$(RM) -f objects_*png
	@$(RM) -f masks_*png
