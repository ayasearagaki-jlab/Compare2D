#ifndef between_cxx
#define between_cxx
#include "Between2D.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
        void Between2D::SetTrees(){
                if(!tree.first||!tree.second){
                        MYLOG_ERROR("Can't find TTree !!");
                        return;
                }
                for(int PMTi=0;PMTi<6;PMTi++){
                        tree.first->SetBranchAddress(Form("E%d",PMTi+1), &event.first.E.at(PMTi));
                        tree.first->SetBranchAddress(Form("E_narrow%d",PMTi+1), &event.first.E_narrow.at(PMTi));
                        tree.second->SetBranchAddress(Form("E%d",PMTi+1), &event.second.E.at(PMTi));
                        tree.second->SetBranchAddress(Form("E_narrow%d",PMTi+1), &event.second.E_narrow.at(PMTi));

                }
                tree.first->SetBranchAddress("diff_TRG", &event.first.TRG);
                tree.second->SetBranchAddress("diff_TRG", &event.second.TRG);
                for(int PMTi=0;PMTi<6/2;++PMTi){
                        tree.first->SetBranchAddress(Form("diff_TDC%dminusTDC%d",2*PMTi+1,(2*PMTi+3)%6+1), &event.first.TDC_2coin.at(PMTi));
                        tree.second->SetBranchAddress(Form("diff_TDC%dminusTDC%d",2*PMTi+1,(2*PMTi+3)%6+1), &event.second.TDC_2coin.at(PMTi));

                        tree.first->SetBranchAddress(Form("diff_TDC%dminusTDC%dhigh",PMTi*2+1,PMTi*2+1), &event.first.TDC_high.at(PMTi));

                        for(int j=0;j<4;j++){
                                tree.first->SetBranchAddress(Form("diff_TDC%dminusTDC%d",2*PMTi+1,arg_3coin.at(PMTi).at(j)+1), &event.first.TDC_3coin.at(PMTi).at(j));
                                tree.second->SetBranchAddress(Form("diff_TDC%dminusTDC%d",2*PMTi+1,arg_3coin.at(PMTi).at(j)+1), &event.second.TDC_3coin.at(PMTi).at(j));
                        }
                }
        
	}

	void Between2D::MakeTag(){
		Tag="_";
		std::ostringstream oss;
		if(doTDC_2coinCut)oss << "TDC_2coin_" << std::fixed << std::setprecision(1) << width_TDC_2coinCut <<"_";
		if(doTDC_3coinCut)oss << "TDC_3coin_" << std::fixed << std::setprecision(1) << width_TDC_3coinCut <<"_";
		if(doExclude_2coinCut)oss<<"Exclude_2coin_"<<std::fixed << std::setprecision(0)<<threshold_Exclude_2coinCut<<"_";
		if(doExclude_3coinCut)oss<<"Exclude_3coin_"<<std::fixed << std::setprecision(0)<<threshold_Exclude_3coinCut<<"_";
		if(doTRG_Cut)oss <<"TRG_"<<std::fixed << std::setprecision(1)<<width_TRG_Cut<<"_";
		if(doADC_Cut_3coin)oss<<"ADC_Cut_3coin_"<<std::fixed << std::setprecision(0)<<threshold_ADC_Cut_3coin<<"_";
		if(doADC_Cut)oss<<"ADC_"<<std::fixed << std::setprecision(0)<<threshold_ADC_Cut<<"_";
		if(doAll_ADC_Cut)oss<<"All_ADC_"<<std::fixed << std::setprecision(0)<<threshold_All_ADC_Cut<<"_";
		Tag+=oss.str();
	
	}
	///////////////////////////////////////////////////////////////////////////////////////
	//			MAIN 	For Loop		///////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////

	void Between2D::MakeTH2D(){
		MakeTag();
		if(hist_2coin.first){
			delete hist_2coin.first;
			hist_2coin.first=nullptr;
		}
		if(hist_2coin.second){
			delete hist_2coin.second;
			hist_2coin.second=nullptr;
		}
		if(hist_3coin_511.first){
			delete hist_3coin_511.first;
			hist_3coin_511.first=nullptr;
		}
		if(hist_3coin_511.second){
                        delete hist_3coin_511.second;
                        hist_3coin_511.second=nullptr;
                }
		if(hist_3coin.first){
                        delete hist_3coin.first;
                        hist_3coin.first=nullptr;
                }
                if(hist_3coin.second){
                        delete hist_3coin.second;
                        hist_3coin.second=nullptr;
                }


		hist_2coin.first=new TH2D("hist_2coin_shibata","2coincidence shibata",total_bin,min_bin,max_bin,total_bin,min_bin,max_bin);
		hist_2coin.second=new TH2D("hist_2coin_usshi","2coincidence usshi",total_bin,min_bin,max_bin,total_bin,min_bin,max_bin);
		
		hist_3coin_511.first=new TH2D("hist_3coin_511_shibata","3coincidence with 511 keV shibata",total_bin,min_bin,max_bin,total_bin,min_bin,max_bin);
                hist_3coin_511.second=new TH2D("hist_3coin_511_usshi","3coincidence with 511 kev usshi",total_bin,min_bin,max_bin,total_bin,min_bin,max_bin);
		
		hist_3coin.first=new TH2D("hist_3coin_shibata","3coincidence with 1275 keV shibata",total_bin,min_bin,max_bin,total_bin,min_bin,max_bin);
                hist_3coin.second=new TH2D("hist_3coin_usshi","3coincidence with 1275 kev usshi",total_bin,min_bin,max_bin,total_bin,min_bin,max_bin);


		type="shibataDATA";
		MYLOG_INFO("process "<<type);
		std::cout<<std::endl;
		for(Long64_t ientry=0;ientry<nentries.first;ientry++){
			std::cout << "\rProcessing entry " << ientry << " / " << nentries.first<<" = "<<ientry*100 /nentries.first<< " %"<< std::flush;
			tree.first->GetEntry(ientry);
			FillData2coin();
			FillData3coin_511();
			FillData3coin();
		}
		std::cout<<std::endl;
		IntegralCR();
		IntegralVR();
		BlindAndIntegralSR();
		type="usshiDATA";
		MYLOG_INFO("process "<<type);
		std::cout<<std::endl;
		for(Long64_t ientry=0;ientry<nentries.second;ientry++){
                	std::cout << "\rProcessing entry " << ientry << " / " << nentries.second<<" = "<<ientry*100 /nentries.second<< " %"<< std::flush;
			tree.second->GetEntry(ientry);
			FillData2coin();
			FillData3coin_511();
			FillData3coin();
		}
		std::cout<<std::endl;
		IntegralCR();
		IntegralVR();
		BlindAndIntegralSR();
		std::string outname="/home/fermi/rshibata/Tree/datacut/report"+Tag+".root";
		TFile* out=new TFile(outname.c_str(),"RECREATE");
		hist_2coin.first->Write();
		hist_2coin.second->Write();
		hist_3coin_511.first->Write();
		hist_3coin_511.second->Write();
		hist_3coin.first->Write();
		hist_3coin.second->Write();



	
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////

	void Between2D::FillData2coin(){
		if(!hist_2coin.first||!hist_2coin.second){
			MYLOG_ERROR("2coincidence TH2D is broken!!");
			return;
		}	
		const Event& ev = (type == "shibataDATA") ? event.first : event.second;

		for(int PMTi=0;PMTi<6/2;++PMTi){
			bool tofilled2coin=true;
			energy eee(ev.E[2*PMTi],ev.E[(2*PMTi+3)%6]);
			tofilled2coin=tofilled2coin&&TRG_Cut();
			tofilled2coin=tofilled2coin&&TDC_2coinCut(PMTi);
			tofilled2coin=tofilled2coin&&Exclude_2coinCut(PMTi);
			tofilled2coin=tofilled2coin&&ADC_Cut(PMTi);			
			tofilled2coin=tofilled2coin&&All_ADC_Cut();	
			
			if(tofilled2coin){
				if(type == "shibataDATA")hist_2coin.first->Fill(eee.up,eee.low);
				else hist_2coin.second->Fill(eee.up,eee.low);
			
			}
		}	
	
	
	}
	void Between2D::FillData3coin_511(){
		 if(!hist_3coin_511.first||!hist_3coin_511.second){
                        MYLOG_ERROR("3coincidence (511 keV) TH2D is broken!!");
                        return;
		 }
		 const Event& ev = (type == "shibataDATA") ? event.first : event.second;
		 for(int PMTi=0;PMTi<6/2;++PMTi){
                        bool tofilled3coin_511=true;
			std::vector<int> nth;
                        energy eee(ev.E[2*PMTi],ev.E[(2*PMTi+3)%6]);
                        tofilled3coin_511=tofilled3coin_511&&TRG_Cut();
                        tofilled3coin_511=tofilled3coin_511&&TDC_2coinCut(PMTi);
			tofilled3coin_511=tofilled3coin_511&&Find3coin(511,nth,PMTi);
                        tofilled3coin_511=tofilled3coin_511&&Exclude_3coinCut(PMTi,nth);
			tofilled3coin_511=tofilled3coin_511&&TDC_3coinCut(PMTi,nth);
                        tofilled3coin_511=tofilled3coin_511&&ADC_Cut(PMTi);
			tofilled3coin_511=tofilled3coin_511&&ADC_Cut_3coin(PMTi,nth);
                        tofilled3coin_511=tofilled3coin_511&&All_ADC_Cut();

                        if(tofilled3coin_511){
                                if(type == "shibataDATA")hist_3coin_511.first->Fill(eee.up,eee.low);
                                else hist_3coin_511.second->Fill(eee.up,eee.low);

                        }
                }


	}

	void Between2D::FillData3coin(){
                 if(!hist_3coin.first||!hist_3coin.second){
                        MYLOG_ERROR("3coincidence (1275 keV) TH2D is broken!!");
                        return;
                 }
                 const Event& ev = (type == "shibataDATA") ? event.first : event.second;
                 for(int PMTi=0;PMTi<6/2;++PMTi){
                        bool tofilled3coin=true;
			std::vector<int> nth;
                        energy eee(ev.E[2*PMTi],ev.E[(2*PMTi+3)%6]);
                        tofilled3coin=tofilled3coin&&TRG_Cut();
                        tofilled3coin=tofilled3coin&&TDC_2coinCut(PMTi);
                        tofilled3coin=tofilled3coin&&Find3coin(1275,nth,PMTi);
                        tofilled3coin=tofilled3coin&&Exclude_3coinCut(PMTi,nth);
                        tofilled3coin=tofilled3coin&&TDC_3coinCut(PMTi,nth);
                        tofilled3coin=tofilled3coin&&ADC_Cut(PMTi);
			tofilled3coin=tofilled3coin&&ADC_Cut_3coin(PMTi,nth);
                        tofilled3coin=tofilled3coin&&All_ADC_Cut();

                        if(tofilled3coin){
                                if(type == "shibataDATA")hist_3coin.first->Fill(eee.up,eee.low);
                                else hist_3coin.second->Fill(eee.up,eee.low);

                        }
                }


        }



	/////////////////////////////////////////////////////////////////////////
	//		DATA 		SELECTION	////////////////////////
	///////////////////////////////////////////////////////////////////////
	

	bool Between2D::Find3coin(int ener,std::vector<int> &num,int pair){
		double thre_up,thre_low;
		num.clear();
		if(ener==511){
			thre_up=600;
			thre_low=450;
		}else if(ener==1275){
			thre_up=1390;
                        thre_low=1200;
		}else{
			MYLOG_ERROR("Invalid Input value!!");
			return false;
		}
		
		const Event& ev = (type == "shibataDATA") ? event.first : event.second;

		bool OK=false;
		for(int j=0;j<4;++j){
			if(ev.E[arg_3coin.at(pair).at(j)]>thre_low&&ev.E[arg_3coin.at(pair).at(j)]<thre_up){
                                OK=true;
				num.push_back(j);
			}
			
		}
		
		return OK;
	
	
	}

	bool Between2D::TDC_2coinCut(int pair){
		if(!doTDC_2coinCut) return true;
		
		const Event& ev = (type == "shibataDATA") ? event.first : event.second;

		return TMath::Abs(ev.TDC_2coin.at(pair))<=width_TDC_2coinCut;
	}
	
	bool Between2D::TDC_3coinCut(int pair,std::vector<int> &num){
                if(!doTDC_3coinCut) return true;
		if(num.size()==0)return false;
		const Event& ev = (type == "shibataDATA") ? event.first : event.second;
		bool OK=false;
		for(int i=0;i<num.size();++i){
			if(num.at(i)<0||num.at(i)>=4) continue;
			if(TMath::Abs(ev.TDC_3coin.at(pair).at(num.at(i)))<=width_TDC_3coinCut){
				OK=true;
				
			}else{
				num.at(i)=-1;	
			}

		}

		
                return OK;
        }

	bool Between2D::Exclude_2coinCut(int pair){
		if(!doExclude_2coinCut)return true;

		const Event& ev = (type == "shibataDATA") ? event.first : event.second;
		bool OK=true;
		for(int j=0;j<4;++j){
			if(ev.E[arg_3coin.at(pair).at(j)]>threshold_Exclude_2coinCut){
				OK=false;
				break;
			}
		}
		return OK;
	}
	
	bool Between2D::Exclude_3coinCut(int pair,std::vector<int> &num){
                if(!doExclude_3coinCut)return true;
		if(num.size()==0)return false;
                const Event& ev = (type == "shibataDATA") ? event.first : event.second;
                bool OK=false;
                for(int i=0;i<num.size();++i){
			bool exclude3coin=true;
			if(num.at(i)<0||num.at(i)>=4) continue;
			for(int j=0;j<4;++j){
				if(j==num.at(i))continue;
                        	if(ev.E[arg_3coin.at(pair).at(j)]>threshold_Exclude_2coinCut){
                                	exclude3coin=false;
					break;
                        	}
                	}
			if(exclude3coin){
				OK=true;
			}else{
				num.at(i)=-1;
			}
		}
		return OK;
		
                
        }


	bool Between2D::TRG_Cut(){
		if(!doTRG_Cut)return true;

		const Event& ev = (type == "shibataDATA") ? event.first : event.second;
		return TMath::Abs(ev.TRG) <= width_TRG_Cut;	
	}

	bool Between2D::ADC_Cut(int pair){
		if(!doADC_Cut) return true;

		const Event& ev = (type == "shibataDATA") ? event.first : event.second;
		return (TMath::Abs(ev.E.at(2*pair)-ev.E_narrow.at(2*pair))<=threshold_ADC_Cut)&&(TMath::Abs(ev.E.at((2*pair+3)%6)-ev.E_narrow.at((2*pair+3)%6))<=threshold_ADC_Cut);
	}
	
	bool Between2D::ADC_Cut_3coin(int pair,std::vector<int> &num){
		if(!doADC_Cut_3coin)return true;
		if(num.size()==0)return false;
		const Event& ev = (type == "shibataDATA") ? event.first : event.second;
		bool OK=false;
                for(int i=0;i<num.size();++i){
                        if(num.at(i)<0||num.at(i)>=4) continue;
                        if(TMath::Abs(ev.E.at(arg_3coin.at(pair).at(num.at(i)))-ev.E_narrow.at(arg_3coin.at(pair).at(num.at(i))))<=threshold_ADC_Cut_3coin){
                                OK=true;
                        }else{
				num.at(i)=-1;
			}

                }


                return OK;
	}


	bool Between2D::All_ADC_Cut(){
		if(!doAll_ADC_Cut) return true;

		const Event& ev = (type == "shibataDATA") ? event.first : event.second;
		bool OK=true;
		for(int PMTi=0;PMTi<6;++PMTi){
			if(TMath::Abs(ev.E.at(PMTi)-ev.E_narrow.at(PMTi))>threshold_All_ADC_Cut){
				OK=false;
				break;
			}
		}
		return OK;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	void Between2D::IntegralCR(){
		const TH2D *h;
		const TH2D *h_511;
		if(type == "shibataDATA"){
			h=hist_2coin.first;
			h_511=hist_3coin_511.first;
		}else{
			h=hist_2coin.second;
                        h_511=hist_3coin_511.second;
		}
		std::vector<double> &cr= (type == "shibataDATA") ? CR.first : CR.second;
		std::vector<double> &dcr= (type == "shibataDATA") ? dCR.first : dCR.second;
		double energy_xup[NCR]={600.0,1390.0,1390.0,1499.0,600.0,1390.0,1499.0};
    		double energy_xlow[NCR]={450.0,1200.0,1200.0,200.0,450.0,1200.0,200.0};
    		double energy_yup[NCR]={600.0,1390.0,600.0,1499.0,600.0,600.0,1499.0};
    		double energy_ylow[NCR]={450.0,1200.0,450.0,200.0,450.0,450.0,200.0};
	
	

    		int CR_xup[NCR];
    		int CR_xlow[NCR];
    		int CR_yup[NCR];
    		int CR_ylow[NCR];
		for(int i=0;i<4;i++){
    			CR_xup[i]=h->GetXaxis()->FindBin(energy_xup[i]);
        		CR_xlow[i]=h->GetXaxis()->FindBin(energy_xlow[i]);
       			CR_yup[i]=h->GetYaxis()->FindBin(energy_yup[i]);
     			CR_ylow[i]=h->GetYaxis()->FindBin(energy_ylow[i]);
		}
    		for(int i=4;i<NCR;i++){
    			CR_xup[i]=h_511->GetXaxis()->FindBin(energy_xup[i]);
     			CR_xlow[i]=h_511->GetXaxis()->FindBin(energy_xlow[i]);
      			CR_yup[i]=h_511->GetYaxis()->FindBin(energy_yup[i]);
      			CR_ylow[i]=h_511->GetYaxis()->FindBin(energy_ylow[i]);
		}
		std::ostringstream msg;
		msg<<"Caluculating "<<type<<" CR..."<<std::endl;
		msg<<"/////////////////////////////////"<<std::endl<<"CR_i\tCounting\t+-Errors"<<std::endl;


		for(int i=0;i<4;i++){
			double value,error;
         		value=h->IntegralAndError(CR_xlow[i],CR_xup[i],CR_ylow[i],CR_yup[i],error);
			cr.push_back(value);
           		dcr.push_back(error);
            		msg<<"CR_"<<i<<"\t"<<right<<setw(9)<<cr.at(i)<<"\t+-"<<dcr.at(i)<<std::endl;
     		}
		for(int i=4;i<NCR;i++){
			double value,error;
			value=h_511->IntegralAndError(CR_xlow[i],CR_xup[i],CR_ylow[i],CR_yup[i],error);
			cr.push_back(value);
            		dcr.push_back(error);
            		msg<<"CR_"<<i<<"\t"<<right<<setw(9)<<cr.at(i)<<"\t+-"<<dcr.at(i)<<std::endl;
      		}
		msg<<"/////////////////////////////////"<<std::endl;
		MYLOG_INFO(msg.str());
		MYLOG_INFO("End Caluculating "<<type<<" CR"<<std::endl);


	}

	void Between2D::IntegralVR(){
		TH2D *h=(type == "shibataDATA") ? hist_3coin.first : hist_3coin.second;
		std::vector<double> &vr= (type == "shibataDATA") ? VR.first : VR.second;
                std::vector<double> &dvr= (type == "shibataDATA") ? dVR.first : dVR.second;
	
		double energy_xup[NVR]={600.0,1390.0,1499};
   		double energy_xlow[NVR]={450.0,1200.0,200};
    		double energy_yup[NVR]={600.0,600.0,1499};
    		double energy_ylow[NVR]={450.0,450.0,200};
	
		int VR_xup[NVR];
    		int VR_xlow[NVR]; 
    		int VR_yup[NVR];
    		int VR_ylow[NVR];

		for(int i=0;i<NVR;i++){
                	VR_xup[i]=h->GetXaxis()->FindBin(energy_xup[i]);
                	VR_xlow[i]=h->GetXaxis()->FindBin(energy_xlow[i]);
        
                	VR_yup[i]=h->GetYaxis()->FindBin(energy_yup[i]);
                	VR_ylow[i]=h->GetYaxis()->FindBin(energy_ylow[i]);
  		}
		std::ostringstream msg;
		msg<<"Caluculating "<<type<<" VR..."<<std::endl;
		msg<<"/////////////////////////////////"<<std::endl<<"VR_i\tCounting\t+-Errors"<<std::endl;


		for(int i=0;i<NVR;i++){
			double value,error;
        		value=h->IntegralAndError(VR_xlow[i],VR_xup[i],VR_ylow[i],VR_yup[i],error);
			vr.push_back(value);
        		dvr.push_back(error);
        		msg<<"VR_"<<i<<"\t"<<right<<setw(9)<<vr.at(i)<<"\t+-"<<dvr.at(i)<<std::endl;
     		}
	 	MYLOG_INFO(msg.str());

	}
	void Between2D::BlindAndIntegralSR(){
		TH2D *h=(type == "shibataDATA") ? hist_3coin.first : hist_3coin.second;
		std::vector<double> &sr= (type == "shibataDATA") ? SR.first : SR.second;
                std::vector<double> &dsr= (type == "shibataDATA") ? dSR.first : dSR.second;
		double energy_xup[NSR]={1200.0,1499.0};
    		double energy_xlow[NSR]={600.0,1390.0};
    		double energy_yup[NSR]={1200.0,1499.0};
    		double energy_ylow[NSR]={600.0,600.0};
		
		int SR_xup[NSR];
    		int SR_xlow[NSR];
    		int SR_yup[NSR];
    		int SR_ylow[NSR];
		for(int i=0;i<NSR;i++){
			SR_xup[i]=h->GetXaxis()->FindBin(energy_xup[i]);
        		SR_xlow[i]=h->GetXaxis()->FindBin(energy_xlow[i]);

        		SR_yup[i]=h->GetYaxis()->FindBin(energy_yup[i]);
        		SR_ylow[i]=h->GetYaxis()->FindBin(energy_ylow[i]);
  		}

		if(Blind){
			MYLOG_INFO("Make Blind");
			for(int i=0;i<NSR;i++){
				for (int ix = SR_xlow[i]; ix <= SR_xlow[i]; ++ix) {
    				for (int iy = SR_xlow[i]; iy <= SR_yup[i]; ++iy) {
        				h->SetBinContent(ix, iy, 0);
    			}}}
		}
		
		std::ostringstream msg;
		msg<<"Caluculating "<<type<<"  SR..."<<std::endl;
		msg<<"/////////////////////////////////"<<std::endl<<"SR_i\tCounting\t+-Errors"<<std::endl;


		for(int i=0;i<NSR;i++){
			double value,error;
        		value=h->IntegralAndError(SR_xlow[i],SR_xup[i],SR_ylow[i],SR_yup[i],error);
			sr.push_back(value);
			dsr.push_back(error);
			msg<<"SR_"<<i<<"\t"<<right<<setw(9)<<sr.at(i)<<"\t+-"<<dsr.at(i)<<std::endl;
		}
     	
		MYLOG_INFO(msg.str());



	}
	void Between2D::MakeReport(){
		MakeTag();
		const std::string filename = "/home/fermi/rshibata/Tree/datacut/report"+Tag+".csv";
		std::ofstream fout(filename);
    		if (!fout.is_open()) {
        		MYLOG_ERROR("Failed to open file: " << filename);
        		return;
    		}
	
		fout << "# Region_i,shibataDATA,usshiDATA,ratio,d_shibata,d_usshi\n";
		auto dump = [&](const std::string& region,
                    	const std::pair<std::vector<double>, std::vector<double>>& main,
                    	const std::pair<std::vector<double>, std::vector<double>>& err) {
        		size_t n = main.first.size();
        		for (size_t i = 0; i < n; ++i) {
            			double s = main.first[i];
            			double u = main.second[i];
            			double ratio = (u!= 0) ? s / u : 0;
            			double ds = (i < err.first.size()) ? err.first[i] : 0;
            			double du = (i < err.second.size()) ? err.second[i] : 0;
            
				fout << region << i << "," << s << "," << u << "," << ratio << "," << ds << "," << du << "\n";
        		}
    		};
		
		dump("CR", CR, dCR);
    		dump("VR", VR, dVR);
    		dump("SR", SR, dSR);
		
		fout << "\n# TotalEntries,usshiDATA,shibataDATA\n";
    		fout << "nentries," << nentries.first << "," << nentries.second << ","<<(double)nentries.first/nentries.second<<"\n";

    		fout.close();
		MYLOG_INFO("Comparison result saved to: " << filename);
	}

#endif
