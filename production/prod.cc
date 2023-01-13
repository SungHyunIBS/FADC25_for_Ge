#include <iostream>

#include <TObject.h>
#include <TString.h>
#include <TFile.h>
#include <TChain.h>
#include <TMath.h>

#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

using namespace std;

const int PedRange = 100;
#define DATA_POINT 1024

class fadc{
 public:
  Int_t FADC[DATA_POINT];
};

int main(int argc, char ** argv){

    
    if(argc != 4){
     	cout << "Wrong input (./prod.run type runno subno)" << endl;
	return 0;
    }
    
    TString stype   = argv[1];
    TString srun    = argv[2];
    TString ssubrun = argv[3];

    int irun        = atoi(srun);
    int subrun      = atoi(ssubrun);

    TString iname, fname;
    iname = Form("data/r%.4d/out_%.4d_", irun, irun)+stype+Form("_%.3d.root", subrun);
    fname = Form("prod/r%.4d/prod_%.4d_", irun, irun)+stype+Form("_%.3d.root", subrun);    

    TFile *fin;
    TFile *fout;

    TTree* fadc25;
    TTree* tree;

    int firstsub(0);
    // Input
    int adc[DATA_POINT];
    Double_t ttimeD;
    Long64_t evtt;

    // Output
    int    runno;
    int    subno;
    int    evtno;
    int    fmaxx;

    // sttime   : first event timestamp (sec)
    // stimeD   : first event timestamp (nsec)
    // daqtime  : event timestamp (nsec)
    // daqtimeC : event timestamp (nsec), overflow correction
    // trgtime  : event timestamp from first event (sec)
    double sttime(0), sttimeD(0);

    double trgtime;
    double daqtime, daqtimeC;
    double curtime;

    double pedm, peds;
    double fmax, qsum;

    int    flag_sat;

    // Production
    int    nent(0), ntotent(0);
    double tmprms, fadc;

    fin = new TFile(iname);
    if(fin->IsZombie()){
	fin->Close();
    }
    fout = new TFile(fname, "recreate");

    fadc25 = (TTree*)fin->Get("nt");
    fadc25->SetBranchAddress("DAQtimeD", &ttimeD);
    fadc25->SetBranchAddress("fadc",     adc);
    fadc25->SetBranchAddress("evtt",     &evtt);

    tree = new TTree("prod", "prod");

    tree->Branch("evtno",    &evtno);
    tree->Branch("evtnot",   &ntotent);
    tree->Branch("pedm",     &pedm);
    tree->Branch("peds",     &peds);
    tree->Branch("fmaxx",    &fmaxx);
    tree->Branch("fmax",     &fmax);
    tree->Branch("qsum",     &qsum);
    tree->Branch("curtime",  &curtime);
    tree->Branch("trgtime",  &trgtime);
    tree->Branch("daqtimeD", &daqtime);
    tree->Branch("daqtimeC", &daqtimeC);
    tree->Branch("flag_sat", &flag_sat);

    nent = fadc25->GetEntries();

    cout << "============================" << endl;
    cout << Form("Number of Events = %d", nent)<<endl;
    cout << "============================" << endl;
    cout << endl;
    
    sttime  = 1.e-09*ttimeD;
    sttimeD = ttimeD;

    for(int i = 0; i < nent; i++){
	evtno = i;
	if((ntotent+1)%10000 == 0) cout << Form("%dth event is processing...", ntotent+1) << endl;
	fadc25->GetEntry(i);

	daqtime = ttimeD;
	if(ttimeD < sttimeD){
	    ttimeD += 11258999068426240; // overflow in FPGA, 0x28000000000000
	}
	daqtimeC = ttimeD; // nsec
	trgtime = 1.e-09*ttimeD - sttime; // sec
	curtime = (double) evtt;

	// pedestal calculation
	pedm     = 0;
	tmprms   = 0;
	fmax     = 0;
	qsum     = 0;
	flag_sat = 0;
	for(int j = 0; j < PedRange; j++){
	    pedm   += 1.*adc[j];
	    tmprms += 1.*adc[j]*adc[j];
	}
	pedm   *= 1./PedRange;
	tmprms *= 1./PedRange;
	tmprms -= pedm*pedm;
	peds    = TMath::Sqrt(tmprms);

	for(int j = PedRange; j < DATA_POINT; j++){
	    if(adc[j] == 4095){
		flag_sat = 1;
	    }

	    fadc = 1.*adc[j] - pedm;

	    if(fmax < fadc){
		fmax  = fadc;
		fmaxx = j+1;
	    }
	    qsum += fadc;
	} 
	tree->Fill();
	ntotent++;
    }
    tree->Write();
    fout->Close();

    return 0;
}
