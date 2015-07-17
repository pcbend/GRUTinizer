.PHONY: clean all
.SECONDARY:
.SECONDEXPANSION:

# EDIT THIS SECTION

INCLUDES   = include
CPP        = g++
CFLAGS     = -g -std=c++11 -O2 -Wall -Wextra -pedantic -Wno-unused-parameter
LINKFLAGS  = -L/opt/X11/lib -lX11 -lXpm
SRC_SUFFIX = cxx

# EVERYTHING PAST HERE SHOULD WORK AUTOMATICALLY

COM_COLOR=\033[0;34m
OBJ_COLOR=\033[0;36m
BLD_COLOR=\033[3;34m
LIB_COLOR=\033[3;36m
OK_COLOR=\033[0;32m
ERROR_COLOR=\033[0;31m
WARN_COLOR=\033[0;33m
NO_COLOR=\033[m

OK_STRING="[OK]"
ERROR_STRING="[ERROR]"
WARN_STRING="[WARNING]"
COM_STRING="Compiling"
BLD_STRING="Building\ "
FIN_STRING="Finished Building"

LIBRARY_DIRS   := $(shell find libraries -type d -links 2)
LIBRARY_NAMES  := $(notdir $(LIBRARY_DIRS))
LIBRARY_OUTPUT := $(patsubst %,libraries/lib%.so,$(LIBRARY_NAMES))

INCLUDES  := $(addprefix -I,$(INCLUDES))
CFLAGS    += $(shell root-config --cflags)
CFLAGS    += -MMD $(INCLUDES)
LINKFLAGS += -Llibraries $(addprefix -l,$(LIBRARY_NAMES)) -Wl,-rpath,\$$ORIGIN/../libraries
LINKFLAGS += $(shell root-config --glibs) -lSpectrum

UTIL_O_FILES    := $(patsubst %.$(SRC_SUFFIX),build/%.o,$(wildcard util/*.$(SRC_SUFFIX)))
SANDBOX_O_FILES := $(patsubst %.$(SRC_SUFFIX),build/%.o,$(wildcard sandbox/*.$(SRC_SUFFIX)))
MAIN_O_FILES    := $(patsubst %.$(SRC_SUFFIX),build/%.o,$(wildcard src/*.$(SRC_SUFFIX)))
EXE_O_FILES     := $(UTIL_O_FILES) $(SANDBOX_O_FILES)
EXECUTABLES     := $(patsubst %.o,bin/%,$(notdir $(EXE_O_FILES))) bin/grutinizer

run_and_test =@printf "%b" " $(3)$(4)$(5)$(2)$(NO_COLOR)\r";  \
                $(1) 2> $(2).log || touch $(2).error; \
                if test -e $(2).error; then \
                      printf "%-80b%b%s%b" "$(3)$(4)$(5)$(2)" "$(ERROR_COLOR)" "$(ERROR_STRING)" "$(NO_COLOR)\n"   ; \
                      cat $(2).log; \
                      rm -f $(2).log $(2).error; \
                      exit 1; \
                elif test -s $(2).log; then \
                      printf "%-80b%b%s%b" "$(3)$(4)$(5)$(2)" "$(WARN_COLOR)" "$(WARN_STRING)" "$(NO_COLOR)\n"   ; \
                      cat $(2).log; \
                else  \
                      printf "%-80b%b%s%b" "$(3)$(4)$(5)$(2)" "$(OK_COLOR)" "$(OK_STRING)" "$(NO_COLOR)\n"   ; \
                fi; \
                rm -f $(2).log $(2).error

all: $(EXECUTABLES) $(LIBRARY_OUTPUT)
	@printf "$(OK_COLOR)Compilation successful, $(WARN_COLOR)woohoo!$(NO_COLOR)\n"

bin/grutinizer: $(MAIN_O_FILES) | $(LIBRARY_OUTPUT) bin
	$(call run_and_test,$(CPP) $< -o $@ $(LINKFLAGS),$@,$(COM_COLOR),$(COM_STRING),$(OBJ_COLOR) )

bin/%: build/sandbox/%.o | $(LIBRARY_OUTPUT) bin
	$(call run_and_test,$(CPP) $< -o $@ $(LINKFLAGS),$@,$(COM_COLOR),$(COM_STRING),$(OBJ_COLOR) )

bin/%: build/util/%.o | $(LIBRARY_OUTPUT) bin
	$(call run_and_test,$(CPP) $< -o $@ $(LINKFLAGS),$@,$(COM_COLOR),$(COM_STRING),$(OBJ_COLOR) )



bin:
	@mkdir -p $@

# Functions for determining the files included in a library.
# All src files in the library directory are included.
# If a LinkDef.h file is present in the library directory,
#    a dictionary file will also be generated and added to the library.
libdir          = $(shell find libraries -name $(1) -type d)
lib_src_files   = $(shell find $(call libdir,$(1)) -name "*.$(SRC_SUFFIX)")
lib_o_files     = $(patsubst %.$(SRC_SUFFIX),build/%.o,$(call lib_src_files,$(1)))
lib_linkdef     = $(wildcard $(call libdir,$(1))/LinkDef.h)
lib_dictionary  = $(patsubst %/LinkDef.h,build/%/Dictionary.o,$(call lib_linkdef,$(1)))

libraries/lib%.so: $$(call lib_o_files,%) $$(call lib_dictionary,%)
	$(call run_and_test,$(CPP) -fPIC -shared -o $@ $^,$@,$(BLD_COLOR),$(BLD_STRING),$(OBJ_COLOR) )

build/%.o: %.$(SRC_SUFFIX)
	@mkdir -p $(dir $@)
	$(call run_and_test,$(CPP) -fPIC -c $< -o $@ $(CFLAGS),$@,$(COM_COLOR),$(COM_STRING),$(OBJ_COLOR) )

build/%/Dictionary.o: build/%/Dictionary.cc
	$(call run_and_test,$(CPP) -fPIC -c $< -o $@ $(CFLAGS),$@,$(COM_COLOR),$(COM_STRING),$(OBJ_COLOR) )


dict_header_files = $(subst //,,$(shell head $(1) -n 1))

build/%/Dictionary.cc: %/LinkDef.h
	@mkdir -p $(dir $@)
	$(call run_and_test,rootcint -f $@ -c $(INCLUDES) $(ROOTCFLAGS) -I$(dir $<) $(call dict_header_files,$<) $(notdir $<),$@,$(COM_COLOR),$(BLD_STRING),$(OBJ_COLOR))

-include $(shell find build -name '*.d' 2> /dev/null)

clean:
	@printf "\nCleaning up\n\n"
	@rm -rf build
	@rm -rf bin
	@rm -f $(LIBRARY_OUTPUT)
