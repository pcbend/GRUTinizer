.PHONY: clean all extras
.SECONDARY:
.SECONDEXPANSION:


PLATFORM:=$(PLATFORM)
# EDIT THIS SECTION

INCLUDES   = include
CFLAGS     = -g -std=c++11 -O3 -Wall -Wextra -pedantic -Wno-unused-parameter
LINKFLAGS_PREFIX  =
LINKFLAGS_SUFFIX  = -L/opt/X11/lib -lX11 -lXpm -std=c++11
SRC_SUFFIX = cxx

# EVERYTHING PAST HERE SHOULD WORK AUTOMATICALLY

USING_ROOT_6 = $(shell expr $(shell root-config --version | cut -f1 -d.) \>= 6)

ifeq ($(PLATFORM),Darwin)
export __APPLE__:= 1
CFLAGS     += -DOS_DARWIN -DHAVE_ZLIB
CFLAGS     += -I/opt/X11/include -Qunused-arguments
CPP        = clang++
SHAREDSWITCH = -install_name # ENDING SPACE
else
export __LINUX__:= 1
CPP        = g++
LINKFLAGS_PREFIX += -Wl,--no-as-needed
SHAREDSWITCH = -shared -Wl,-soname,# NO ENDING SPACE
endif

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
COM_STRING= "Compiling"
BLD_STRING= "Building\ "
COPY_STRING="Copying\ \ "
FIN_STRING="Finished Building"

LIBRARY_DIRS   := $(shell find libraries -type d -links 2 2> /dev/null)
LIBRARY_NAMES  := $(notdir $(LIBRARY_DIRS))
LIBRARY_OUTPUT := $(patsubst %,lib/lib%.so,$(LIBRARY_NAMES))

INCLUDES  := $(addprefix -I$(PWD)/,$(INCLUDES))
CFLAGS    += $(shell root-config --cflags)
CFLAGS    += -MMD -MP $(INCLUDES)
LINKFLAGS += -Llib $(addprefix -l,$(LIBRARY_NAMES)) -Wl,-rpath,\$$ORIGIN/../lib
LINKFLAGS += $(shell root-config --glibs) -lSpectrum -lPyROOT -lMinuit
LINKFLAGS := $(LINKFLAGS_PREFIX) $(LINKFLAGS) $(LINKFLAGS_SUFFIX) $(CFLAGS)

ROOT_LIBFLAGS := $(shell root-config --cflags)

UTIL_O_FILES    := $(patsubst %.$(SRC_SUFFIX),.build/%.o,$(wildcard util/*.$(SRC_SUFFIX)))
MAIN_O_FILES    := $(patsubst %.$(SRC_SUFFIX),.build/%.o,$(wildcard src/*.$(SRC_SUFFIX)))
EXE_O_FILES     := $(UTIL_O_FILES)
EXECUTABLES     := $(patsubst %.o,bin/%,$(notdir $(EXE_O_FILES))) bin/grutinizer

HISTOGRAM_SO    := $(patsubst histos/%.$(SRC_SUFFIX),lib/lib%.so,$(wildcard histos/*.$(SRC_SUFFIX)))
FILTER_SO    := $(patsubst filters/%.$(SRC_SUFFIX),lib/lib%.so,$(wildcard filters/*.$(SRC_SUFFIX)))

ifdef VERBOSE
run_and_test = @echo $(1) && $(1);
else
run_and_test =@printf "%b%b%b" " $(3)$(4)$(5)" $(notdir $(2)) "$(NO_COLOR)\r";  \
                $(1) 2> $(2).log || touch $(2).error; \
                if test -e $(2).error; then \
                      printf "%-60b%b%s%b" "$(3)$(4)$(5)$(2)" "$(ERROR_COLOR)" "$(ERROR_STRING)" "$(NO_COLOR)\n"   ; \
                      cat $(2).log; \
                      rm -f $(2).log $(2).error; \
                      exit 1; \
                elif test -s $(2).log; then \
                      printf "%-60b%b%s%b" "$(3)$(4)$(5)$(2)" "$(WARN_COLOR)" "$(WARN_STRING)" "$(NO_COLOR)\n"   ; \
                      cat $(2).log; \
                else  \
                      printf "%b%-60s%b%s%b" "$(3)$(4)$(5)" $(notdir $(2)) "$(OK_COLOR)" "$(OK_STRING)" "$(NO_COLOR)\n"   ; \
                fi; \
                rm -f $(2).log $(2).error
endif

all: include/GVersion.h $(EXECUTABLES) $(LIBRARY_OUTPUT) bin/grutinizer-config bin/gadd_fast.py\
     $(HISTOGRAM_SO) $(FILTER_SO) extras
	@printf "$(OK_COLOR)Compilation successful, $(WARN_COLOR)woohoo!$(NO_COLOR)\n"

docs:
	doxygen doxygen.config

bin/%: util/% | bin
	@ln -sf ../$< $@

bin/grutinizer: $(MAIN_O_FILES) | $(LIBRARY_OUTPUT) bin
	$(call run_and_test,$(CPP) $^ -o $@ $(LINKFLAGS),$@,$(COM_COLOR),$(COM_STRING),$(OBJ_COLOR) )

bin/%: .build/util/%.o | $(LIBRARY_OUTPUT) bin
	$(call run_and_test,$(CPP) $< -o $@ $(LINKFLAGS),$@,$(COM_COLOR),$(COM_STRING),$(OBJ_COLOR) )

bin lib:
	@mkdir -p $@

include/GVersion.h: .git/HEAD .git/index
	$(call run_and_test,util/gen_version.sh,$@,$(COM_COLOR),$(COM_STRING),$(OBJ_COLOR) )

lib/lib%.so: .build/histos/%.o | lib
	$(call run_and_test,$(CPP) -fPIC $^ $(SHAREDSWITCH)lib$*.so $(ROOT_LIBFLAGS) -o $@,$@,$(BLD_COLOR),$(BLD_STRING),$(OBJ_COLOR) )

lib/lib%.so: .build/filters/%.o | lib
	$(call run_and_test,$(CPP) -fPIC $^ $(SHAREDSWITCH)lib$*.so $(ROOT_LIBFLAGS) -o $@,$@,$(BLD_COLOR),$(BLD_STRING),$(OBJ_COLOR) )

# Functions for determining the files included in a library.
# All src files in the library directory are included.
# If a LinkDef.h file is present in the library directory,
#    a dictionary file will also be generated and added to the library.
libdir          = $(shell find libraries -name $(1) -type d)
lib_src_files   = $(shell find $(call libdir,$(1)) -name "*.$(SRC_SUFFIX)")
lib_o_files     = $(patsubst %.$(SRC_SUFFIX),.build/%.o,$(call lib_src_files,$(1)))
lib_linkdef     = $(wildcard $(call libdir,$(1))/LinkDef.h)
lib_dictionary  = $(patsubst %/LinkDef.h,.build/%/LibDictionary.o,$(call lib_linkdef,$(1)))
lib_pcm         = lib/$(dir $(notdir $(call lib_linkdef,$(1)))).pcm

lib/lib%.so: $$(call lib_o_files,%) $$(call lib_dictionary,%) | lib
	$(call run_and_test,$(CPP) -fPIC $^ $(SHAREDSWITCH)lib$*.so $(ROOT_LIBFLAGS) -o $@,$@,$(BLD_COLOR),$(BLD_STRING),$(OBJ_COLOR) )

.build/%.o: %.$(SRC_SUFFIX)
	@mkdir -p $(dir $@)
	$(call run_and_test,$(CPP) -fPIC -c $< -o $@ $(CFLAGS),$@,$(COM_COLOR),$(COM_STRING),$(OBJ_COLOR) )

dict_header_files = $(addprefix $(PWD)/include/,$(subst //,,$(shell head $(1) -n 1 2> /dev/null)))
find_linkdef = $(shell find $(1) -name "*LinkDef.h")

# In order for all function names to be unique, rootcint requires unique output names.
# Therefore, usual wildcard rules are insufficient.
# Eval is more powerful, but is less convenient to use.
define library_template
.build/$(1)/$(notdir $(1))Dict.cxx: $$(call dict_header_files,$(1)/LinkDef.h) $(1)/LinkDef.h
	@mkdir -p $$(dir $$@)
	$$(call run_and_test,rootcint -f $$@ -c $$(INCLUDES) -p $$^,$$@,$$(COM_COLOR),$$(BLD_STRING) ,$$(OBJ_COLOR))
ifeq ($(USING_ROOT_6),1)
	@mkdir -p lib && cp .build/$(1)/$(notdir $(1))Dict_rdict.pcm lib/$(notdir $(1))Dict_rdict.pcm
endif


.build/$(1)/LibDictionary.o: .build/$(1)/$(notdir $(1))Dict.cxx
	$$(call run_and_test,$$(CPP) -fPIC -c $$< -o $$@ $$(CFLAGS),$$@,$$(COM_COLOR),$$(COM_STRING),$$(OBJ_COLOR) )


lib/$(notdir $(1))Dict_rdict.pcm: .build/$(1)/$(notdir $(1))Dict_rdict.pcm | lib
	$$(call run_and_test,cp $$< $$@,$$@,$$(COM_COLOR),$$(COPY_STRING),$$(OBJ_COLOR) )
endef

# rootcling makes the Dict_rdict.pcm at the same time as making Dict.cxx
# We need to tell make that it will exist once Dict.cxx exists.
%_rdict.pcm: %.cxx
	@touch $@

$(foreach lib,$(LIBRARY_DIRS),$(eval $(call library_template,$(lib))))

-include $(shell find .build -name '*.d' 2> /dev/null)

clean:
	@printf "\n$(WARN_COLOR)Cleaning up$(NO_COLOR)\n\n"
	@-$(RM) -rf .build bin lib include/GVersion.h

cleaner: clean
	@printf "\nEven more clean up\n\n"
	@-$(RM) -rf htmldoc
