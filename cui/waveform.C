void waveform(int data_point){

    const double bintons = 40; //40 for 25Mhz
    double rangef = (double)data_point * bintons;
    TH1D *hhh = new TH1D("hhh", "Waveform", data_point, 0, rangef);
    
    FILE *fp = fopen("read_data.dat", "r");
    int data(0);

    for(int i = 0; i < data_point; i++){
	fscanf(fp, "%i", &data);
	hhh->Fill((i+0.5)*40, data);
    }
    fclose(fp);

    TCanvas *c1 = new TCanvas("c1", "", 800, 600);
    c1->Divide(1,1);
    c1->cd(1);
    hhh->Draw("hist");
    hhh->SetMinimum(0);
      
    return;
}
