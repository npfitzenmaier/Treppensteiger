#include "treppensteiger.hpp"
#include "datenlogger.hpp"

#include <iostream>
#include <iomanip>
#include <string>

int Datenlogger::init(){
	aktiv = false;
	
	anzahl_fehlerprotokoll_eintraege = 0;
	anzahl_systeminfo_eintraege = 0;
	anzahl_winkelsensor_eintraege = 0;
	anzahl_imu_eintraege = 0;
	
	fehlerprotokoll.open(dateiname_fehlerprotokoll, std::fstream::out | std::fstream::trunc);
	
	logdatei_systeminfo.open(dateiname_systeminfo, std::fstream::out | std::fstream::trunc);	
	logdatei_winkelsensor.open(dateiname_winkelsensor, std::fstream::out | std::fstream::trunc);
	logdatei_imu.open(dateiname_imu, std::fstream::out | std::fstream::trunc);	
	
	if(fehlerprotokoll.is_open() && logdatei_systeminfo.is_open() && logdatei_imu.is_open() && logdatei_winkelsensor.is_open()){		
		return 0;
	}
	else{
		return -1;
	}
}

void Datenlogger::schreibe_fehlerprotokoll_eintrag(Treppensteiger &ts, std::string beschreibung){	
	fehlerprotokoll << std::fixed << std::setprecision(3);
	fehlerprotokoll << std::setw(15) << ((double)ts.systeminfo.vergangene_us_seit_start)/1000000.0;	//(1)
	fehlerprotokoll << std::setw(15) << ts.systeminfo.schleifenzeit*1000.0;							//(2)
	fehlerprotokoll << std::setw(15) << " ";
	fehlerprotokoll << beschreibung;
	fehlerprotokoll << "\n";
	anzahl_fehlerprotokoll_eintraege++;
}

void Datenlogger::schreibe_systeminfo_logeintrag(Treppensteiger &ts){
	if(aktiv){
		anzahl_systeminfo_eintraege++;
	}
}

void Datenlogger::schreibe_winkelsensoren_logeintrag(Treppensteiger &ts){
	static bool erster_durchlauf = true;
	if(erster_durchlauf){
		erster_durchlauf = false;
		
		std::string spaltenzahl;
		const int anzahl_spalten = 14;
		
		for(int i=1; i<=anzahl_spalten; i++){
			spaltenzahl = '(' + std::to_string(i) + ')';
			logdatei_winkelsensor << std::setw(15) << spaltenzahl;
		}
		logdatei_winkelsensor << "\n";
		for(int i=1; i<=(anzahl_spalten+1); i++){			
			logdatei_winkelsensor << "---------------";
		}
		logdatei_winkelsensor << "\n";
	}
	if(aktiv){		
		logdatei_winkelsensor << std::fixed << std::setprecision(3);
		logdatei_winkelsensor << std::setw(15) << ((double)ts.systeminfo.vergangene_us_seit_start)/1000000.0;	//(1)
		logdatei_winkelsensor << std::setw(15) << ts.systeminfo.schleifenzeit*1000.0;							//(2)
		logdatei_winkelsensor << std::setw(15) << ts.dreistern_links.winkelsensor_antrieb.rohwert;				//(3)
		logdatei_winkelsensor << std::setw(15) << ts.dreistern_links.winkelsensor_antrieb.winkel_grad;			//(4)
		logdatei_winkelsensor << std::setw(15) << ts.dreistern_rechts.winkelsensor_antrieb.rohwert;				//(5)
		logdatei_winkelsensor << std::setw(15) << ts.dreistern_rechts.winkelsensor_antrieb.winkel_grad;			//(6)
		logdatei_winkelsensor << std::setw(15) << ts.dreistern_links.winkelsensor_steig.rohwert;				//(7)
		logdatei_winkelsensor << std::setw(15) << ts.dreistern_links.winkelsensor_steig.winkel_grad;			//(8)
		logdatei_winkelsensor << std::setw(15) << ts.dreistern_rechts.winkelsensor_steig.rohwert;				//(9)
		logdatei_winkelsensor << std::setw(15) << ts.dreistern_rechts.winkelsensor_steig.winkel_grad;			//(10)
		logdatei_winkelsensor << std::setw(15) << ts.arm_vorne.lenkung.winkelsensor.rohwert;					//(11)
		logdatei_winkelsensor << std::setw(15) << ts.arm_vorne.lenkung.winkelsensor.winkel_grad;				//(12)
		logdatei_winkelsensor << std::setw(15) << ts.arm_hinten.lenkung.winkelsensor.rohwert;					//(13)
		logdatei_winkelsensor << std::setw(15) << ts.arm_hinten.lenkung.winkelsensor.winkel_grad;				//(14)
		logdatei_winkelsensor << "\n";
		anzahl_winkelsensor_eintraege++;
	}	
}

void Datenlogger::schreibe_imu_logeintrag(Treppensteiger &ts){
	if(aktiv){
		anzahl_imu_eintraege++;
	}
}
