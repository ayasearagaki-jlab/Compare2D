#include"/home/fermi/rshibata/include/Calibration.h"
#include"/home/fermi/rshibata/include/Calibration.cpp"
#include"/home/fermi/rshibata/include/Calibration_part2.cpp"
#include"/home/fermi/rshibata/include/Calibration_part3.cpp"
std::string date="180128";//FIXME
Calibration calib(date);

int main(){
	calib.MakeTreeRaw();
//	calib.MakeTH1D("TimeCut");
//	calib.SetDEBUGMode();
//	calib.ExecuteCalibration();
//	calib.MakeFinalTree();

}

