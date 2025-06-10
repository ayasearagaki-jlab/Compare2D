//Between2D.h
#ifndef BETWEEN_H
#define BETWEEN_H
#include"log.h"
#include <array>
struct Event{
	public:
		std::array<double,6> E;
		std::array<double,6> E_narrow;
		double TRG;
		std::array<double,3> TDC_2coin;
		std::array<double,3> TDC_high;
		std::array<std::array<double,4>,3>TDC_3coin;

};
struct energy{
	public:
		double up=0;
		double low=0;
	energy(double e1,double e2){
		if(e1>e2){
			up=e1;
			low=e2;
		}else{
			up=e2;
			low=e1;
		}
	}
};



class Between2D{
	private: 
		std::string class_name="Between2D";
	public:
		Between2D(){
			std::string path_shibata=path+"shibataDATA.root";
			f.first=(TFile*)gROOT->GetListOfFiles()->FindObject(path_shibata.c_str());
			if(!f.first||!f.first->IsOpen()){
				f.first=new TFile(path_shibata.c_str());
			}
			f.first->GetObject("tree",tree.first);

			std::string path_usshi=path+"usshiDATA.root";
                        f.second=(TFile*)gROOT->GetListOfFiles()->FindObject(path_usshi.c_str());
                        if(!f.second||!f.second->IsOpen()){
                                f.second=new TFile(path_usshi.c_str());
                        }
                        f.second->GetObject("tree",tree.second);

			nentries.first=tree.first->GetEntries();
			nentries.second=tree.second->GetEntries();
			MYLOG_INFO("nentries *** shibataDATA ****** usshiDATA *******");
			MYLOG_INFO("         *** "<<nentries.first<<" ****** "<<nentries.second<<" *********");

			SetTrees();
			
		}
		~Between2D(){
			if(f.first){
				if(f.first->IsOpen()) f.first->Close();
				delete f.first;
				f.first=nullptr;
			}
			if(f.second){
				if(f.second->IsOpen()) f.second->Close();
				delete f.second;
                                f.second=nullptr;
			}
		
		}
		
		void MakeTH2D();
		void MakeReport();
		void SetConfig_TDC_2coinCut(bool on=false,double width=6){
			doTDC_2coinCut=on;
			width_TDC_2coinCut=width;
			std::stringstream ss;
			ss << std::boolalpha<< "TDC_2coinCut is " << "\033[1;32m" << doTDC_2coinCut << "\033[0m";
   			if(doTDC_2coinCut)ss<< ", Width is " << width_TDC_2coinCut << " ns";

			MYLOG_INFO(ss.str());
		}
		void SetConfig_TDC_3coinCut(bool on=false, double width=12.0) {
    			doTDC_3coinCut = on;
    			width_TDC_3coinCut = width;
    			std::stringstream ss;
    			ss << std::boolalpha << "TDC_3coinCut is \033[1;32m" << doTDC_3coinCut << "\033[0m";
       			if(doTDC_3coinCut)ss<< ", Width is " << width_TDC_3coinCut << " ns";
    			MYLOG_INFO(ss.str());
		}

		void SetConfig_Exclude_2coinCut(bool on=false, double threshold=200.0) {
    			doExclude_2coinCut = on;
    			threshold_Exclude_2coinCut = threshold;
    			std::stringstream ss;
    			ss << std::boolalpha << "Exclude_2coinCut is \033[1;32m" << doExclude_2coinCut << "\033[0m";
       			if(doExclude_2coinCut)ss<< ", Threshold is " << threshold_Exclude_2coinCut<<" keV";
    			MYLOG_INFO(ss.str());
		}

		void SetConfig_Exclude_3coinCut(bool on=false, double threshold=200.0) {
    			doExclude_3coinCut = on;
    			threshold_Exclude_3coinCut = threshold;
    			std::stringstream ss;
    			ss << std::boolalpha << "Exclude_3coinCut is \033[1;32m" << doExclude_3coinCut << "\033[0m";
       			if(doExclude_3coinCut)ss<< ", Threshold is " << threshold_Exclude_3coinCut<<" keV";
    			MYLOG_INFO(ss.str());
		}

		void SetConfig_TRG_Cut(bool on=false, double width=1.5) {
    			doTRG_Cut = on;
    			width_TRG_Cut = width;
    			std::stringstream ss;
    			ss << std::boolalpha << "TRG_Cut is \033[1;32m" << doTRG_Cut << "\033[0m";
       			if(doTRG_Cut)ss<< ", Width is " << width_TRG_Cut << " ns";
    			MYLOG_INFO(ss.str());
		}

		void SetConfig_ADC_Cut(bool on=false, double threshold=100.0) {
    			doADC_Cut = on;
    			threshold_ADC_Cut = threshold;
    			std::stringstream ss;
    			ss << std::boolalpha << "ADC_Cut is \033[1;32m" << doADC_Cut << "\033[0m";
       			if(doADC_Cut)ss<< ", Threshold is " << threshold_ADC_Cut<<" keV";
    			MYLOG_INFO(ss.str());
		}

		void SetConfig_ADC_Cut_3coin(bool on=false, double threshold=100.0) {
                        doADC_Cut_3coin = on;
                        threshold_ADC_Cut_3coin = threshold;
                        std::stringstream ss;
                        ss << std::boolalpha << "ADC_Cut_3coin is \033[1;32m" << doADC_Cut_3coin << "\033[0m";
                        if(doADC_Cut_3coin)ss<< ", Threshold is " << threshold_ADC_Cut_3coin<<" keV";
                        MYLOG_INFO(ss.str());
                }


		void SetConfig_All_ADC_Cut(bool on=false, double threshold=100.0) {
    			doAll_ADC_Cut = on;
    			threshold_All_ADC_Cut = threshold;
    			std::stringstream ss;
    			ss << std::boolalpha << "All_ADC_Cut is \033[1;32m" << doAll_ADC_Cut << "\033[0m";
       			if(doAll_ADC_Cut)ss<< ", Threshold is " << threshold_All_ADC_Cut<<" keV";
    			MYLOG_INFO(ss.str());
		}
		void SetBlind(bool i){
			Blind=i;
			std::stringstream ss;
                        ss << std::boolalpha << "Blind is \033[1m" << Blind<< "\033[0m";
                        MYLOG_INFO(ss.str());

		}

	private:
		
		void SetTrees();
		void FillData2coin();
		void FillData3coin_511();
		void FillData3coin();
		void IntegralCR();
		void IntegralVR();
		void BlindAndIntegralSR();
		void MakeTag();
		int total_bin=1500;
		int min_bin=0;
		int max_bin=1500;
		static constexpr int NCR=7;
		static constexpr int NVR=3;
		static constexpr int NSR=2;
		bool Blind=true;
		std::string Tag="_";
		std::string path="/home/fermi/rshibata/Tree/aftercalib/";
		std::string type;
		std::pair<TFile*,TFile*>f;//first = shibata second = usshi
		std::pair<TTree*,TTree*>tree;	
		std::pair<Long64_t,Long64_t>nentries;
		std::pair<Event,Event> event;
		std::pair<TH2D*,TH2D*>hist_2coin;
		std::pair<TH2D*,TH2D*>hist_3coin_511;
		std::pair<TH2D*,TH2D*>hist_3coin;
		std::pair<std::vector<double>,std::vector<double>>CR;
		std::pair<std::vector<double>,std::vector<double>>dCR;
		std::pair<std::vector<double>,std::vector<double>>VR;
                std::pair<std::vector<double>,std::vector<double>>dVR;
		std::pair<std::vector<double>,std::vector<double>>SR;
                std::pair<std::vector<double>,std::vector<double>>dSR;
		std::array<TH1D*,6>hist_1D;//for MERGEfermi!!
		//Selection
		
		bool TDC_2coinCut(int pair);
		//config of double coincidence cut
		bool doTDC_2coinCut=false;
		double width_TDC_2coinCut=6;

		bool TDC_3coinCut(int pair,std::vector<int> &num);
                //config of double coincidence cut
                bool doTDC_3coinCut=false;
                double width_TDC_3coinCut=12;
		
		bool Exclude_2coinCut(int pair);
		//config of Exclude_2coinCut
		bool doExclude_2coinCut=false;
		double threshold_Exclude_2coinCut=200.0;

		bool Exclude_3coinCut(int pair,std::vector<int> &num);
                //config of Exclude_3coinCut
                bool doExclude_3coinCut=false;
                double threshold_Exclude_3coinCut=200.0;
		
		bool TRG_Cut();
		//config of TRG_Cut
		bool doTRG_Cut=false;
		double width_TRG_Cut=1.5;

		bool ADC_Cut(int pair);
		//config of ADC_Cut
		bool doADC_Cut=false;
		double threshold_ADC_Cut=100.0;
		
		bool ADC_Cut_3coin(int pair,std::vector<int> &num);
		//config of ADC Cut 3coin
		bool doADC_Cut_3coin=false;
		double threshold_ADC_Cut_3coin=100.0;


		bool All_ADC_Cut();
		//config of ADC_Cut
                bool doAll_ADC_Cut=false;
                double threshold_All_ADC_Cut=100.0;

		bool Find3coin(int ener,std::vector<int> &num,int pair);
};
#endif

