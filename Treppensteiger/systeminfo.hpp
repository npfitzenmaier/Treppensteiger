#ifndef systeminfo_H
#define systeminfo_H

#include <vector>
#include <time.h>
#include <string>

#ifdef __linux__
#elif _WIN32
#include <chrono>
#endif

class Systeminfo{
	public:
		double schleifenzeit = 0.0;				// [s]
		double mittlere_schleifenzeit = 0.0;	// [s]
		double maximale_schleifenzeit = 0.0;	// [s]
		double minimale_schleifenzeit = 0.0;	// [s]		
		
		unsigned int anzahl_schleifenzeit_array_elemente = 0;
		
		unsigned long long vergangene_us_seit_start = 0;	// [µs]	
	
		float prozessorgesamtauslastung = 0.0;
		std::vector<float> prozessorkernauslastung;
		
		int init();
		void berechne_schleifenzeit();
		void berechne_prozessorauslastung();
		
	private:
		#ifdef __linux__
			struct timespec startzeitpunkt;
			struct timespec endzeitpunkt;
		#elif _WIN32
			std::chrono::high_resolution_clock::time_point startzeitpunkt;
			std::chrono::high_resolution_clock::time_point endzeitpunkt;
		#endif
		
	
		enum Prozessorzustaende{
			S_USER = 0,
			S_NICE,
			S_SYSTEM,
			S_IDLE,
			S_IOWAIT,
			S_IRQ,
			S_SOFTIRQ,
			S_STEAL,
			S_GUEST,
			S_GUEST_NICE
		};

		double vergangene_zeit_seit_letzter_schleifenzeit_statistik = 0.0;
		std::vector<float> schleifenzeit_array;
		
		double vergangene_zeit_seit_letzter_prozessorauslastung_berechnung = 0.0;
		static const int anzahl_prozessorzustaende = 10;

		typedef struct Prozessordaten{
			std::string cpu;
			size_t zeiten[anzahl_prozessorzustaende];
		}Prozessordaten;
		
		std::vector<Prozessordaten> eintraege_zuvor;
		std::vector<Prozessordaten> eintraege_aktuell;
		
		static double timespec_zu_sekunden(struct timespec* ts);
		
		void prozessordaten_einlesen(std::vector<Prozessordaten> &eintraege);
		size_t berechne_prozessor_leerlauf_dauer(const Prozessordaten &e);
		size_t berechne_prozessor_aktive_dauer(const Prozessordaten &e);
};

#endif