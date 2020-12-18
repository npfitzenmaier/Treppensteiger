#ifndef kraftsensor_H
#define kraftsensor_H

#include <cstdint>

#include "i2c_schnittstelle.hpp"

class Kraftsensor{
	public:
		float kraft = 0.0;	// [N]
		
		int kraft_rohwert = 0;
		int mittelwert_kraft_rohwert = 0;
		
		static const float maximale_kraft;
		
		static const int anzahl_array_elemente = 100;
		int array[anzahl_array_elemente];
		int array_summe = 0;
		int zeiger_element = 0;
		
		int nullabweichung;
		float skalierung;
		
		static constexpr int I2C_Adresse = 0x2b;
		static constexpr uint32_t DATEN_LAENGE = 9;
		
		static I2C_Schnittstelle* i2c_schnittstelle;
		
		Kraftsensor(){
			for(int i=0; i<anzahl_array_elemente; i++){
				array[i] = 0;
			}
		}
		
		static void daten_anfordern(Kraftsensor &k1, Kraftsensor &k2);
		
		static unsigned char getCRC(unsigned char message[], unsigned char length);
		
		void mittelwert();
	private:
		static const unsigned char CRC7_POLY = 0x91;
};

#endif