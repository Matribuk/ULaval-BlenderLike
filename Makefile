# --- Define OF_ROOT before including anything ---
ifndef OF_ROOT
	OF_ROOT=../../..
endif

# --- Define PROJECT_ROOT default ---
ifndef PROJECT_ROOT
	PROJECT_ROOT = .
endif

# --- Include path for headers ---
PROJECT_INCLUDE_PATHS += $(shell find $(PROJECT_ROOT)/include -type d)
USER_CFLAGS += $(addprefix -I, $(shell find $(PROJECT_ROOT)/include -type d))

USER_CFLAGS += -Wno-unused-function
USER_CFLAGS += -DOF_KEY_SPACE=32 -DOF_KEY_LEFT_SHIFT=340

# Attempt to load a config.make file (may override some vars)
ifneq ($(wildcard config.make),)
	include config.make
endif

# --- Finally include the openFrameworks project compiler ---
include $(OF_ROOT)/libs/openFrameworksCompiled/project/makefileCommon/compile.project.mk
