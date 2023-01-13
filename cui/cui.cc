// C/C++ Lib
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <time.h>
// User Lib
#include "FADC25.h"
#include "util.h"

// ROOT Lib
#include <TSystem.h>
#include <TObject.h>
#include <TROOT.h>
#include <TPDF.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TFile.h>
#include <TTree.h>

using namespace std;

void print_menu();
void print_menu_config();
void print_menu_debug();

void read_config(vector<int> &);
int set_runnum();
double read_baseline(FADC25*, int, int);
    
void take_data_nevt    (FADC25*, int, int);
void take_data_time_sec(FADC25*, int, int);

const string ip   = "192.168.2.2";
const int ohour   = 3600;
const int nevtcut = 100000;
const double bintons = 40; //40 for 25MHz

// debug
void read_data(FADC25*, int, int, int);

int main(){

    FADC25 *FADC = new FADC25();

    bool loop_act_mast   = false;
    int Action_mast(0);

    bool loop_act_config = false;
    int Action_config(0);

    bool loop_act_debug  = false;
    int Action_debug(0);

    // config initial values
    int data_point(1024); // 0
    int trg_mode(0);      // 1
    int threshold(25);    // 2
    double baseline(60);  // 3
    vector<int> rconfig;
    
    FILE *fr, *fw;

    int runno(0);

    int nevent(0);
    int ntime(0);
    int nhour(0);

    // Debug
    int record_length = set_RL(data_point);
    int record_time   = data_point * bintons;
    int status(0);

    while(!loop_act_mast){
	print_menu();
	cout << "Select action? " << endl;
	cin >> Action_mast;
	switch(Action_mast){
	case 0: // exit
	    loop_act_mast = true;
	    cout << "Bye" << endl;
	    break;
	case 1: //
	    loop_act_config = false;
	    while(!loop_act_config){
		print_menu_config();
		cout << "Select action? " << endl;
		cin >> Action_config;
		switch(Action_config){
		case 0: // exit
		    loop_act_config = true;
		    break;
		case 1: // Read
		    
		    rconfig.clear();
		    read_config(rconfig);
		    
		    data_point = rconfig[0];
		    trg_mode   = rconfig[1];
		    threshold  = rconfig[2];
		    cout << "Data point : " << data_point << endl;
		    cout << "Trg Mode   : " << trg_mode   << endl;
		    cout << "threshold  : " << threshold  << endl;
		    break;
		    
		case 2: // Write
		    
		    cout << "1 : Set Data point ? " << endl;
		    cout << "Current Setting : " << data_point << endl;
		    cin >> data_point;
		    
		    cout << "2 : Set Trigger Mode ? " << endl;
		    cout << "(0) : self Trigger\t (1) : External Trigger " << endl;
		    cout << "Current Setting : " << trg_mode << endl;
		    cin >> trg_mode;
		    
		    cout << "3 : Set Threshold (0 - 4095) ? " << endl;
		    cout << "Current Setting : " << threshold << endl;
		    cin >> threshold;

		    cout << "Write config file" << endl;
		    fw = fopen("config", "w");
		    fprintf(fw, "%d\n", data_point);
		    fprintf(fw, "%d\n", trg_mode);
		    fprintf(fw, "%d\n", threshold);
		    fclose(fw);
		
		    break;

		default:
		    loop_act_config = true;
		    break;
		}
	    } // Action_config
	    break;
	    
	case 10:
	    cout << "How many events to take? " << endl;
	    cin >> nevent;
	    if(nevent <= 0){
		cout << "Wrong # event value" << endl;
		break;
	    }

	    // Directory setting
	    runno = set_runnum();

	    // Take a data with nevent
	    take_data_nevt(FADC, nevent, runno);

	    break;
	case 11:
	    cout << "Time? (hour)" << endl;
	    cin >> nhour;
	    if(nhour <= 0){
		cout << "Wrong # time value" << endl;
		break;
	    }
	    // Directory setting
	    runno = set_runnum();
	    ntime = nhour * ohour;
	    // Take a data with nevent
	    take_data_time_sec(FADC, ntime, runno);

	    break;
	case 12:
	    cout << "Time? (second)" << endl;
	    cin >> ntime;
	    if(ntime <= 0){
		cout << "Wrong # time value" << endl;
		break;
	    }
	    // Directory setting
	    runno = set_runnum();

	    // Take a data with nevent
	    take_data_time_sec(FADC, ntime, runno);

	    break;
	case 99: // debug
	    while(!loop_act_debug){
		loop_act_debug = false;
		print_menu_debug();
		cout << "Select action? " << endl;
		cin >> Action_debug;
		switch(Action_debug){
		case 0: // exit
		    loop_act_debug = true;
		    cout << "Bye" << endl;
		    break;
		case 1:
		    FADC->FADC25open(ip);
		    FADC->FADC25reset();
		    FADC->FADC25close();
		    break;
		case 2:
		    cout << "Set Data point ? " << endl;
		    cout << "Current Setting : " << data_point << endl;
		    cin >> data_point;
		    record_length = set_RL(data_point);
		    record_time = data_point * bintons;
		    cout << "Total recording time in ns = " << record_time << endl;
		    cout << "Record_length " << record_length << endl;
		    FADC->FADC25open(ip);
		    FADC->FADC25setlength(record_length);
		    FADC->FADC25close();	    
		    break;

		case 3:
		    cout << "Set Trigger Mode ? (0) : Self Trigger    (1) : External Trigger " << endl;
		    cin >> trg_mode;
		    FADC->FADC25open(ip);
		    FADC->FADC25settrigmode(trg_mode);
		    FADC->FADC25close();
		    break;

		case 4:
		    cout << "Set Threshold (0 - 4095) ? " << endl;
		    cin >> threshold;
		    FADC->FADC25open(ip);
		    FADC->FADC25setthreshold(threshold);
		    FADC->FADC25close();
		    break;

		case 5:
		    cout << "DAQ Start" << endl;
		    FADC->FADC25open(ip);
		    FADC->FADC25start();
		    FADC->FADC25close();
		    break;
	    
		case 6:
		    FADC->FADC25open(ip);
		    status = FADC->FADC25getstat();
		    FADC->FADC25close();
		    if(status == 0){
			cout << "Buffer is full" << endl;
		    }else{
			cout << "DAQ is still running" << endl;
		    }
		    break;

		case 7:
		    cout << "Send Random Trigger" << endl;
		    FADC->FADC25open(ip);
		    FADC->FADC25sendtrig();
		    FADC->FADC25close();
		    break;

		case 8:
		    cout << "Now measuring baseline" << endl;
		    for(int iii = 0; iii < 3; iii++){
			baseline = read_baseline(FADC, record_length, threshold);
			sleep(1);
		    }
		    cout << "Measured baseline " << baseline << endl;
		    break;
		
		case 9:
		    cout << "Data point    : " << data_point << endl;
		    cout << "Record length : " << record_length << endl;
		    cout << "Threshold     : " << threshold << endl;
		    read_data(FADC, data_point, record_length, threshold);
		    break;

		case 10:
		    fr = fopen("runno", "r");
		    fscanf(fr, "%d", &runno);
		    fclose(fr);
		    cout << "Current runno = " << runno << endl;
		    cout << "Set ? " << endl;
		    cin >> runno;

		    fw = fopen("runno", "w");
		    fprintf(fw, "%d", runno);
		    fclose(fw);
		    break;
		    
		default:
		    loop_act_debug = false;
		    break;
		} 
	    } // Action_debug
	    break;
	    
	default:
	    break;
	    
	}
    } // Action_mast
    return 0;
}

void print_menu(){
    
    cout << "===========================================================" << endl;
    cout << "  0    : Quit                                              " << endl;
    cout << "  1    : Config                                            " << endl;
    cout << "-----------------------------------------------------------" << endl;
    cout << " 10    : Take a data with Nevent                           " << endl;
    cout << " 11    : Take a data with time (hour)                      " << endl;
    cout << " 12    : Take a data with time (sec)                       " << endl;
    cout << "===========================================================" << endl;
    cout << " 99    : Debug Mode                                        " << endl;
    cout << "===========================================================" << endl;
    
    return;
}

void print_menu_config(){
    
    cout << "===========================================================" << endl;
    cout << "  0    : Quit                                              " << endl;
    cout << "-----------------------------------------------------------" << endl;
    cout << "  1    : Read config           2    : Write config         " << endl;
    cout << "===========================================================" << endl;
    
    return;
}

void print_menu_debug(){

    cout << "===========================================================" << endl;
    cout << "  0    : Quit                                              " << endl;
    cout << "-----------------------------------------------------------" << endl;
    cout << "  1    : Reset                 6    : Read DAQ Status      " << endl;;
    cout << "  2    : Set Record Length     7    : Send Random Trigger  " << endl;
    cout << "  3    : Set Trigger Mode      8    : Read Baseline        " << endl;
    cout << "  4    : Set Threshold         9    : Read Data            " << endl;
    cout << "  5    : DAQ Start            10    : Set Runno            " << endl;
    cout << "===========================================================" << endl;
    
    return;
}

void read_config(vector<int> &rconfig){

    FILE *fr = fopen("config", "r");
    int tmp1(0), tmp2(0), tmp3(0);
    
    fscanf(fr, "%d",  &tmp1);
    fscanf(fr, "%d",  &tmp2);
    fscanf(fr, "%d",  &tmp3);
    fclose(fr);

    rconfig.push_back(tmp1);
    rconfig.push_back(tmp2);
    rconfig.push_back(tmp3);

}

int set_runnum(){

    int runno(0);
    // Setting
    //    system("mv runno tmp");
    // FILE *fr = fopen("tmp", "r");
    // fscanf (fr, "%d", &runno);
    // fclose(fr);
    // system ("rm -rf tmp");
	    
    FILE *fr = fopen("runno", "r");
    fscanf (fr, "%d", &runno);
    fclose(fr);
    
    runno += 1;
    FILE *fw = fopen("runno", "w");
    fprintf(fw, "%d", runno);
    fclose(fw);
    TString newdir = Form("mkdir data/r%.4d", runno);
    system(newdir.Data());

    return runno;
}

double read_baseline(FADC25* FADC, int record_length, int threshold){

    double baseline(0);
    int data[8192] = {0};

    FADC->FADC25open(ip);
    // Read Baseline
    FADC->FADC25reset();
    FADC->FADC25setlength(7);
    FADC->FADC25setthreshold(4095);
    FADC->FADC25start();
    for(int i = 0; i < 16; i++){
	FADC->FADC25sendtrig();
	usleep(1);
    }
    for(int i = 0; i < 16; i++){
	FADC->FADC25getdata(7, data);
	for(int j = 0; j < 8192; j++){
	    baseline += (data[i] & 0xFFF);
	}
    }
    baseline = baseline / 8192 / 16;

    // Reset
    FADC->FADC25reset();
    FADC->FADC25setlength(record_length);
    FADC->FADC25setthreshold(threshold);
    FADC->FADC25close();

    return baseline;
}

void take_data_nevt(FADC25* FADC, int nevent, int runno){

    vector<int> rconfig;
    rconfig.clear();
    read_config(rconfig);
    
    int data_point    = rconfig[0];
    int trg_mode      = rconfig[1];
    int threshold     = rconfig[2];
    int record_length = set_RL(data_point);
    double baseline(0);
    cout << "Now measuring baseline" << endl;
    for(int iii = 0; iii < 3; iii++){
	baseline = read_baseline(FADC, record_length, threshold);
	sleep(1);
    }
    cout << "Measured baseline " << baseline << endl;

    int th_ped = (int)baseline + threshold;
    time_t evt_t;
    long evttime;

    Long64_t time_tag;
    double dtime_tag(0);

    int itime_tag(0), ctime_tag;
    Int_t sdata[data_point];
    int data[8192] = {0};

    TString sname = Form("data/r%.4d/out_%.4d_nevt_", runno, runno);
    TString ssub, fname;

    bool flag = true;
    TFile *nf;
    TTree *nt; 
    TTree *daq;
    int cnt(0);
    string nfadc;

    int quotient  = nevent / nevtcut;
    int remainder = nevent % nevtcut;

    for(int i = 0; i <= quotient; i++){
	cnt  = 0;
	flag = true;
	if(i == quotient && remainder == 0){
	    break;
	}

	ssub  = Form("%.3d.root", i);
	fname = sname + ssub;

	nf  = new TFile(fname, "recreate");
	nt  = new TTree("nt", "FADC");
	daq = new TTree("daq", "header");

	nfadc = "FADC["+u_int2string(data_point,1)+"]/I";
	nt->Branch("fadc",  &sdata, nfadc.c_str());
	nt->Branch("evtt",  &evttime, "EVTT/L");
	nt->Branch("DAQtime",  &time_tag);
	nt->Branch("DAQtimeD", &dtime_tag);

	daq->Branch("runno",     &runno, "runno/I");
	daq->Branch("subno",     &i,     "subno/I");
	daq->Branch("evtno",     &cnt,   "evtno/I");
	daq->Branch("threshold", &threshold);
	daq->Branch("recl",      &record_length);
	daq->Fill();

	//Run start
	FADC->FADC25open(ip);
	FADC->FADC25reset();
	FADC->FADC25setlength(record_length);
	FADC->FADC25settrigmode(trg_mode);
	FADC->FADC25setthreshold(th_ped);

	FADC->FADC25start();
	
	while(flag){
	    while(!FADC->FADC25geteventnumber()){;}

	    time_tag  = FADC->FADC25getdata(record_length, data);
	    dtime_tag = (double) time_tag;
	    time_tag  = (time_tag / 40) & 0x1FFF;

	    for(int j = 0; j < data_point; j++){
		sdata[j] = data[j] & 0xFFF;
	    }
	    evttime = time(&evt_t);
	    nt->Fill();
	    u_printstat(i, cnt);
	    if(quotient == 0){
		if(cnt == remainder - 1){
		    flag = false;
		}
	    }else if(i == quotient){
		if(cnt == remainder - 1){
		    flag = false;
		}
	    }else{
		if(cnt == nevtcut - 1){
		    flag = false;
		}
	    }
	    cnt++;
	}
	nf->Write();
	FADC->FADC25reset();
	FADC->FADC25close();
    }

    return;
}

void take_data_time_sec(FADC25* FADC, int ntime, int runno){

    vector<int> rconfig;
    rconfig.clear();
    read_config(rconfig);
    
    int data_point    = rconfig[0];
    int trg_mode      = rconfig[1];
    int threshold     = rconfig[2];
    int record_length = set_RL(data_point);
    double baseline(0);
    cout << "Now measuring baseline" << endl;
    for(int iii = 0; iii < 3; iii++){
	baseline = read_baseline(FADC, record_length, threshold);
	sleep(1);
    }
    cout << "Measured baseline " << baseline << endl;

    int th_ped = (int)baseline + threshold;
    time_t evt_t;
    long evttime;

    Long64_t time_tag;
    double dtime_tag(0);

    int itime_tag(0), ctime_tag;
    Int_t sdata[data_point];
    int data[8192] = {0};
    
    TString sname = Form("data/r%.4d/out_%.4d_ntime_", runno, runno);
    TString ssub, fname;

    bool flag = true;
    TFile *nf;
    TTree *nt; 
    TTree *daq;
    int cnt(0);
    string nfadc;

    int nhour = ntime / ohour;
    int nsec  = ntime % ohour;

    double itime, ftime, dtime;
    
    for(int i = 0; i <= nhour; i++){

	cnt  = 0;
	flag = true;

	if(i == nhour && nsec == 0){
	    break;
	}

	ssub  = Form("%.3d.root", i);
	fname = sname + ssub;

	nf  = new TFile(fname, "recreate");
	nt  = new TTree("nt", "FADC");
	daq = new TTree("daq", "header");

	nfadc = "FADC["+u_int2string(data_point,1)+"]/I";
	nt->Branch("fadc",  &sdata, nfadc.c_str());
	nt->Branch("evtt",  &evttime, "EVTT/L");
	nt->Branch("DAQtime",  &time_tag);
	nt->Branch("DAQtimeD", &dtime_tag);

	daq->Branch("runno",     &runno, "runno/I");
	daq->Branch("subno",     &i,     "subno/I");
	daq->Branch("evtno",     &cnt,   "evtno/I");
	daq->Branch("threshold", &threshold);
	daq->Branch("recl",      &record_length);
	daq->Fill();

	//Run start
	FADC->FADC25open(ip);
	FADC->FADC25reset();
	FADC->FADC25setlength(record_length);
	FADC->FADC25settrigmode(trg_mode);
	FADC->FADC25setthreshold(th_ped);
    
	FADC->FADC25start();

	while(flag){
	    while(!FADC->FADC25geteventnumber()){;}

	    time_tag  = FADC->FADC25getdata(record_length, data);
	    dtime_tag = (double) time_tag;
	    time_tag  = (time_tag / 40) & 0x1FFF;

	    ftime = dtime_tag * 1.e-09;
	    if(cnt == 0){
		itime = ftime;
	    }
	    dtime = ftime - itime;

	    for(int j = 0; j < data_point; j++){
		sdata[j] = data[j] & 0xFFF;
	    }
	    evttime = time(&evt_t);
	    nt->Fill();

	    u_printstatsec(i, (int)dtime, cnt);
	    
	    if(nhour == 0){
		if(dtime > nsec - 1){
		    flag = false;
		}
	    }else if(i == nhour){
		if(dtime > nsec - 1){
		    flag = false;
		}
	    }else{
		if(dtime > ohour){
		    flag = false;
		}
	    }
	    cnt++;
	}
	nf->Write();
	FADC->FADC25reset();
	FADC->FADC25close();

    }

    return;
}

void read_data(FADC25* FADC, int data_point, int record_length, int threshold){

    double baseline(0);
    cout << "Now measuring baseline" << endl;
    for(int iii = 0; iii < 3; iii++){
	baseline = read_baseline(FADC, record_length, threshold);
	sleep(1);
    }
    cout << "Measured baseline " << baseline << endl;
    int th_ped = (int)baseline + threshold;
    long time_tag(0);
    int data[8192] = {0};

    FADC->FADC25open(ip);
    FADC->FADC25reset();
    FADC->FADC25setlength(record_length);
    FADC->FADC25settrigmode(0); // trg mode
    FADC->FADC25setthreshold(th_ped);
    FADC->FADC25start();
    while(!FADC->FADC25geteventnumber()){;}
    time_tag = FADC->FADC25getdata(record_length, data);
    FADC->FADC25reset();
    FADC->FADC25close();

    // output 
    FILE *fp = fopen("read_data.dat", "w");
    for(int i = 0; i < data_point; i++){
	fprintf(fp, "%d\n", data[i]);
    }
    fclose(fp);

    string sdata = u_int2string(data_point,1);
    
    // Open output data using ROOT
    ostringstream cmd;
    cmd << "root -l waveform.C\'("+sdata+")\'";
    system(cmd.str().c_str());

    return;
}
