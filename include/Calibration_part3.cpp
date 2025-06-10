#ifndef Calibration_CPP3
#define Calibration_CPP3
#include "Calibration.h"
#include "log.h"
#include <TSpectrum.h>
void Calibration::MakeFinalTree(){
	if(mode!="TimeCut_Done"&&mode!="Default_Done"){
		MYLOG_ERROR("mode is "<<mode<<" please do ExecuteCalibration !!");
		return;
	}
	
	std::string fname="/home/fermi/rshibata/Tree/TH1D/TH1D_"+date+".root";
//////////////////////Read objects from MakeTH1D()////////////////////////
        TFile *histfile=new TFile(fname.c_str(),"READ");
        TTree *coefficient=nullptr;
        histfile->GetObject("coefficient",coefficient);
        
	int histnum=coefficient->GetEntries();

	std::vector<std::pair<double,double>>slope(6,std::pair<double,double>());
        std::vector<std::pair<double,double>>segment(6,std::pair<double,double>());

        for(int PMTi=0;PMTi<6;PMTi++){
                coefficient->SetBranchAddress(Form("ADC%dslope",PMTi+1),&slope.at(PMTi).first);
                coefficient->SetBranchAddress(Form("ADC%dsegment",PMTi+1),&segment.at(PMTi).first);
                coefficient->SetBranchAddress(Form("ADC_narrow%dslope",PMTi+1),&slope.at(PMTi).second);
                coefficient->SetBranchAddress(Form("ADC_narrow%dsegment",PMTi+1),&segment.at(PMTi).second);
        }

	int start,stop,file_num;
	std::vector<std::pair<int,int>>timestamp;
	if(mode=="TimeCut_Done"){
                coefficient->SetBranchAddress("start",&start);
                coefficient->SetBranchAddress("stop",&stop);
		for(int ientry=0;ientry<histnum;++ientry){
			coefficient->GetEntry(ientry);
			timestamp.emplace_back(start,stop);
		}
        }else{
                coefficient->SetBranchAddress("filenum",&file_num);
        }
	SetRawTree();
        
	if(!fChain){
                MYLOG_ERROR("tree is not found ");
                MYLOG_ERROR("Do you conduct MakeTreeRaw() ?")
                return;
        }
	std::vector<std::vector<double>> TDC_peak = config["TDC_peak"].get<std::vector<std::vector<double>>>();
	double TRG_peak=config["TRG"];
	///////////////////////Make Output TTree////////////////////////////////////////////////
	std::string outname="/home/fermi/rshibata/Tree/aftercalib/"+type+"/calib_"+date+".root";
	TFile *out=new TFile(outname.c_str(),"RECREATE");
	TTree *outtree=new TTree("calibtree","calibtree");
	std::array<double,6> E;
	std::array<double,6> E_narrow;
	double diff_TRG;
	std::array<double,3> diff_TDC_2coin;
	std::array<double,3> diff_TDC_high;
	std::array<std::array<double,4>,3>diff_TDC_3coin;
	for(int PMTi=0;PMTi<6;PMTi++){
		outtree->Branch(Form("E%d",PMTi+1), &E.at(PMTi));
		outtree->Branch(Form("E_narrow%d",PMTi+1), &E_narrow.at(PMTi));
	}
	outtree->Branch("diff_TRG", &diff_TRG);
	for(int PMTi=0;PMTi<6/2;++PMTi){
		outtree->Branch(Form("diff_TDC%dminusTDC%d",2*PMTi+1,(2*PMTi+3)%6+1), &diff_TDC_2coin.at(PMTi));
		if(type=="shibataDATA")outtree->Branch(Form("diff_TDC%dminusTDC%dhigh",PMTi*2+1,PMTi*2+1), &diff_TDC_high.at(PMTi));
		for(int j=0;j<4;j++){
			outtree->Branch(Form("diff_TDC%dminusTDC%d",2*PMTi+1,arg_3coin.at(PMTi).at(j)+1), &diff_TDC_3coin.at(PMTi).at(j));
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////
	Long64_t nentries=fChain->GetEntries();
        MYLOG_INFO("tree Nentry = "<<nentries);
	std::cout<<std::endl;
	for(Long64_t  ientry=0;ientry<nentries;ientry++){
		fChain->GetEntry(ientry);
                std::cout << "\rProcessing entry " << ientry << " / " << nentries<<" = "<<ientry*100 /nentries<< " %"<< std::flush;
		int index_h=0;
		if(mode=="TimeCut_Done"){
                        for(index_h=0;index_h<histnum;index_h++){
                                if(timestamp.at(index_h).first<=d.time&&timestamp.at(index_h).second>d.time)break;
                        }
                        if(index_h==histnum) index_h=histnum-1;
		}else{index_h=d.file;}
		coefficient->GetEntry(index_h);
		for(int PMTi=0;PMTi<6;++PMTi){
			E.at(PMTi)=d.ADC.at(PMTi)*slope.at(PMTi).first+segment.at(PMTi).first;
			E_narrow.at(PMTi)=d.ADC_n.at(PMTi)*slope.at(PMTi).second+segment.at(PMTi).second;
		}
		diff_TRG=d.TRG-TRG_peak;
		for(int PMTi=0;PMTi<6/2;++PMTi){
			if(d.TDC.at(PMTi*2)!=-4444&&d.TDC.at((2*PMTi+3)%6)!=-4444){
				diff_TDC_2coin.at(PMTi)=d.TDC.at(PMTi*2)-d.TDC.at((2*PMTi+3)%6)-TDC_peak.at(PMTi*2).at((2*PMTi+3)%6);
			}else{
				diff_TDC_2coin.at(PMTi)=-4444;
			}
			if(type=="shibataDATA"){
				if(d.TDC.at(PMTi*2)!=-4444&&d.TDC_high.at(PMTi)!=-4444){
					diff_TDC_high.at(PMTi)=d.TDC.at(PMTi*2)-d.TDC_high.at(PMTi)-TDC_peak.at(PMTi*2).at(PMTi*2);
				}else if(d.TDC_high.at(PMTi)==-4444&&d.TDC.at(PMTi*2)!=-4444){
					diff_TDC_high.at(PMTi)=0;
				}else{
					 diff_TDC_high.at(PMTi)=-4444;
				}
			}
			for(int j=0;j<4;j++){
				if(d.TDC.at(2*PMTi)!=-4444&&d.TDC.at(arg_3coin.at(PMTi).at(j))!=-4444){
					diff_TDC_3coin.at(PMTi).at(j)=d.TDC.at(2*PMTi)-d.TDC.at(arg_3coin.at(PMTi).at(j))-TDC_peak.at(2*PMTi).at(arg_3coin.at(PMTi).at(j));
				}else{
					diff_TDC_3coin.at(PMTi).at(j)=-4444;
				}
			}
		
		}
		outtree->Fill();


	}

	out->cd();
	outtree->Write();
	MYLOG_INFO("Save TTree "<<outname);
	out->Close();
	histfile->Close();
	return;








}

#endif
