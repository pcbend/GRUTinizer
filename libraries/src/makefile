
OBJECTS = grutinizer.o 
vpath %.so $(BASE)/libraries
LIBRARIES = $(BASE)/libraries/libTGRUTint.so

all:grutinizer

LDROOT =  $(shell root-config --glibs)  #`root-config --glibs`
#LDROOT += -lXMLParser
#LDROOT += -lXMLIO
#LDROOT += -lProof
LDROOT += -lGuiHtml
LDROOT += -lSpectrum

INC = -I$(BASE)/include
INC += -I$(MIDASSYS)/include

LIBDIR = -L$(BASE)/libraries
ifeq ($(PLATFORM),Darwin)
LIBDIR += -L/opt/X11/lib 
endif
ifdef MIDASSYS
ifeq ($(PLATFORM),Darwin)
LIBDIR += -L$(MIDASSYS)/darwin/lib
else ifeq ($(PLATFORM),Linux)
LIBDIR += -L$(MIDASSYS)/linux/lib
endif
endif

ifndef __APPLE__
LFLAGS = -Wl,--no-as-needed
endif

LIBS =  -lTGRUTint
LIBS += -lGEBFormat

#DETLIBS =  -lGretina
#DETLIBS += -lPhosWall
#DETLIBS += -lLaBr

#DETLIBS += -lGROOT

SYSLIBS = #-ldl -lxml2
SYSLIBS += -lX11 
SYSLIBS += -lXpm


#COMP_STRING="Now Compiling"

grutinizer: $(OBJECTS) $(LIBRARIES)
	@printf " ${COM_COLOR}%s ${OBJ_COLOR}\t grutinizer ${NO_COLOR}" $(COMP_STRING)
	@printf " ${COM_COLOR}%s ${OBJ_COLOR}\t grutinizer ${NO_COLOR}" $(CXX)
	@$(CXX) $^ $(CFLAGS) $(CPPFLAGS) $(INC) $(LIBDIR) $(LIBS) $(DETLIBS) $(LDROOT) $(SYSLIBS) -o $@  2> temp.log || touch temp.errors
	@if test -e temp.errors; then  \
	 printf "\r ${COM_COLOR}%s ${OBJ_COLOR}%-30s ${ERROR_COLOR}%*s${NO_COLOR}\n" $(COMP_STRING) $@ 10 $(ERROR_STRING) \
	 && $(CAT) temp.log  \
	 && printf "${ERROR_COLOR}%s\n${NO_COLOR}" ${PWD};  \
	 elif test -s temp.log; then \
	 printf "\r ${COM_COLOR}%s ${OBJ_COLOR}%-30s ${WARN_COLOR}%*s${NO_COLOR}\n" $(COMP_STRING) $@ 10 $(WARN_STRING) \
	 && $(CAT) temp.log; \
	 else printf "\r ${COM_COLOR}%s ${OBJ_COLOR}%-30s ${OK_COLOR}%*s${NO_COLOR}\n" $(COMP_STRING) $@ 10  $(OK_STRING); \
	 fi;
	@$(RM) -f temp.errors temp.log


%.o: %.cxx
	@printf " ${COM_COLOR}%s ${OBJ_COLOR}%s ${NO_COLOR}" $(COMP_STRING) $@ 
	@$(CXX) -c $^ $(CFLAGS) $(CPPFLAGS) 2> temp.log || touch temp.errors
	@if test -e temp.errors; then \
	 printf "\r ${COM_COLOR}%s ${OBJ_COLOR}%-30s ${ERROR_COLOR}%*s${NO_COLOR}\n" $(COMP_STRING) $@ 10 $(ERROR_STRING) \
	 && $(CAT) temp.log && \
	 printf "${ERROR_COLOR}%s\n${NO_COLOR}" ${PWD};  \
	 elif test -s temp.log; then \
	 printf "\r ${COM_COLOR}%s ${OBJ_COLOR}%-30s ${WARN_COLOR}%*s${NO_COLOR}\n" $(COMP_STRING) $@ 10 $(WARN_STRING) \
	 && $(CAT) temp.log; \
	 else printf "\r ${COM_COLOR}%s ${OBJ_COLOR}%-30s ${OK_COLOR}%*s${NO_COLOR}\n" $(COMP_STRING) $@ 10  $(OK_STRING); \
	 fi;
	@$(RM) -f temp.errors temp.log

clean:
	$(RM) *.o *~ 



