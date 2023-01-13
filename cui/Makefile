########################################################################
FILENAME = cui

TNHOME = ..
INCLUDEDIR = -I$(TNHOME)/include -I$(shell root-config --cflags)
#EXT_LIBS   = -L$(shell root-config --libs)
EXT_LIBS   = $(shell root-config --libs)
CPPFLAGS   = $(INCLUDEDIR)
LDFLAGS	=

LDLIBS	= -L$(TNHOME)/lib -lFADC25 -lNoticeNTCP 

OBJS	= $(FILENAME).o

PROGRAM = $(FILENAME).exe

CXX	= g++
#########################################################################

all	:	$(PROGRAM)

clean	:
		/bin/rm -f $(PROGRAM) $(OBJS)


$(PROGRAM)	:	$(OBJS)
		$(CXX) $(LDFLAGS) $^ $(LDLIBS) $(EXT_LIBS) -o $@
