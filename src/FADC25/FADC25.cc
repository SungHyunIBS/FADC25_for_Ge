#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "FADC25.h"
#include "NoticeNTCP.h"

FADC25::FADC25(){} // constructor
FADC25::~FADC25(){} // destructor

int FADC25::dummy_func(unsigned handle, int xType, int errCode, void *callbackData){
    return 0;
}

int FADC25::FADC25open(string ip){

    char gip[20];
    strcpy(gip, ip.c_str());
    tcp_Handle = o_nkntcp->tcpOpen(gip, 5000);

    return tcp_Handle;
}

int FADC25::FADC25open(string ip, int port){

    char gip[20];
    strcpy(gip, ip.c_str());
    tcp_Handle = o_nkntcp->tcpOpen(gip, port);

    return tcp_Handle;
 
}

void FADC25::FADC25close(){
    o_nkntcp->tcpClose(tcp_Handle);
}

void FADC25::FADC25reset(){

    char buf[2];
    buf[0] = 1;
    o_nkntcp->tcpWrite(tcp_Handle, buf, 1);
    o_nkntcp->tcpRead(tcp_Handle, buf, 1);
    
    return;
}
 
// start NFADC25 DAQ
void FADC25::FADC25start(){

    char buf[2];
    buf[0] = 2;
    o_nkntcp->tcpWrite(tcp_Handle, buf, 1);
    o_nkntcp->tcpRead(tcp_Handle, buf, 1);

    return;
}

// read DAQ status
// return : 0 = buffer is full, 1 = DAQ is still running
int FADC25::FADC25getstat(){

    char buf[2];
    int value;
    buf[0] = 3;
    o_nkntcp->tcpWrite(tcp_Handle, buf, 1);
    o_nkntcp->tcpRead(tcp_Handle, buf, 1);
    value = buf[0] & 0xFF;

    return value;
}

// set trigger mode
// 0 = self trigger mode, 1 = external trigger mode
void FADC25::FADC25settrigmode(int value){
    
    char buf[2];
    buf[0] = 4;
    buf[1] = value & 0xFF;

    o_nkntcp->tcpWrite(tcp_Handle, buf, 2);
    o_nkntcp->tcpRead(tcp_Handle, buf, 1);

    return;
}

// send random trigger signal
void FADC25::FADC25sendtrig(){

    char buf[2];
    buf[0] = 5;
    o_nkntcp->tcpWrite(tcp_Handle, buf, 1);
    o_nkntcp->tcpRead(tcp_Handle, buf, 1);
  
    return;
}

// set discriminator threshold
void FADC25::FADC25setthreshold(int value){

    char buf[3];
    buf[0] = 6;
    buf[1] = value & 0xFF;
    buf[2] = (value >> 8) & 0xFF;

    o_nkntcp->tcpWrite(tcp_Handle, buf, 3);
    o_nkntcp->tcpRead(tcp_Handle, buf, 1);
  
    return;
}

// set one event storage length
// 0 = 2.5us (= 64 samples),   	1 = 5us (= 128 samples), 
// 2 = 10us (= 256 samples),    3 = 20us (= 512 samples), 
// 4 = 40us (= 1024 samples),   5 = 80us (= 2048 samples), 
// 6 = 160us (= 4096 samples),  7 = 320us (= 8192 samples), 
void FADC25::FADC25setlength(int value){
    
    char buf[2];
    buf[0] = 7;
    buf[1] = value & 0xFF;

    o_nkntcp->tcpWrite(tcp_Handle, buf, 2);
    o_nkntcp->tcpRead(tcp_Handle, buf, 1);
    
    return;
}

int FADC25::FADC25geteventnumber(){

    char buf[2];
    int value;
    buf[0] = 8;

    o_nkntcp->tcpWrite(tcp_Handle, buf, 1);
    o_nkntcp->tcpRead(tcp_Handle, buf, 1);

    value = buf[0] & 0xFF;

    return value;
    
}

// get ADC data
// returns time tag in ns (40 ns precision)
// length = set value in FADC25setlength
// data size is dependent to length as follows
// 0 = 64   	1 = 128
// 2 = 256      3 = 512
// 4 = 1024     5 = 2048
// 6 = 4096   	7 = 8192
long FADC25::FADC25getdata(int length, int *data){

    char buf[16384] = {0};
    int nsample(0), nchunk(0);
    long time_tag(0), ltmp(0);
	
    nsample = 64 << (length);
	
    if(nsample < 1024){

	buf[0] = 9;
	buf[1] = length & 0xFF;

	o_nkntcp->tcpWrite(tcp_Handle, buf, 2);
	o_nkntcp->tcpRead(tcp_Handle, buf, nsample * 2);
   	
	for (int i = 0; i < nsample; i++){
	    data[i] = buf[2 * i + 1] & 0xFF;
	    data[i] = data[i] << 8;
	    data[i] = data[i] + (buf[2 * i] & 0xFF);
	}	
    }else{
	
	nchunk = (nsample >> 9) - 1;
    
	// receive first chunk
	buf[0] = 9;
	buf[1] = length & 0xFF;

	o_nkntcp->tcpWrite(tcp_Handle, buf, 2);
	o_nkntcp->tcpRead(tcp_Handle, buf, 1024);
	
	for(int i = 0; i < 512; i++){
	    data[i] = buf[2 * i + 1] & 0xFF;
	    data[i] = data[i] << 8;
	    data[i] = data[i] + (buf[2 * i] & 0xFF);
	}	
		
	// receive successive chunk
	for(int chunk = 0; chunk < nchunk; chunk++){

	    buf[0] = 10;
	    o_nkntcp->tcpWrite(tcp_Handle, buf, 1);
	    o_nkntcp->tcpRead(tcp_Handle, buf, 1024);
	    for(int i = 0; i < 512; i++){
		data[512 * chunk + i + 512] = buf[2 * i + 1] & 0xFF;
		data[512 * chunk + i + 512] = data[512 * chunk + i + 512] << 8;
		data[512 * chunk + i + 512] = data[512 * chunk + i + 512] + (buf[2 * i] & 0xFF);
	    }	
	}
    }

    // read time tag
    buf[0] = 12;
    o_nkntcp->tcpWrite(tcp_Handle, buf, 1);
    o_nkntcp->tcpRead(tcp_Handle, buf, 6);

    time_tag = 0;
    ltmp = buf[0] & 0xFF;
    time_tag = time_tag + ltmp;
    ltmp = buf[1] & 0xFF;
    ltmp = ltmp << 8;
    time_tag = time_tag + ltmp;
    ltmp = buf[2] & 0xFF;
    ltmp = ltmp << 16;
    time_tag = time_tag + ltmp;
    ltmp = buf[3] & 0xFF;
    ltmp = ltmp << 24;
    time_tag = time_tag + ltmp;
    ltmp = buf[4] & 0xFF;
    ltmp = ltmp << 32;
    time_tag = time_tag + ltmp;
    ltmp = buf[5] & 0xFF;
    ltmp = ltmp << 40;
    time_tag = time_tag + ltmp;
    time_tag = time_tag * 40;

    // move to next event
    buf[0] = 11;

    o_nkntcp->tcpWrite(tcp_Handle, buf, 1);
    o_nkntcp->tcpRead(tcp_Handle, buf, 1);

    return time_tag;
    
}

