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
	///////////////////////////////////////////////////////////////////////////////////////
	//			MAIN 	For Loop		///////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////

	void Between2D::MakeTH2D(){
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
		for(Long64_t ientry=0;ientry<nentries.first;ientry++){
			tree.first->GetEntry(ientry);
			FillData2coin();
			FillData3coin_511();
			FillData3coin();
		}

		type="usshiDATA";
		for(Long64_t ientry=0;ientry<nentries.second;ientry++){
                	tree.second->GetEntry(ientry);
			FillData2coin();
			FillData3coin_511();
			FillData3coin();
		}

	
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
			int nth=-1;
                        energy eee(ev.E[2*PMTi],ev.E[(2*PMTi+3)%6]);
                        tofilled3coin_511=tofilled3coin_511&&TRG_Cut();
                        tofilled3coin_511=tofilled3coin_511&&TDC_2coinCut(PMTi);
			tofilled3coin_511=tofilled3coin_511&&Find3coin(511,nth,PMTi);
                        tofilled3coin_511=tofilled3coin_511&&Exclude_3coinCut(PMTi,nth);
			tofilled3coin_511=tofilled3coin_511&&TDC_3coinCut(PMTi,nth);
                        tofilled3coin_511=tofilled3coin_511&&ADC_Cut(PMTi);
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
                        int nth=-1;
                        energy eee(ev.E[2*PMTi],ev.E[(2*PMTi+3)%6]);
                        tofilled3coin=tofilled3coin&&TRG_Cut();
                        tofilled3coin=tofilled3coin&&TDC_2coinCut(PMTi);
                        tofilled3coin=tofilled3coin&&Find3coin(1275,nth,PMTi);
                        tofilled3coin=tofilled3coin&&Exclude_3coinCut(PMTi,nth);
                        tofilled3coin=tofilled3coin&&TDC_3coinCut(PMTi,nth);
                        tofilled3coin=tofilled3coin&&ADC_Cut(PMTi);
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
	

	bool Between2D::Find3coin(int ener,int &num,int pair){
		double thre_up,thre_low;
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
				num=j;
                                break;
			}
			
		}
		
		return OK;
	
	
	}

	bool Between2D::TDC_2coinCut(int pair){
		if(!doTDC_2coinCut) return true;
		
		const Event& ev = (type == "shibataDATA") ? event.first : event.second;

		return TMath::Abs(ev.TDC_2coin.at(pair))<=width_TDC_2coinCut;
	}
	
	bool Between2D::TDC_3coinCut(int pair,int num){
                if(!doTDC_3coinCut) return true;
		if(num<0||num>=4) return false;

                const Event& ev = (type == "shibataDATA") ? event.first : event.second;

                return TMath::Abs(ev.TDC_3coin.at(pair).at(num))<=width_TDC_3coinCut;
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
	
	bool Between2D::Exclude_3coinCut(int pair,int num){
                if(!doExclude_3coinCut)return true;
		if(num<0||num>=4) return false;

                const Event& ev = (type == "shibataDATA") ? event.first : event.second;
                bool OK=true;
                for(int j=0;j<4;++j){
			if(j==num)continue;
                        if(ev.E[arg_3coin.at(pair).at(j)]>threshold_Exclude_2coinCut){
                                OK=false;
                                break;
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

#endif
