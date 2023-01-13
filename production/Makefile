FILENAME = prod

PRODHOME = .

INCLUDEDIR = -I$(PRODHOME)/ -I$(shell root-config --cflags)
CPPFLAGS = $(INCLUDEDIR)

LDFLAGS	=

LDLIBS	= -L$(ROOTSYS)/lib -L$(shell root-config --glibs)#-lCint -lRIO #-lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMat hCore -lThread -pthread -lm -ldl -rdynamic -lRooFit -lMinuit 

OBJS	= $(FILENAME).o

PROGRAM = $(FILENAME).run

CXX = g++
#########################################################################

all	:	$(PROGRAM)

clean	:
		/bin/rm -f $(PROGRAM) $(OBJS)


$(PROGRAM)	:	$(OBJS)
		$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@


