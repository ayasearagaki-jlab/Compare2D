#ifndef Calibration_CPP
#define Calibration_CPP
#include "Calibration.h"
#include "log.h"

void Calibration::MakeTreeRaw(){
	std::string fname="/home/fermi/rshibata/Tree/raw/raw_"+date+".root";
	TFile *f=new TFile(fname.c_str(),"RECREATE");
	TTree *tree=new TTree("tree","tree");
	std::vector<std::vector<TH1D*>>hist_TDC(6,std::vector<TH1D*>(6));
	for(int PMTi=0;PMTi<6;++PMTi){
		for(int sPMTi=0;sPMTi<6;++sPMTi){
			hist_TDC.at(PMTi).at(sPMTi)=new TH1D(Form("TDC%dminusTDC%d",PMTi+1,sPMTi+1),Form("TDC%d-TDC%d",PMTi+1,sPMTi+1),2000,-1000,1000);
		}
	}
	TH1D* hist_TRG=new TH1D("TRG","TRG",2000,-1000,1000);


	int num;
	double ADC1,ADC2,ADC3,ADC4,ADC5,ADC6;
	double ADC_narrow1,ADC_narrow2,ADC_narrow3,ADC_narrow4,ADC_narrow5,ADC_narrow6;
	double TDC1,TDC2,TDC3,TDC4,TDC5,TDC6;
	double TDC7,TDC8;
	double TDC_high1,TDC_high3,TDC_high5;
	int time;
	int file_num=0;
	tree->Branch("ADC1",&ADC1);
	tree->Branch("ADC2",&ADC2);
	tree->Branch("ADC3",&ADC3);
	tree->Branch("ADC4",&ADC4);
	tree->Branch("ADC5",&ADC5);
	tree->Branch("ADC6",&ADC6);

	tree->Branch("ADC_narrow1",&ADC_narrow1);
	tree->Branch("ADC_narrow2",&ADC_narrow2);
	tree->Branch("ADC_narrow3",&ADC_narrow3);
	tree->Branch("ADC_narrow4",&ADC_narrow4);
	tree->Branch("ADC_narrow5",&ADC_narrow5);
	tree->Branch("ADC_narrow6",&ADC_narrow6);
	
	tree->Branch("TDC1",&TDC1);
	tree->Branch("TDC2",&TDC2);
	tree->Branch("TDC3",&TDC3);
	tree->Branch("TDC4",&TDC4);
	tree->Branch("TDC5",&TDC5);
	tree->Branch("TDC6",&TDC6);

	tree->Branch("TRG",&TDC7);

	tree->Branch("filenum",&file_num);
	if(type=="shibataDATA"){
		tree->Branch("TDC_high1",&TDC_high1);
		tree->Branch("TDC_high3",&TDC_high3);
		tree->Branch("TDC_high5",&TDC_high5);
		tree->Branch("time",&time);
	}

	/////////////////Reading txt file //////////
	///////////////////////////////////////////
	std::cout<<std::endl;
	for(file_num=0;file_num<nfile;file_num++){
		std::cout << "\rProcessing file " << file_num + 1 << " / " << nfile << std::flush;
		std::string fpath=upper+to_string(file_num)+lower;
		std::ifstream ifs(fpath);
		if(!ifs.is_open()) {
			MYLOG_ERROR("file can't open !! path is "<<fpath);
			continue;
		}
		std::string line;
		while (std::getline(ifs, line)) {
			if (!line.empty() && line[0] == '#') continue;
			std::istringstream iss(line);
			if(type=="shibataDATA"){
				ifs>>num>>ADC1>>ADC2>>ADC3>>ADC4>>ADC5>>ADC6>>ADC_narrow1>>ADC_narrow2
					>>ADC_narrow3>>ADC_narrow4>>ADC_narrow5>>ADC_narrow6>>TDC1>>TDC2>>
						TDC3>>TDC4>>TDC5>>TDC6>>TDC7>>TDC_high1>>TDC_high3>>TDC_high5>>time;
			}else if(type=="usshiDATA"){
				ifs>>num>>ADC1>>ADC2>>ADC3>>ADC4>>ADC5>>ADC6>>ADC_narrow1>>ADC_narrow2
					>>ADC_narrow3>>ADC_narrow4>>ADC_narrow5>>ADC_narrow6>>TDC1>>TDC2>>
						TDC3>>TDC4>>TDC5>>TDC6>>TDC7>>TDC8;
			}else{
				MYLOG_ERROR("type is broken type = "<<type);
				return;
			}
			tree->Fill();	
			hist_TRG->Fill(TDC7);

			double TDC_values[6] = {TDC1, TDC2, TDC3, TDC4, TDC5, TDC6};
			double TDC_values_high[6]={TDC_high1,0,TDC_high3,0,TDC_high5,0};
			
			for(int PMTi=0;PMTi<6;++PMTi){
                		for(int sPMTi=0;sPMTi<6;++sPMTi){
					if(type=="shibataDATA"&&((PMTi==0&&sPMTi==0)||(PMTi==2&&sPMTi==2)||(PMTi==4&&sPMTi==4))){//calcurate TDC high threshold 
						hist_TDC.at(PMTi).at(sPMTi)->Fill(TDC_values[PMTi]-TDC_values_high[sPMTi]);
					}else{
                        			hist_TDC.at(PMTi).at(sPMTi)->Fill(TDC_values[PMTi]-TDC_values[sPMTi]);
                			}
				}
        		}

		}//while loop	
	}//for loop
	tree->Write();
	hist_TRG->Write("TRG");
	int bin=hist_TRG->GetMaximumBin();
	config["TRG"]=hist_TRG->GetBinLowEdge(bin);

	json TDC_peak = json::array();
	for(int PMTi=0;PMTi<6;++PMTi){
                json row = json::array();
		for(int sPMTi=0;sPMTi<6;++sPMTi){
                        hist_TDC.at(PMTi).at(sPMTi)->Write(Form("TDC%dminusTDC%d",PMTi+1,sPMTi+1));
			bin=hist_TDC.at(PMTi).at(sPMTi)->GetMaximumBin();
			row.push_back(hist_TDC.at(PMTi).at(sPMTi)->GetBinLowEdge(bin));
                }
		TDC_peak.push_back(row);
        }
	config["TDC_peak"]=TDC_peak;

	std::ofstream ofs(configpath);
    	ofs << config.dump(4); 
    	ofs.close();

	f->Close();
	std::cout<<std::endl;
	MYLOG_INFO(fname<<" is created");
	return;
}
void Calibration::MakeTH1D(std::string t="Default"){

	mode=t;
	if(type=="usshiDATA"){
		mode="Default";
		MYLOG_INFO("in case of type = usshiDATA, calibration mode become "<<mode);
	}
	SetRawTree();
	if(!fChain){
                MYLOG_ERROR("tree is not found ");
                MYLOG_ERROR("Do you conduct MakeTreeRaw() ?")
                return;
        }
	MYLOG_INFO("calibration mode is "<<mode);
	std::string fname="/home/fermi/rshibata/Tree/TH1D/TH1D_"+date+".root";
	TFile *f_output=new TFile(fname.c_str(),"RECREATE");
	TTree *output=new TTree("output","output");
       	int start,stop;
	int file_num;
	int histnum=0;
	std::vector<std::pair<int,int>>timestamp;
	Long64_t nentries=fChain->GetEntries();
	MYLOG_INFO("tree Nentry = "<<nentries);
	if(mode=="TimeCut"){
		output->Branch("start",&start); 
        	output->Branch("stop",&stop);
		fChain->GetEntry(0);
		int firsttime=d.time;
		fChain->GetEntry(nentries-1);
		int lasttime=d.time;
		for (int t = firsttime; t < lasttime; t += time_density) {
        		int start_time = t;
        		int stop_time = std::min(t + time_density, lasttime);
        		if(stop_time-start_time<time_density){
				if(!timestamp.empty())timestamp.back().second=lasttime;
				else timestamp.emplace_back(start_time, lasttime);
				break;	
			}else{
				timestamp.emplace_back(start_time, stop_time);
    		
			}
		}

		histnum=timestamp.size();
	}else{
		output->Branch("filenum",&file_num);
		histnum=nfile;
	}
	MYLOG_INFO("size of TH1D is "<<histnum);
	std::vector<std::vector<std::pair<TH1D*, TH1D*>>> v_hist(6, std::vector<std::pair<TH1D*, TH1D*>>(histnum));
        for (int PMTi = 0; PMTi < 6; ++PMTi) {
                for (int j = 0; j < histnum; ++j) {
                        v_hist[PMTi][j].first = new TH1D(Form("h_ADC%d_bin%d", PMTi+1, j),Form("ADC%d in bin %d", PMTi+1, j),total_bin, 0,max_bin);
                	v_hist[PMTi][j].second = new TH1D(Form("h_ADC%d_n__bin%d", PMTi+1, j),Form("ADC%d_n in bin %d", PMTi+1, j),total_bin_n, 0,max_bin_n);
		}
        }


	//	MAKE TH1D for Calibration	///////////////////
	std::cout<<std::endl;
	for(Long64_t  ientry=0;ientry<nentries;ientry++){
		fChain->GetEntry(ientry);
		std::cout << "\rProcessing entry " << ientry << " / " << nentries<<" = "<<ientry*100 /nentries<< " %"<< std::flush;
		if(mode=="TimeCut"){
			int index_h=0;
			for(index_h=0;index_h<histnum;index_h++){
				if(timestamp.at(index_h).first<=d.time&&timestamp.at(index_h).second>d.time)break;
			}
			if(index_h==histnum) index_h=histnum-1;
			for (int PMTi = 0; PMTi < 6; ++PMTi) {
				v_hist[PMTi][index_h].first->Fill(d.ADC.at(PMTi));
				v_hist[PMTi][index_h].second->Fill(d.ADC_n.at(PMTi));
			}
		}else{
			for (int PMTi = 0; PMTi < 6; ++PMTi) {
				v_hist[PMTi][d.file].first->Fill(d.ADC.at(PMTi));
                        	v_hist[PMTi][d.file].second->Fill(d.ADC_n.at(PMTi));
			}
		}
	}
	std::cout<<std::endl;
	std::vector<std::pair<TH1D*, TH1D*>>b_hist(6,std::pair<TH1D*, TH1D*>());
	MYLOG_INFO("Now start save Tree");	
	for(int PMTi=0;PMTi<6;PMTi++){
		output->Branch(Form("ADC%dTH1D",PMTi+1),"TH1D",&b_hist.at(PMTi).first,3200);
		output->Branch(Form("ADC_narrow%dTH1D",PMTi+1),"TH1D",&b_hist.at(PMTi).second,3200);
	}	

	for(int index_h=0;index_h<histnum;++index_h){
		for(int PMTi=0;PMTi<6;PMTi++){
			b_hist.at(PMTi).first=v_hist[PMTi][index_h].first;
			b_hist.at(PMTi).second=v_hist[PMTi][index_h].second;
		}
		if(mode=="TimeCut"){
			start=timestamp.at(index_h).first;
			stop=timestamp.at(index_h).second;
		}else{
			file_num=index_h;
		}
		output->Fill();
	}
	
	f_output->cd();
	output->Write("tree");
	f_output->Close();

	config["mode"]=mode;
	std::ofstream ofs(configpath);
	ofs << config.dump(4);
	ofs.close();
	



}	


void Calibration::SetRawTree(){
	std::string fname="/home/fermi/rshibata/Tree/raw/raw_"+date+".root";
	if(fChain!=nullptr) {
		delete fChain;
		fChain=nullptr;
	}
	if(input){
		input->Close();
		delete input;
		input=nullptr;
	}

	input=(TFile*)gROOT->GetListOfFiles()->FindObject(fname.c_str());
	if (!input|| !input->IsOpen()) {
		input = new TFile(fname.c_str());
	}

	input->GetObject("tree",fChain);
	if(!fChain){
		MYLOG_ERROR("tree is not found at "<<fname);
		MYLOG_ERROR("Do you conduct MakeTreeRaw() ?")
		return;
	}
	for(int PMTi=0;PMTi<6;PMTi++){
		fChain->SetBranchAddress(Form("ADC%d",PMTi+1), &d.ADC.at(PMTi));
		fChain->SetBranchAddress(Form("ADC_narrow%d",PMTi+1), &d.ADC_n.at(PMTi));
		fChain->SetBranchAddress(Form("TDC%d",PMTi+1), &d.TDC.at(PMTi));
	}
	fChain->SetBranchAddress("TRG", &d.TRG);
  	fChain->SetBranchAddress("filenum", &d.file);

   	if(type=="shibataDATA"){
		fChain->SetBranchAddress("TDC_high1", &d.TDC_high.at(0));
		fChain->SetBranchAddress("TDC_high3", &d.TDC_high.at(1));
		fChain->SetBranchAddress("TDC_high5", &d.TDC_high.at(2));
   		fChain->SetBranchAddress("time", &d.time);

	}
	
}
#endif
