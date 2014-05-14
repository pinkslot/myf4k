# Expected variables to be defined by including makefile
# - ROOT_PREFIX
# - LIBS
# - SUB_PROJS
# - EXE_TARGETS
# - LIB_SHORT_NAME
# - PRECOMPILED_OBJECTS

# Directories for -I and -L
IL_DIR := $(ROOT_PREFIX) \
	$(ROOT_PREFIX)/Utils \
	$(ROOT_PREFIX)/3rdParty \
		$(ROOT_PREFIX)/3rdParty/CvBlob \
		$(ROOT_PREFIX)/3rdParty/tinyxmlpp \
		$(ROOT_PREFIX)/3rdParty/sigar/ \
		$(ROOT_PREFIX)/3rdParty/segment/ \
		$(ROOT_PREFIX)/3rdParty/threadpool/ \
		$(ROOT_PREFIX)/3rdParty/lemon/ \
		$(ROOT_PREFIX)/3rdParty/libsvm/ \
	$(ROOT_PREFIX)NewDatabase \
	$(ROOT_PREFIX)/Log \
	$(ROOT_PREFIX)/Algorithms/ \
		$(ROOT_PREFIX)/Algorithms/Classification \
		$(ROOT_PREFIX)/Algorithms/PreProcessing \
		$(ROOT_PREFIX)/Algorithms/PostProcessing \
		$(ROOT_PREFIX)/Algorithms/BlobProcessing \
		$(ROOT_PREFIX)/Algorithms/ResultProcessing \
		$(ROOT_PREFIX)/Algorithms/Detection \
			$(ROOT_PREFIX)/Algorithms/Detection/GaussianMixtureModel \
			$(ROOT_PREFIX)/Algorithms/Detection/IntrinsicModel \
			$(ROOT_PREFIX)/Algorithms/Detection/WaveBack \
			$(ROOT_PREFIX)/Algorithms/Detection/Vibe2 \
			$(ROOT_PREFIX)/Algorithms/Detection/APMM2 \
			$(ROOT_PREFIX)/Algorithms/Detection/CodeBook \
			$(ROOT_PREFIX)/Algorithms/Detection/Adaboost \
			$(ROOT_PREFIX)/Algorithms/Detection/Xbe \
			$(ROOT_PREFIX)/Algorithms/Detection/XMLDetection \
			$(ROOT_PREFIX)/Algorithms/Detection/MRF \
			$(ROOT_PREFIX)/Algorithms/Detection/BGSLibrary \
		$(ROOT_PREFIX)/Algorithms/Tracking \
			$(ROOT_PREFIX)/Algorithms/Tracking/CovarianceTracking \
			$(ROOT_PREFIX)/Algorithms/Tracking/DummyTracker \
		$(ROOT_PREFIX)/Algorithms/Evaluation \
			$(ROOT_PREFIX)/Algorithms/Evaluation/BayesTrackingEvaluation \
			$(ROOT_PREFIX)/Algorithms/Evaluation/DetectionSelfEvaluation \
			$(ROOT_PREFIX)/Algorithms/Evaluation/BayesDetectionEvaluation \
			$(ROOT_PREFIX)/Algorithms/Evaluation/SVMDetectionEvaluation \
	$(ROOT_PREFIX)/XMLTDResults \
	$(ROOT_PREFIX)/XMLConfig \
	$(ROOT_PREFIX)/ProcessingResults \
	$(ROOT_PREFIX)/MotionVector \
	$(ROOT_PREFIX)/GaborFilter \
	$(ROOT_PREFIX)/DetectionSummary \
	$(ROOT_PREFIX)/Context

# Includes and libraries
-include $(ROOT_PREFIX)/external_libs.mk
INCLUDES := ${EXTRA_INCLUDES} $(addprefix -I,$(IL_DIR)) $(OPENCV_INCLUDES) $(MYSQL_CONNECTOR_INCLUDES) $(BOOST_INCLUDES)
LIB_INCLUDES := ${EXTRA_LIB_INCLUDES} -L. $(addprefix -L,$(IL_DIR)) $(OPENCV_LIB_INCLUDES) $(MYSQL_CONNECTOR_LIB_INCLUDES) $(BOOST_LIB_INCLUDES)

# Define subprojects targets
SUB_MAKE_TARGETS := $(patsubst %,%.sub_make,$(SUB_PROJS))
SUB_CLEAN_TARGETS := $(patsubst %,%.sub_clean,$(SUB_PROJS))
SUB_CLEAN_DEP_TARGETS := $(patsubst %,%.sub_clean_dep,$(SUB_PROJS))

# Subproject libraries
SUB_LIBS := $(patsubst %,%/*.a,$(SUB_PROJS))

# Phony rules
.PHONY: all clean clean-dep tests subprojects_make subprojects_clean subprojects_clean_dep

# Compiler flags
CXX := g++
WARNINGS := -Wall
GDB := -ggdb

# Define shell functions
SHELL := /bin/bash
ECHO := echo -e

# If required, append OpenCV libs
ifdef APPEND_OPENCV_LIBS
LIBS := $(LIBS) $(OPENCV_LIBS)
endif

# Build CFLAGS
CXXFLAGS := $(GDB) $(WARNINGS) $(INCLUDES) $(LIB_INCLUDES)

# Library
ifdef LIB_SHORT_NAME
LIB_TARGET := lib$(LIB_SHORT_NAME).a
LIB_TARGET_FLAG := -l$(LIB_SHORT_NAME)
ALL_TARGET := $(LIB_TARGET)
else
LIB_TARGET :=
LIB_TARGET_FLAG :=
ALL_TARGET := tests
endif

# Object files
CPP_TARGETS := $(shell find * -maxdepth 0 -type f | grep cpp$$)
ALL_OBJ_TARGETS := $(patsubst %.cpp,%.o,$(CPP_TARGETS))
EXE_OBJ_TARGETS := $(patsubst %,%.o,$(EXE_TARGETS))
OBJ_TARGETS := $(filter-out $(EXE_OBJ_TARGETS),$(ALL_OBJ_TARGETS))

# Dependency files
DEP_TARGETS := $(patsubst %.cpp,%.d,$(CPP_TARGETS))

# Rules

all: $(ALL_TARGET)

tests: $(EXE_TARGETS)

# Rule for library
$(LIB_TARGET):	subprojects_make $(OBJ_TARGETS)
	@rm -f $(LIB_TARGET)
	@echo -e "AR\t$(LIB_TARGET)"
	@ar Tcq $(LIB_TARGET) $(OBJ_TARGETS) $(SUB_LIBS) $(PRECOMPILED_OBJECTS)

# Rule for simple executables
%:	%.cpp $(LIB_TARGET)
	@$(ECHO) "CXXE\t$@"
	@$(CXX) $< $(LIB_TARGET_FLAG) $(CXXFLAGS) $(LIBS) -o $@
	@mkdir -p $(ROOT_PREFIX)/bin
	@cp $@ $(ROOT_PREFIX)/bin

# Include dependency files
-include $(DEP_TARGETS)

# Rules for dependency files
%.o:	%.cpp %.h
	@$(ECHO) "CXX\t$@"
	@$(CXX) $< $(CXXFLAGS) -MMD -MP -c

%.o:	%.cpp
	@$(ECHO) "CXX\t$@"
	@$(CXX) $< $(CXXFLAGS) -MMD -MP -c

# Rule for subprojects make
subprojects_make:       $(SUB_MAKE_TARGETS)

# Rule for subprojects clean
subprojects_clean:      $(SUB_CLEAN_TARGETS)

# Rule for subprojects clean-del
subprojects_clean_dep:      $(SUB_CLEAN_DEP_TARGETS)

# Rule for subproject make
%.sub_make:
	make -C $(patsubst %.sub_make,%,$@)

# Rule for subproject clean
%.sub_clean:
	make -C $(patsubst %.sub_clean,%,$@) clean

# Rule for subproject clean del
%.sub_clean_dep:
	make -C $(patsubst %.sub_clean_dep,%,$@) clean-dep

clean:	subprojects_clean
	@echo "CLEAN"
	@$(RM) -f $(EXE_TARGETS)
	@$(RM) -f $(OBJ_TARGETS)
	@$(RM) -f $(EXE_OBJ_TARGETS)
	@$(RM) -f $(LIB_TARGET)

clean-dep:	subprojects_clean_dep
	@echo "CLEAN-DEP"
	@$(RM) -f $(EXE_TARGETS)
	@$(RM) -f $(OBJ_TARGETS)
	@$(RM) -f $(EXE_OBJ_TARGETS)
	@$(RM) -f $(DEP_TARGETS)
	@$(RM) -f $(LIB_TARGET)
