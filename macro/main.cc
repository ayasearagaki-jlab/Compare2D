#include "../include/Between2D.h"
#include "../include/Between2D.cpp"

Between2D bet;

int main(){
	bet.SetBlind(true);
	bet.SetConfig_TDC_2coinCut(true,10.);
	bet.SetConfig_TDC_3coinCut(false,15.);
	bet.SetConfig_Exclude_2coinCut(true,400.);
	bet.SetConfig_Exclude_3coinCut(true,400.);
	bet.SetConfig_TRG_Cut(false,1.5);
	bet.SetConfig_ADC_Cut(false,100.);
	bet.SetConfig_ADC_Cut_3coin(false,100.);
	bet.SetConfig_All_ADC_Cut(false,100.);

	//bet.MakeTH2D();
	bet.MakeTH2D_MT(5);
	bet.MakeReport();

	return 1;
}
