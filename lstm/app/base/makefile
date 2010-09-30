C++ = g++
CFLAG = -O4 -Wall  
#CFLAG = -g -O0 -Wall 
#CFLAG = -g -O0 -Wall # -pg

LFLAG = #-lm -ltiff #-lefence
INC = 
CXXFLAGS = $(CFLAG) $(INC) 

EXE = testReadBuffer

SRC =   ${EXE}.cpp \
	IOBase.cpp \
	scanDataFormat.cpp

OBJS = $(SRC:.cpp=.o)

.SUFFIXES: .cpp
.cpp.o: makefile
	$(C++) $(CXXFLAGS) -c $(INC) -o $@ $< 

all: $(EXE) 

$(EXE):  $(OBJS) dep makefile
	$(C++) $(CXXFLAGS) $(LFLAG) -o $(EXE) $(OBJS)  

clean:
	-rm *.o
	-rm depend.dep
	-rm $(EXE)

dep: $(SRC)
	$(C++) -M $(CXXFLAGS) $(SRC) > depend.dep

ifeq ($(wildcard depend.dep), depend.dep)
include depend.dep
endif

# -------------------------------------------------------
tags: $(SRC)
	etags --declarations $(SRC) > $@
