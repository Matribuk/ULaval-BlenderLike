# --- Define OF_ROOT before including anything ---
ifndef OF_ROOT
	OF_ROOT=../../..
endif

# --- Include path for headers ---
PROJECT_INCLUDE_PATHS += $(abspath $(PROJECT_ROOT)/include)
USER_CFLAGS += -I$(abspath $(PROJECT_ROOT)/include)

# Attempt to load a config.make file (may override some vars)
ifneq ($(wildcard config.make),)
	include config.make
endif

# --- Finally include the openFrameworks project compiler ---
include $(OF_ROOT)/libs/openFrameworksCompiled/project/makefileCommon/compile.project.mk
