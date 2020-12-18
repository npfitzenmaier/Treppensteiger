#include "kraftsensor.hpp"

#ifdef __linux__
	#include <linux/i2c-dev.h>
	#include <i2c/smbus.h>
	#include <sys/ioctl.h>
#endif

#include <cstdio>

const float Kraftsensor::maximale_kraft = 1000.0;

I2C_Schnittstelle* Kraftsensor::i2c_schnittstelle = NULL;

void Kraftsensor::daten_anfordern(Kraftsensor &k1, Kraftsensor &k2){
	uint8_t daten[DATEN_LAENGE];

	#ifdef __linux__
		if(ioctl(Kraftsensor::i2c_schnittstelle->fd, I2C_SLAVE, I2C_Adresse) < 0);

		int32_t num = i2c_smbus_read_i2c_block_data(Kraftsensor::i2c_schnittstelle->fd, 0x00, DATEN_LAENGE, daten);

		if(num == DATEN_LAENGE && Kraftsensor::getCRC(daten, DATEN_LAENGE - 1) == ((uint8_t) daten[DATEN_LAENGE - 1])){
				
			k1.kraft_rohwert = daten[3];
			k1.kraft_rohwert = (k1.kraft_rohwert << 8) + daten[2];
			k1.kraft_rohwert = (k1.kraft_rohwert << 8) + daten[1];
			k1.kraft_rohwert = (k1.kraft_rohwert << 8) + daten[0];

			k2.kraft_rohwert = daten[7];
			k2.kraft_rohwert = (k2.kraft_rohwert << 8) + daten[6];
			k2.kraft_rohwert = (k2.kraft_rohwert << 8) + daten[5];
			k2.kraft_rohwert = (k2.kraft_rohwert << 8) + daten[4];
		
			k1.kraft_rohwert -= k1.nullabweichung;
			k2.kraft_rohwert -= k2.nullabweichung;
		
			k1.kraft = ((float)k1.kraft_rohwert) * k1.skalierung;
			k2.kraft = ((float)k2.kraft_rohwert) * k2.skalierung;
		
			k1.mittelwert();
			k2.mittelwert();
		}
	#endif
}

unsigned char Kraftsensor::getCRC(unsigned char nachricht[], unsigned char laenge){
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

void Kraftsensor::mittelwert(){
	
	array_summe -= array[zeiger_element];
	array[zeiger_element] = kraft_rohwert;	
	array_summe += array[zeiger_element];
	
	zeiger_element++;
	
	if(zeiger_element >= anzahl_array_elemente){
		zeiger_element = 0;
	}
	
	mittelwert_kraft_rohwert = array_summe / anzahl_array_elemente;
}