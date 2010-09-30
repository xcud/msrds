#
# Makefile for LSTM
#
# To create a tbz for the webpage do: make tar 
#

SRC_DIR = ../
BASE_DIR = $(SRC_DIR)/base
LSTM_DIR = $(SRC_DIR)/LSTM

INCPATH = -I$(BASE_DIR)
INCPATH += -I$(LSTM_DIR)

LIBPATH = 
LIBS_STD = -lm #-lncurses #-lefence 
LIBS_QT	 = #-lqt-mt #-lefence #-Wlqt-mt #-Wlqt
LIBS	 = $(LIBPATH) $(LIBS_STD) $(LIBS_QT) $(LIBS_GSL)
#MOC  	 = $(QTDIR)/bin/moc 


LFLAGS  = #-lefence    #-wL static -L$(QTDIR)/lib
CXX 	 = g++ #checkergcc #g++
OPTIONS  = -O0 -g  # -O4 #-g # -pg #-DQT_THREAD_SUPPORT -O0 -g # -O3
CFLAGS   = -Wall -Winline $(OPTIONS)

# ####### Target and Customization 

PROJECT = LSTM
TARGET  = ${PROJECT}

# ####### Files 

SOURCES = \
	$(LSTM_DIR)/LSTM.cpp \
	$(LSTM_DIR)/neuralNetBase.cpp \
	$(LSTM_DIR)/patternManagement.cpp \
	$(BASE_DIR)/IOBase.cpp

OBJECTS = $(SOURCES:.cpp=.o)
#OBJECTS += 

# Header with function (for etags) bodies to build tags.
HEADERS = $(LSTM_DIR)/*.h $(BASE_DIR)/*.h

# ####### Implicit rules
.SUFFIXES: .cpp
.cpp.o: 
	$(CXX) -c $(CFLAGS) $(INCPATH) $(LFLAGS)  -o $@ $<

# ####### Build rules

all: $(TARGET) 

$(TARGET): $(OBJECTS) makefile 
	$(CXX) $(LFLAGS) $(CFLAGS) -o $(PROJECT) $(OBJECTS) $(LIBS)

clean:
	-rm -f $(OBJECTS) $(PROJECT) $(OBJMOC) $(SRCMOC)
	-rm -f *.o *~ core $(TARGET)

# Create TAGS file for emacs
# Headers with function bodies must be listed 
# along wiht .cpp files.
tags: TAGS 
TAGS: $(SOURCES) $(HEADERS) dep 
	etags --declarations $(HEADERS) $(SOURCES) >$@	

########## dependencies

dep: $(SOURCES)
	$(CXX) -M $(INCPATH) $(SOURCES) > depend.dep
ifeq ($(wildcard depend.dep), depend.dep)
include depend.dep
endif

