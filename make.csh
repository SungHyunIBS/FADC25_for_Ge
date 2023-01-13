#! /bin/tcsh -f
mkdir include lib
cd src
cd ntcp
make clean; make; make install;
cd ../FADC25
make clean; make; make install;
