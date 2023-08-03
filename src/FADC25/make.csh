#! /bin/tcsh -f

make clean 
make 
make install

rm -f *.o
