#ifndef Calibration_H
#define Calibration_H
#include<vector>
#include<array>
#include <fstream>
#include <nlohmann/json.hpp>
#include"log.h"
#include <string>
using json = nlohmann::json;

struct Data
{
        public:
                std::array<double,6>ADC;
                std::array<double,6>ADC_n;
                std::array<double,6>TDC;
                double TRG;
                std::array<double,3>TDC_high;
                int time =0;
                int file=0;
        Data()
        : ADC{}, ADC_n{}, TDC{}, TRG(0), TDC_high{}, time(0), file(0) {}
};
struct CalibrationValue
{
	public:
		double constant=0;
		double peak=0;
		double sigma=0;
		double peakerror=0;
		void SetFunction(const TF1 *f){
			if(f->GetNpar()!=3){
				MYLOG_ERROR("TF1 Dimention is not 3 !! "<<f->GetNdim());
			}
			constant=f->GetParameter(0);
			peak=f->GetParameter(1);
			sigma=f->GetParameter(2);
			peakerror=f->GetParError(1);

		}


	private:
		std::string class_name = "CalibrationValue";

};
std::ostream& operator<<(std::ostream& os, const CalibrationValue& val) {
    os << " { "
       << "constant: " << val.constant << ", "
       << "peak: " << val.peak << ", "
       << "sigma: " << val.sigma << ", "
       << "peakerror: " << val.peakerror
       << " }";
    return os;
}


class Calibration{
	private:
		std::string class_name = "Calibration";
	public:
		Calibration(std::string ss="240713"):date(ss){
			configpath ="/home/fermi/rshibata/config/dataconfig/datapathconfig_"+date+".json";
			std::ifstream in(configpath);
			if (!in.is_open()) {
            			MYLOG_ERROR("Failed to open config file: " << configpath);
            			throw std::runtime_error("Config file not found: " + configpath);
        		}
			in>>config;	
			type = config["type"];
    			nfile = config["file_number"];
    			upper = config["paths"]["upper"];
    			lower = config["paths"]["lower"];
			MYLOG_INFO("type is "<<type<<" date is "<<date);
			in.close();
			mode=config["mode"];
			MYLOG_INFO("mode is "<<mode);
			if(type=="shibataDATA"){
                		max_bin=15000;
				total_bin=7500;
                		max_bin_n=1000;
                		total_bin_n=2000;
				Rebin=30;
				Rebin_n=8;
				threADC=1500;
        		}else if(type=="usshiDATA"){
                		total_bin=3500;
                		max_bin=3500;
                		total_bin_n=1000;
                		max_bin_n=1000;
				Rebin=7;
                                Rebin_n=4;
				threADC=500;
        		}else{MYLOG_ERROR("type is incorrect "<<type);}
			bin_density=(max_bin-0)/total_bin;
			bin_density_n=(max_bin_n-0)/total_bin_n;
			MYLOG_INFO("total bin = "<<total_bin<<" max bin ch = "<<max_bin<<" bin_density = "<<bin_density<<" total bin (narrow) = "<<total_bin_n<<" max bin ch (narrow) = "<<max_bin_n<<" bin_density (narrow) = "<<bin_density_n);
		}
		~Calibration(){
			if (input) {
        			if (input->IsOpen()) {
           	 			input->Close();
        			}
        			delete input;
        			input = nullptr;
    			}
			if(c_fit){
				delete c_fit;
				c_fit=nullptr;
			}
		}
		void ExecuteCalibration();
		void MakeTreeRaw();
		void TH1DFitting(TH1D *h,CalibrationValue &v_511,CalibrationValue &v_1275,bool iswide);
		void MakeTH1D(std::string t="Default");//make TH1D for calibration
		void MakeFinalTree();
		void SetTimeDensity(int t){
			time_density=t;
			MYLOG_INFO("time density is setted "<<time_density<<" s !!");
		}
		void SetDEBUGMode(){
			DEBUG=true;
			MYLOG_INFO("Enter DEBUG mode in Execute Calibration this takes time ... Do you OK?");
		}

	private:
		//std::array<std::array<int, 4>, 3> arg_3coin = {{
    		//	{{1, 2, 4, 5}},
    		//	{{0, 1, 3, 4}},
		//	{{0, 2, 3, 5}}
		//}};

		void SetRawTree();
		bool DEBUG=false;
		int nfile=0;
		std::string date;
		std::string type;
		std::string upper;
		std::string lower;
		int time_density=120*60;//second
		TTree *fChain=nullptr;
		TFile *input=nullptr;
		Data d;
		std::string mode;
		json config;
		std::string configpath;
		int total_bin=0;
        	int max_bin=0;
        	int total_bin_n=0;
        	int max_bin_n=0;
		int Rebin=0;
		int Rebin_n=0;
		int bin_density=0;
		int bin_density_n=0;
		double threADC=0;
		TCanvas *c_fit=nullptr;
};
#endif
