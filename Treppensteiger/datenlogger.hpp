#ifndef datenlogger_H
#define datenlogger_H

#include <fstream>
#include <string>

#include "datenlogger.hpp"

class Treppensteiger;

class Datenlogger{		
	public:
		bool aktiv;
		int anzahl_fehlerprotokoll_eintraege;
		int anzahl_systeminfo_eintraege;
		int anzahl_winkelsensor_eintraege;
		int anzahl_imu_eintraege;		
		
		std::fstream fehlerprotokoll;
		std::fstream logdatei_systeminfo;
		std::fstream logdatei_winkelsensor;
		std::fstream logdatei_imu;		
	
		Datenlogger(){}
			
		~Datenlogger(){
			if(fehlerprotokoll.is_open()){
				fehlerprotokoll.close();
			}
			if(logdatei_systeminfo.is_open()){
				logdatei_systeminfo.close();
			}
			if(logdatei_winkelsensor.is_open()){
				logdatei_winkelsensor.close();
			}
			if(logdatei_imu.is_open()){
				logdatei_imu.close();
			}
		}
		
		int init();
		
		void schreibe_fehlerprotokoll_eintrag(Treppensteiger &ts, std::string beschreibung);
		
		void schreibe_systeminfo_logeintrag(Treppensteiger &ts);
		void schreibe_winkelsensoren_logeintrag(Treppensteiger &ts);
		void schreibe_imu_logeintrag(Treppensteiger &ts);		
		
	private:		
		const char* dateiname_fehlerprotokoll	= "Logdaten/fehlerprotokoll.log";
		const char* dateiname_systeminfo		= "Logdaten/systeminfo.log";
		const char* dateiname_winkelsensor		= "Logdaten/winkelsensor.log";
		const char* dateiname_imu				= "Logdaten/imu.log";		
};

#endif