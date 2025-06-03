#ifndef Calibration_CPP2
#define Calibration_CPP2
#include "Calibration.h"
#include "log.h"
#include <TSpectrum.h>
///////////////////////////////////////////////////////////////////
//////////////Execute Calibration////////////////////////////////
/////////////////////////////////////////////////////////////////

void Calibration::ExecuteCalibration(){
        if(mode=="Not"){
                MYLOG_ERROR("calibration mode is "<<mode<<" ,so conduct MakeTH1D!!");
                return;
        }else if(mode=="TimeCut_Done"||mode=="Default_Done"){
		std::string yn_2="p";
        	while(yn_2!="y"||yn_2!="n"){
                	MYLOG_ERROR("calibration mode is "<<mode<<" ,calibration is done so try again? (y/n)");
			std::cout<<"<<";
                	std::cin >> yn_2;
                	if(yn_2=="y"){
                        	mode = mode.substr(0, mode.size() - 5);
                        	MYLOG_INFO("OK !! updated config mode is "<<mode);
                	}else if(yn_2=="n"){
                        	MYLOG_INFO("OK !! end this proces");
                        	return;
                	}else{
                        	MYLOG_INFO("Invalid input. Please type 'y' or 'n'.");
                	}
        	}

	}
        MYLOG_INFO("calibration mode is "<<mode);
        std::string fname="/home/fermi/rshibata/Tree/TH1D/TH1D_"+date+".root";
        //////////////////////Read objects from MakeTH1D()////////////////////////
	TFile *histfile=new TFile(fname.c_str(),"UPDATE");
        TTree *histtree=nullptr;
        histfile->GetObject("tree",histtree);
        std::vector<std::pair<TH1D*,TH1D*>> v_hist(6,std::pair<TH1D*,TH1D*>());
	Long64_t nentries=histtree->GetEntries();
        MYLOG_INFO("calibration round is "<<nentries);
        for(int PMTi=0;PMTi<6;++PMTi){
		histtree->SetBranchAddress(Form("ADC%dTH1D",PMTi+1),&v_hist.at(PMTi).first);
		histtree->SetBranchAddress(Form("ADC_narrow%dTH1D",PMTi+1),&v_hist.at(PMTi).second);
	}
	int start,stop,file_num;
	if(mode=="TimeCut"){
		histtree->SetBranchAddress("start",&start);
		histtree->SetBranchAddress("stop",&stop);
	}else{
		histtree->SetBranchAddress("filenum",&file_num);
	}
	///////////////////////Make Output TTree////////////////////////////////////////////////
	
        TTree *coefficient=new TTree("coefficient","coefficient");
	std::vector<std::pair<double,double>>slope(6,std::pair<double,double>());
	std::vector<std::pair<double,double>>segment(6,std::pair<double,double>());	
	for(int PMTi=0;PMTi<6;PMTi++){
		coefficient->Branch(Form("ADC%dslope",PMTi+1),&slope.at(PMTi).first);
		coefficient->Branch(Form("ADC%dsegment",PMTi+1),&segment.at(PMTi).first);
		coefficient->Branch(Form("ADC_narrow%dslope",PMTi+1),&slope.at(PMTi).second);
		coefficient->Branch(Form("ADC_narrow%dsegment",PMTi+1),&segment.at(PMTi).second);
	}

	if(mode=="TimeCut"){
		coefficient->Branch("start",&start);
                coefficient->Branch("stop",&stop);
	}else{
		coefficient->Branch("filenum",&file_num);
	}
	///////////////////////////////////////////////////////////////////////////////////////
	
	if(DEBUG)c_fit = new TCanvas("c_fit", "Fitting", 800, 600);
		
	std::vector<TGraphErrors*> gr511_wide(6), gr1275_wide(6);
	std::vector<TGraphErrors*> gr511_narrow(6), gr1275_narrow(6);

	for (int PMTi = 0; PMTi < 6; ++PMTi) {
    		gr511_wide[PMTi] = new TGraphErrors();
    		gr1275_wide[PMTi] = new TGraphErrors();
    		gr511_narrow[PMTi] = new TGraphErrors();
    		gr1275_narrow[PMTi] = new TGraphErrors();

    		gr511_wide[PMTi]->SetTitle(Form("PMT%d 511keV wide", PMTi+1));
    		gr1275_wide[PMTi]->SetTitle(Form("PMT%d 1275keV wide", PMTi+1));
    		gr511_narrow[PMTi]->SetTitle(Form("PMT%d 511keV narrow", PMTi+1));
    		gr1275_narrow[PMTi]->SetTitle(Form("PMT%d 1275keV narrow", PMTi+1));
	}

	total_bin/=Rebin;
	total_bin_n/=Rebin_n;
	bin_density=(max_bin-0)/total_bin;
	bin_density_n=(max_bin_n-0)/total_bin_n;
	MYLOG_INFO("In this Calibration Rebin total bin = "<<total_bin<<" bin_density = "<<bin_density<<" total bin (narrow) = "<<total_bin_n<<" bin_density (narrow) = "<<bin_density_n);
	/////////////////////////////////////////////////////////
	//	Execute Calibration ////////////////////////////
	///////////////////////////////////////////////////////
	for(Long64_t  ientry=0;ientry<nentries;ientry++){
		histtree->GetEntry(ientry);
		for(int PMTi=0;PMTi<6;++PMTi){
			v_hist.at(PMTi).first->Rebin(Rebin);
			
			CalibrationValue value_1275;	
			CalibrationValue value_511;//first value of fitting
			MYLOG_INFO("PMT"<<PMTi<<" round = "<<ientry<<" start Fitting !!");

			TH1DFitting(v_hist.at(PMTi).first,value_511,value_1275,true);
			slope.at(PMTi).first=(1274.5-511)/(value_1275.peak-value_511.peak);
			segment.at(PMTi).first=511-value_511.peak*slope.at(PMTi).first;	
			
			gr511_wide[PMTi]->SetPoint(ientry,ientry,value_511.peak);
			gr1275_wide[PMTi]->SetPoint(ientry,ientry,value_1275.peak);
			gr511_wide[PMTi]->SetPointError(ientry, 0, value_511.peakerror);  // error_y
        		gr1275_wide[PMTi]->SetPointError(ientry, 0, value_1275.peakerror);

			v_hist.at(PMTi).second->Rebin(Rebin_n);

			CalibrationValue value_1275_n;
                        CalibrationValue value_511_n;//first value of fitting
			
			MYLOG_INFO("PMT"<<PMTi<<" (narrow) round = "<<ientry<<" start Fitting !!");
			TH1DFitting(v_hist.at(PMTi).second,value_511_n,value_1275_n,false);
			slope.at(PMTi).second=(1274.5-511)/(value_1275_n.peak-value_511_n.peak);
                        segment.at(PMTi).second=511-value_511_n.peak*slope.at(PMTi).second;

			gr511_narrow[PMTi]->SetPoint(ientry,ientry,value_511_n.peak);
                        gr1275_narrow[PMTi]->SetPoint(ientry,ientry,value_1275_n.peak);
			gr511_narrow[PMTi]->SetPointError(ientry, 0, value_511_n.peakerror);  // error_y
                        gr1275_narrow[PMTi]->SetPointError(ientry, 0, value_1275_n.peakerror);


		}	
		coefficient->Fill();
	}

	TCanvas *c1_511_wide = new TCanvas("c1_511_wide", "511 keV - Wide", 1800, 1200);
	c1_511_wide->Divide(3, 2);
	for (int i = 0; i < 6; ++i) {
    		c1_511_wide->cd(i + 1);
    		gr511_wide[i]->SetMarkerStyle(20);
    		gr511_wide[i]->SetLineColor(kBlue);
    		gr511_wide[i]->Draw("AP");
	}
	std::string pngname="/home/fermi/rshibata/png/511_wide_graphs_"+date+".png";
	c1_511_wide->SaveAs(pngname.c_str());

	// 1275 keV wide
	TCanvas *c2_1275_wide = new TCanvas("c2_1275_wide", "1275 keV - Wide", 1800, 1200);
	c2_1275_wide->Divide(3, 2);
	for (int i = 0; i < 6; ++i) {
    		c2_1275_wide->cd(i + 1);
    		gr1275_wide[i]->SetMarkerStyle(21);
   	 	gr1275_wide[i]->SetLineColor(kRed);
    		gr1275_wide[i]->Draw("AP");
	}
	pngname="/home/fermi/rshibata/png/1275_wide_graphs_"+date+".png";
	c2_1275_wide->SaveAs(pngname.c_str());

	// 511 keV narrow
	TCanvas *c3_511_narrow = new TCanvas("c3_511_narrow", "511 keV - Narrow", 1800, 1200);
	c3_511_narrow->Divide(3, 2);
	for (int i = 0; i < 6; ++i) {
    		c3_511_narrow->cd(i + 1);
    		gr511_narrow[i]->SetMarkerStyle(20);
    		gr511_narrow[i]->SetLineColor(kBlue);
    		gr511_narrow[i]->Draw("AP");
	}
	pngname="/home/fermi/rshibata/png/511_narrow_graphs_"+date+".png";
	c3_511_narrow->SaveAs(pngname.c_str());

	// 1275 keV narrow
	TCanvas *c4_1275_narrow = new TCanvas("c4_1275_narrow", "1275 keV - Narrow", 1800, 1200);
	c4_1275_narrow->Divide(3, 2);
	for (int i = 0; i < 6; ++i) {
    		c4_1275_narrow->cd(i + 1);
    		gr1275_narrow[i]->SetMarkerStyle(21);
    		gr1275_narrow[i]->SetLineColor(kRed);
    		gr1275_narrow[i]->Draw("AP");
	}
	pngname="/home/fermi/rshibata/png/1275_narrow_graphs_"+date+".png";
	c4_1275_narrow->SaveAs(pngname.c_str());


	coefficient->Write("coefficient");
	histfile->Close();
	std::string yn="p";
	while(yn!="y"||yn!="n"){
		MYLOG_INFO("Do you want to update config become Done ? (y/n): ");
		std::cout<<"<<";
		std::cin >> yn;
		if(yn=="y"){
			mode+="_Done";
			config["mode"]=mode;
			MYLOG_INFO("OK !! updated config mode is "<<mode);
			std::ofstream ofs(configpath);
			ofs << config.dump(4);
			ofs.close();
			return;
		}else if(yn=="n"){
			MYLOG_INFO("OK !! mode is "<<mode);
			return;
		}else{
			MYLOG_INFO("Invalid input. Please type 'y' or 'n'.");
		}
	}



}
/////////////////////////////////////////////////////////
//      Execute Fitting ////////////////////////////
///////////////////////////////////////////////////////

void Calibration::TH1DFitting(TH1D *h,CalibrationValue &v_511,CalibrationValue &v_1275,bool iswide){
	if(!c_fit&&DEBUG){
		c_fit = new TCanvas("c_fit", "Fitting", 800, 600);
	}
	if(DEBUG)c_fit->cd();
	double max_value;
	if(iswide) {
		max_value=max_bin;
	}else{max_value=max_bin_n;}
	if(DEBUG)gSystem->ProcessEvents();
	if(DEBUG)gPad->Update();
	h->GetXaxis()->SetRangeUser(0,max_value*0.2);
	TSpectrum *s = new TSpectrum(1);
	int nPeaks = s->Search(h, 1, "",0.8);
	double *xPeaks = s->GetPositionX();
	MYLOG_INFO("Search npeaks is pedesutal peak is "<<xPeaks[0]);
	h->GetXaxis()->SetRangeUser(xPeaks[0]+max_value*0.10,max_value);
	TSpectrum *s2 = new TSpectrum(1);
	nPeaks = s2->Search(h, 1, "",0.8);
	xPeaks = s2->GetPositionX();
	MYLOG_INFO("Search npeaks is 511 peak is "<<xPeaks[0]);
	
	//calcurate firstvalue/////////////////////////////////////////
	v_511.peak=xPeaks[0];
	int bin_peak=h->FindBin(v_511.peak);
	v_511.constant=h->GetBinContent(bin_peak);
	int FWHM_high_511 = h->FindLastBinAbove(v_511.constant/2,bin_peak);
	int FWHM_511=2*(FWHM_high_511-bin_peak);

	if(iswide) {
		v_511.sigma=(FWHM_511/sqrt(8.0*log(2.0)))*bin_density;
	}else{
		v_511.sigma=(FWHM_511/sqrt(8.0*log(2.0)))*bin_density_n;
	}
	////////////////////////////////////////////////////////////

	MYLOG_INFO(v_511);

	TF1* func=new TF1("func511","gaus(0)",0,max_value);
	func->SetParameters(v_511.constant,v_511.peak,v_511.sigma);
	func->SetLineColor(kRed);

	h->Fit(func,"L","same",v_511.peak-1.5*v_511.sigma,v_511.peak+2.0*v_511.sigma);
	v_511.SetFunction(func);
	h->Fit(func,"L","same",v_511.peak-1.5*v_511.sigma,v_511.peak+1.5*v_511.sigma);
	v_511.SetFunction(func);
	h->Fit(func,"+L","same",v_511.peak-1.5*v_511.sigma,v_511.peak+1.5*v_511.sigma);
	v_511.SetFunction(func);
	
	//h->GetXaxis()->SetRangeUser(v_511.peak-2.0*v_511.sigma,max_value);
	if(DEBUG)h->Draw();
	if(DEBUG)gPad->Update();

	double Chi_511=func->GetChisquare()/func->GetNDF();
        MYLOG_INFO("chi2/dof(511_3) = "<<Chi_511);
	int bin_up=h->FindBin(v_511.peak+2.0*v_511.sigma);
	int bin_low=h->FindBin(v_511.peak-2.0*v_511.sigma);
	double N_511=h->Integral(bin_low,bin_up)/0.9594;
	double standard_error_511=v_511.sigma/sqrt(N_511);//標準誤差を計算します
	double ratio_err_511=standard_error_511/func->GetParError(1);
	MYLOG_INFO("standard error/Fiterror(511) = "<<ratio_err_511);
			
	//calcurate firstvalue/////////////////////////////////////////
	h->GetXaxis()->SetRangeUser(v_511.peak+18.0*v_511.sigma,max_value);
	int bin_location_maximum_1275=h->GetMaximumBin();
	v_1275.constant=h->GetMaximum();
	 if(iswide) {
		 v_1275.peak=bin_location_maximum_1275*bin_density+0;
	 }else{
	 	v_1275.peak=bin_location_maximum_1275*bin_density_n+0;
	 }
	int FWHM_high_1275=h->FindLastBinAbove(v_1275.constant/2,1,bin_location_maximum_1275);
        int FWHM_1275=2*(FWHM_high_1275-bin_location_maximum_1275);
	if(iswide) {
        	v_1275.sigma=(FWHM_1275/sqrt(8.0*log(2.0)))*bin_density;
	}else{
		v_1275.sigma=(FWHM_1275/sqrt(8.0*log(2.0)))*bin_density_n;
	}
	h->GetXaxis()->SetRangeUser(v_511.peak-2.0*v_511.sigma,max_value);
	//////////////////////////////////////////////////////////////////
			
	MYLOG_INFO(v_1275);

	TF1* func_1275=new TF1("func1275","gaus(0)",0,max_value);
        func_1275->SetParameters(v_1275.constant,v_1275.peak,v_1275.sigma);
	func_1275->SetLineColor(kBlue);
	h->Fit(func_1275,"L","same",v_1275.peak-1.5*v_1275.sigma,v_1275.peak+1.5*v_1275.sigma);
	v_1275.SetFunction(func_1275);
	h->Fit(func_1275,"L","same",v_1275.peak-1.5*v_1275.sigma,v_1275.peak+1.5*v_1275.sigma);
	v_1275.SetFunction(func_1275);
	h->Fit(func_1275,"+L","same",v_1275.peak-1.5*v_1275.sigma,v_1275.peak+1.5*v_1275.sigma);
	v_1275.SetFunction(func_1275);
	if(DEBUG)gPad->Update();


	double Chi_1275=func_1275->GetChisquare()/func_1275->GetNDF();
        MYLOG_INFO("chi2/dof(1275_3) = "<<Chi_1275);
        bin_up=h->FindBin(v_1275.peak+2.0*v_1275.sigma);
        bin_low =h->FindBin(v_1275.peak-2.0*v_1275.sigma);
        double N_1275=h->Integral(bin_low,bin_up)/0.9594;
        double standard_error_1275=v_1275.sigma/sqrt(N_1275);//標準誤差を計算します
        double ratio_err_1275=standard_error_1275/func_1275->GetParError(1);
        MYLOG_INFO("standard error/Fiterror(1275) = "<<ratio_err_1275);
	

}

#endif
