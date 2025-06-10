//CheckADC.cpp
std::string date="180202";//FIXME
int file_num=0;//FIXME

void CheckADC() {
    std::string path = "/home/fermi/rshibata/Tree/raw/raw_" + date + ".root";
    TFile* f = new TFile(path.c_str(), "READ");
    TTree* tree = (TTree*)f->Get("tree");

    const int N_ADC = 6;
    const char* adcNames[N_ADC] = {"ADC1", "ADC2", "ADC3", "ADC4", "ADC5", "ADC6"};
    double adcValues[N_ADC];
    int file;

    for (int i = 0; i < N_ADC; ++i) {
        tree->SetBranchAddress(adcNames[i], &adcValues[i]);
    }
    tree->SetBranchAddress("filenum", &file);
    Long64_t n = tree->GetEntries();

    TH2D* hists[N_ADC];
    TCanvas* canvases[N_ADC];

    for (int i = 0; i < N_ADC; ++i) {
        canvases[i] = new TCanvas(Form("c%d", i+1), adcNames[i], 800, 600);
        hists[i] = new TH2D(Form("h%s", adcNames[i]),
                            Form("%s vs Entry", adcNames[i]),
                            20000, 0, n,
                            500, 800,4096);
    }
    int count=0;
    for (Long64_t i = 0; i < n; ++i) {
        tree->GetEntry(i);
        //if (file == file_num) {
            for (int j = 0; j < N_ADC; ++j) {
                hists[j]->Fill(i, adcValues[j]);
            }
	    count++;
        //}
    }

    for (int i = 0; i < N_ADC; ++i) {
        canvases[i]->cd();
	canvases[i]->SetLogz();
	//canvases[i]->SetLogy();
        hists[i]->GetXaxis()->SetTitle("Entry");
        hists[i]->GetYaxis()->SetTitle(adcNames[i]);
        hists[i]->Draw("COLZ");
    }
}
