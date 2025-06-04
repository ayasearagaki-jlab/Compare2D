#include "../include/Between2D.h"
#include "../include/Between2D.cpp"

Between2D bet;

int main(){
	bet.SetBlind(false);
	bet.SetConfig_TDC_2coinCut(false,6.);
	bet.SetConfig_TDC_3coinCut(false,12.);
	bet.SetConfig_Exclude_2coinCut(false,200.);
	bet.SetConfig_Exclude_3coinCut(false,200.);
	bet.SetConfig_TRG_Cut(true,1.5);
	bet.SetConfig_ADC_Cut(false,100.);
	bet.SetConfig_All_ADC_Cut(false,100.);

	bet.MakeTH2D();
	bet.MakeReport();

	return 1;
}
