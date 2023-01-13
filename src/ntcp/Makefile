
FILENAME = NoticeNTCP

NKLIBHOME = ../..

LIB	=	lib$(FILENAME).so

CC	=	g++

COPTS	=	-fPIC -DLINUX -Wall -DNKC

#FLAGS	=	-shared -Wall -nostartfiles -s
FLAGS	=	-shared -Wall -nostartfiles


LIBS	= -L$(NKLIBHOME)/lib

INCLUDEDIR = -I$(NKLIBHOME)/include

INSTALLDIR     =	$(NKLIBHOME)/lib
INSTALLINCLUDE =	$(NKLIBHOME)/include

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

