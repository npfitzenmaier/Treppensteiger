#include "systeminfo.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

//https://github.com/vivaladav/BitsOfBytes/blob/master/cpp-program-to-get-cpu-usage-from-command-line-in-linux/main.cpp

int Systeminfo::init(){
	#ifdef __linux__
		if (clock_gettime(CLOCK_MONOTONIC_RAW, &startzeitpunkt)) {
			return -1;
		}
	#elif _WIN32
		startzeitpunkt = std::chrono::high_resolution_clock::now();
	#endif
	
	try{
		prozessordaten_einlesen(eintraege_aktuell);
		return 0;
	}
	catch(...){
		return -1;
	}
}

void Systeminfo::berechne_schleifenzeit(){
	#ifdef __linux__
		if(clock_gettime(CLOCK_MONOTONIC_RAW, &endzeitpunkt)){
			/* handle error */ 
		}
	
		schleifenzeit = timespec_zu_sekunden(&endzeitpunkt) - timespec_zu_sekunden(&startzeitpunkt);	
	
		if(clock_gettime(CLOCK_MONOTONIC_RAW, &startzeitpunkt)){
			/* handle error */
		}
	#elif _WIN32
		endzeitpunkt = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> duration = endzeitpunkt - startzeitpunkt;
		startzeitpunkt = std::chrono::high_resolution_clock::now();
		schleifenzeit = duration.count();		
	#endif
	
	vergangene_us_seit_start += (unsigned long long)(schleifenzeit*1000000.0);
	
	vergangene_zeit_seit_letzter_schleifenzeit_statistik += schleifenzeit;
	if(vergangene_zeit_seit_letzter_schleifenzeit_statistik >= 1.0){		
		anzahl_schleifenzeit_array_elemente = schleifenzeit_array.size();
		
		if(anzahl_schleifenzeit_array_elemente >= 1){
			maximale_schleifenzeit = schleifenzeit_array[0];
			minimale_schleifenzeit = schleifenzeit_array[0];			
			double schleifenzeit_summe = 0.0;
			
			for(unsigned int i = 0; i < anzahl_schleifenzeit_array_elemente; ++i){
				schleifenzeit_summe += schleifenzeit_array[i];
				
				if(schleifenzeit_array[i] >= maximale_schleifenzeit)	maximale_schleifenzeit = schleifenzeit_array[i];
				if(schleifenzeit_array[i] < minimale_schleifenzeit)		minimale_schleifenzeit = schleifenzeit_array[i];
			}
			mittlere_schleifenzeit = schleifenzeit_summe/((double)anzahl_schleifenzeit_array_elemente);
			schleifenzeit_array.clear();
		}
		else{
			// Fehler
		}		
		vergangene_zeit_seit_letzter_schleifenzeit_statistik = 0.0;
	}
	else{
		schleifenzeit_array.push_back(schleifenzeit);
	}
}

double Systeminfo::timespec_zu_sekunden(struct timespec* ts){
    return (double)ts->tv_sec + (double)ts->tv_nsec / 1000000000.0;
}

void Systeminfo::berechne_prozessorauslastung(){
	vergangene_zeit_seit_letzter_prozessorauslastung_berechnung += schleifenzeit;
	if(vergangene_zeit_seit_letzter_prozessorauslastung_berechnung >= 1.0){		
		eintraege_zuvor = eintraege_aktuell;
		eintraege_aktuell.clear();
		prozessordaten_einlesen(eintraege_aktuell);
		
		const size_t anzahl_eintraege = eintraege_aktuell.size();
		
		prozessorkernauslastung.clear();

		for(size_t i = 0; i < anzahl_eintraege; ++i){
			const Prozessordaten &e1 = eintraege_zuvor[i];
			const Prozessordaten &e2 = eintraege_aktuell[i];		

			const float aktive_zeit		= static_cast<float>(berechne_prozessor_aktive_dauer(e2) - berechne_prozessor_aktive_dauer(e1));
			const float leerlauf_zeit	= static_cast<float>(berechne_prozessor_leerlauf_dauer(e2) - berechne_prozessor_leerlauf_dauer(e1));
			const float gesamtzeit		= aktive_zeit + leerlauf_zeit;		
			
			if(i == 0){
				prozessorgesamtauslastung = 100.f*(aktive_zeit/gesamtzeit);
			}
			else{
				prozessorkernauslastung.push_back(100.f*(aktive_zeit/gesamtzeit));
			}
		}
		vergangene_zeit_seit_letzter_prozessorauslastung_berechnung = 0.0;
	}		
}

void Systeminfo::prozessordaten_einlesen(std::vector<Prozessordaten> &eintraege){
	#ifdef __linux__
		std::ifstream fd("/proc/stat");

		std::string zeile;

		const std::string cpu_str("cpu");
		const std::size_t cpu_str_laenge = cpu_str.size();
		const std::string tot_str("tot");

		while(std::getline(fd, zeile)){
			// cpu zustandszeile gefunden
			if(!zeile.compare(0, cpu_str_laenge, cpu_str)){
				std::istringstream ss(zeile);

				// eintrag abspeichern
				eintraege.emplace_back(Prozessordaten());
				Prozessordaten &eintrag = eintraege.back();

				// read cpu label
				ss >> eintrag.cpu;

				// remove "cpu" from the label when it's a processor number
				if(eintrag.cpu.size() > cpu_str_laenge)
					eintrag.cpu.erase(0, cpu_str_laenge);
				// replace "cpu" with "tot" when it's total values
				else
					eintrag.cpu = tot_str;

				// zeiten lesen
				for(int i = 0; i < anzahl_prozessorzustaende; ++i)
					ss >> eintrag.zeiten[i];
			}
		}
	#endif
}

size_t Systeminfo::berechne_prozessor_leerlauf_dauer(const Prozessordaten &e){
	return	e.zeiten[S_IDLE] + 
			e.zeiten[S_IOWAIT];
}

size_t Systeminfo::berechne_prozessor_aktive_dauer(const Prozessordaten &e){
	return	e.zeiten[S_USER] +
			e.zeiten[S_NICE] +
			e.zeiten[S_SYSTEM] +
			e.zeiten[S_IRQ] +
			e.zeiten[S_SOFTIRQ] +
			e.zeiten[S_STEAL] +
			e.zeiten[S_GUEST] +
			e.zeiten[S_GUEST_NICE];
}