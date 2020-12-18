#ifndef stellglied_H
#define stellglied_H

#include <cstdint>

#include "i2c_schnittstelle.hpp"
#include "motortreiber.hpp"
#include "kraftsensor.hpp"

// 0.1 mm Schritte (int)

class Stellglied{
	public:
		// TODO
		// Abspeichern der Position in einer Datei
		
		float laenge = 0.0;			// [mm]
		int position_rohwert = 0;	// 0.1 mm Schritte
		int position_offset = 0;
		
		static const float hall_schritte_pro_mm;		// [Schritte/mm]
		static const float eingezogene_laenge;			// [mm]
		static const float standard_laenge;				// [mm]
		
		static I2C_Schnittstelle* i2c_schnittstelle;
		Motortreiber motortreiber;
		Kraftsensor kraftsensor;
		
		float p, i, d;
		float Kp, Ki, Kd, T;		
		float fuehrungsgroesse = 0.0;
		float regelabweichung;
		float regelabweichung_zuvor;
		float stellgroesse;
		float minimale_stellgroesse;
		float regelgroesse;
		
		Stellglied(){}
		Stellglied(Motortreiber &motortreiber)
			: motortreiber(motortreiber){
				
			float Kr_kritisch = 0.00523703;
			float T_kritisch = 0.7;
			/*
			// PID-Reglerparameter nach Ziegler-Nichols				
			float Kr = 0.6 * Kr_kritisch;
			float Tn = 0.5 * T_kritisch;
			float Tv = 0.125 * T_kritisch;
			
			Kp = Kr;
			Ki = Kr/Tn;
			Kd = Kr*Tv;*/
			
			float Kr = 0.5 * Kr_kritisch;
			
			Kp = Kr;
			Ki = 0.0;
			Kd = 0.0;
				
			i = 0.0;
			
			regelabweichung = 0.0;
		}
		
		static void daten_anfordern(Stellglied &s1, Stellglied &s2);		
		static unsigned char getCRC(unsigned char message[], unsigned char length);
		void pid_regler_setze_stellgroesse(float vergangene_Zeit);
	private:	
		static const unsigned char CRC7_POLY = 0x91;
		
		static constexpr int I2C_ADDRESSE = 0x2A;
		static constexpr uint32_t DATEN_LAENGE = 9;
};

#endif