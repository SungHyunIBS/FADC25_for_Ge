########################################################################
#                                                                      #
#   FADC25 C Libary                                                       #
#                                                                      #
#   Created: Oct/14/2020                                               #
#                                                                      #
########################################################################

FILENAME = FADC25

TNHOME = ../..

LIB	=	lib$(FILENAME).so

CC	=	g++

COPTS	=	-fPIC -DLINUX -Wall -DNKC

#FLAGS	=	-shared -Wall -nostartfiles -s
FLAGS	=	-shared -Wall -nostartfiles

LIBS	= -L$(TNHOME)/lib
INCLUDEDIR = -I$(TNHOME)/include

INSTALLDIR     =	$(TNHOME)/lib
INSTALLINCLUDE =	$(TNHOME)/include

OBJS	=	$(FILENAME).o

INCLUDES =	$(FILENAME).h 


#########################################################################

all	:	$(LIB)

clean	:
		/bin/rm -f $(OBJS) $(LIB)

install	:	$(LIB)
		install -m 755 $(LIB) $(INSTALLDIR)
		install -m 644 $(FILENAME).h $(INSTALLINCLUDE)
		/bin/rm -f $(LIB)



$(LIB)	:	$(OBJS)
		$(CC) $(FLAGS) -o $(LIB) $(OBJS) $(DEPLIBS)

$(OBJS)	:	$(INCLUDES) 

%.o	:	%.cc
		$(CC) $(COPTS) $(INCLUDEDIR) -c -o $@ $<

