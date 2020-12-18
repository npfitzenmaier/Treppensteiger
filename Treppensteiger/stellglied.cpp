#ifdef __linux__
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <sys/ioctl.h>
#endif

#define _USE_MATH_DEFINES
#include <cmath>

#include "stellglied.hpp"

I2C_Schnittstelle* Stellglied::i2c_schnittstelle = NULL;

const float Stellglied::hall_schritte_pro_mm = 28.6 * 2.0;	// [Schritte/mm]
const float Stellglied::eingezogene_laenge = 260.0;		// [mm]
const float Stellglied::standard_laenge = 328.0;		// [mm]	Autodesk Modell
//const float Stellglied::standard_laenge = 324.0;		// [mm] real gemessen vorne
//const float Stellglied::standard_laenge = 308.0;		// [mm] real gemessen hinten

void Stellglied::daten_anfordern(Stellglied &s1, Stellglied &s2){
	uint8_t daten[DATEN_LAENGE];
	
	#ifdef __linux__
		if(ioctl(Stellglied::i2c_schnittstelle->fd, I2C_SLAVE, I2C_ADDRESSE) < 0)	return;

		int32_t num = i2c_smbus_read_i2c_block_data(Stellglied::i2c_schnittstelle->fd, 0x00, DATEN_LAENGE, daten);
	

		if(num == DATEN_LAENGE && Stellglied::getCRC(daten, DATEN_LAENGE - 1) == ((uint8_t) daten[DATEN_LAENGE - 1])){		
				
			s1.position_rohwert = daten[3];
			s1.position_rohwert = (s1.position_rohwert << 8) + daten[2];
			s1.position_rohwert = (s1.position_rohwert << 8) + daten[1];
			s1.position_rohwert = (s1.position_rohwert << 8) + daten[0];
		
			s2.position_rohwert = daten[7];
			s2.position_rohwert = (s2.position_rohwert << 8) + daten[6];
			s2.position_rohwert = (s2.position_rohwert << 8) + daten[5];
			s2.position_rohwert = (s2.position_rohwert << 8) + daten[4];
		
			s1.laenge = standard_laenge + ((float) s1.position_rohwert - s1.position_offset) / hall_schritte_pro_mm;
			s2.laenge = standard_laenge + ((float) s2.position_rohwert - s2.position_offset) / hall_schritte_pro_mm;
		}
	#endif
}
/*
void Stellglied::schreibe_position(float laenge){
	unsigned char sendepuffer[DATEN_LAENGE];
	unsigned char *z_puffer;
	
	int position = (int) (laenge / 10.0);
	
	z_puffer = &sendepuffer[0];
	*z_puffer++ = position;
	*z_puffer++ = position >> 8;
	*z_puffer++ = position >> 16;
	*z_puffer++ = position >> 24;
	*z_puffer++ = getCRC(&sendepuffer[0], (z_puffer - &sendepuffer[0]));	
	
	i2c_smbus_write_i2c_block_data(i2c_schnittstelle->fd, static_cast<uint8_t>(id), DATEN_LAENGE, sendepuffer);
}
*/

void Stellglied::pid_regler_setze_stellgroesse(float vergangene_Zeit){
	regelgroesse = kraftsensor.kraft;
	regelabweichung = fuehrungsgroesse - regelgroesse;
	
	regelabweichung_zuvor = regelabweichung;
	
	T = vergangene_Zeit;
	
	p = Kp * regelabweichung;
    i += Ki * (((regelabweichung + regelabweichung_zuvor)/2)*T);
    d = Kd * ((regelabweichung - regelabweichung_zuvor)/T);
	
	stellgroesse = p + i + d;
	
	if(std::abs(stellgroesse) > 1.0){
		stellgroesse = stellgroesse/std::abs(stellgroesse);
	}
	
	//if(std::abs(stellgroesse) < minimale_stellgroesse)	stellgroesse = 0.0;
	
	motortreiber.setze_antriebsleistung(stellgroesse);
}

unsigned char Stellglied::getCRC(unsigned char nachricht[], unsigned char laenge){
	unsigned char i, j, crc = 0;
	for (i = 0; i < laenge; i++){
		crc ^= nachricht[i];
		for (j = 0; j < 8; j++)	{
			if (crc & 1)
			crc ^= CRC7_POLY;
			crc >>= 1;
		}
	}
	return crc;
}


