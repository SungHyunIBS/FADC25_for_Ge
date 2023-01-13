#ifndef FADC25_H
#define FADC25_H

#include <string>

using namespace std;

class NKNTCP;
class FADC25{

 public :
    FADC25();
    virtual ~FADC25();

    int dummy_func(unsigned, int, int, void *);
    int  FADC25open(string);
    int  FADC25open(string, int);
    void FADC25close();

    void FADC25reset();
    void FADC25start();
    int  FADC25getstat();
    void FADC25settrigmode(int value);
    void FADC25sendtrig();
    void FADC25setthreshold(int value);
    void FADC25setlength(int value);
    int  FADC25geteventnumber();
    long FADC25getdata(int length, int *data);

 private :
    int tcp_Handle;
    NKNTCP *o_nkntcp;
    
#ifdef NKROOT
    ClassDef(FADC25,1)
#endif

	};
#endif
