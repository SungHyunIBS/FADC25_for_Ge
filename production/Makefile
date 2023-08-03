FILENAME = prod

PRODHOME = .

INCLUDEDIR = -I$(PRODHOME)/ -I$(shell root-config --cflags)
CPPFLAGS = $(INCLUDEDIR)

LDFLAGS	=

LDLIBS	= -L$(ROOTSYS)/lib $(shell root-config --glibs)

OBJS	= $(FILENAME).o

PROGRAM = $(FILENAME).run

CXX = g++
#########################################################################

all	:	$(PROGRAM)

clean	:
		/bin/rm -f $(PROGRAM) $(OBJS)


$(PROGRAM)	:	$(OBJS)
		$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@


