#include <iostream>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <cstdio>
#include <math.h>

//int tcp_Handle;
using namespace std;
int set_RL(int);
int read_runno();

int bitode(string);
string detobi(int vvv, int flag_reverse, int n_bit);
string itos(int);
void u_time();

int set_RL(int data_point){

    int record_length(0);

    switch(data_point){
    case   64: record_length = 0; break;
    case  128: record_length = 1; break;
    case  256: record_length = 2; break;
    case  512: record_length = 3; break;
    case 1024: record_length = 4; break;
    case 2048: record_length = 5; break;
    case 4096: record_length = 6; break;
    default:   record_length = 7; break;
    }
    
    return record_length;
}

int read_runno(){

    int runno(0);
    
    FILE *ff = fopen("runno", "r+"); // read and write
    fscanf (ff, "%d", &runno);       // read current runno
    fclose(ff);

    return runno;
}

// bit to decimal
int bitode(string ch){
  
    int dec = 0;
    for (int i = 0; i < (int)ch.size(); ++i) {
	dec += (int(ch[i]) - 48)*(int)pow(2.0, i);
    }  
    return dec;
}
// decimal to binary
string detobi(int vvv,
	      int flag_reverse,
	      int n_bit){
    string binary = "";
    int mask = 1;
    for(int iii = 0; iii < n_bit; iii++){
	if((mask&vvv) >= 1){ binary = "1"+binary; }
	else               { binary = "0"+binary; }
        mask<<=1;
    }
    if(flag_reverse==1){
	reverse(binary.begin(), binary.end());
    }
    return binary;
}
// int to string
string itos(int f){
    stringstream s;
    s << f;
    return s.str();
}

string u_int2string( int u_int, int u_num ){

    string aaa = "0";
    ostringstream bbb;
    bbb << u_int;
    if ( u_num == 0 || u_num == 1  ){
        aaa = bbb.str();
    }else if ( u_num == 2 ){
        if (u_int<10) { aaa = "0" + bbb.str();}
        else          { aaa =       bbb.str(); }
    } else if ( u_num == 3 ){
        if      (u_int< 10) { aaa = "00" + bbb.str(); }
        else if (u_int<100) { aaa =  "0" + bbb.str(); }
        else           { aaa =        bbb.str(); }
    } else if ( u_num == 4 ){
        if      (u_int <  10) { aaa = "000" + bbb.str(); }
        else if (u_int < 100) { aaa =  "00" + bbb.str(); }
        else if (u_int <1000) { aaa =   "0" + bbb.str(); }
        else            { aaa =         bbb.str(); }
    } else if ( u_num == 5 ){
        if      (u_int<   10) { aaa = "0000" + bbb.str(); }
        else if (u_int<  100) { aaa =  "000" + bbb.str(); }
        else if (u_int< 1000) { aaa =   "00" + bbb.str(); }
        else if (u_int<10000) { aaa =    "0" + bbb.str(); }
        else             { aaa =          bbb.str(); }
    } else if ( u_num == 6 ){
        if (u_int<    10) { aaa = "00000" + bbb.str(); }
        else if (u_int<   100) { aaa =  "0000" + bbb.str(); }
        else if (u_int<  1000) { aaa =   "000" + bbb.str(); }
        else if (u_int< 10000) { aaa =    "00" + bbb.str(); }
        else if (u_int<100000) { aaa =     "0" + bbb.str(); }
        else                   { aaa =           bbb.str(); }
    }
    return aaa;
}

void u_time(){

    time_t timer;
    struct tm *local;
    
    timer = time(NULL);
    local = localtime(&timer);
    printf("[%d/%d/%d %2d:%2d:%2d]\n",
	   local->tm_year + 1900,
	   local->tm_mon + 1,
	   local->tm_mday,
	   local->tm_hour,
	   local->tm_min,
	   local->tm_sec);

    return;
}

void u_printstatk(int i){

    if(i % 1000 == 0){
        printf("%.6dk",(int)i/1000);
        fflush(stdout);
        printf("\r");
    }
}

void u_printstat(int i){

    printf("%.6d", i);
    fflush(stdout);
    printf("\r");
}

void u_printstat(int i, int j){

    printf("%.3d\t %.6d", i, j);
    fflush(stdout);
    printf("\r");
}

void u_printstatp(int t, int i){

    if(i % 1000 == 0){
        printf("%.2f%%",(double)i/t*100);
        fflush(stdout);
        printf("\r");
    }
}

void u_printstatsec(int hour, int time, int cnt){

    printf("%.8d\t %.2d hour\t %.4d sec", cnt, hour, time);
    fflush(stdout);
    printf("\r");
}
