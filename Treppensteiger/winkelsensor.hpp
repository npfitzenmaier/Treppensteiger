#ifndef winkelsensor_H
#define winkelsensor_H

#include <stdlib.h>
#include <cstdint>

#include "gpio_schnittstelle.hpp"
#include "spi_schnittstelle.hpp"
#include "datenlogger.hpp"

#define GPIO_CS_TLE5012_ANTRIEB_RECHTS	5
#define GPIO_CS_TLE5012_ANTRIEB_LINKS	0
#define GPIO_CS_TLE5012_STEIG_LINKS		13
#define GPIO_CS_TLE5012_STEIG_RECHTS	6
#define GPIO_CS_TLE5012_LENKUNG_HINTEN	19
#define GPIO_CS_TLE5012_LENKUNG_VORNE	26

class Winkelsensor{
	public:
		short rohwert;
		float winkel_grad;
		float winkel_radiant;
		
		int uebertragungsfehler = 0;
		
		int gpio_cs;
		GPIO_Schnittstelle* gpio_schnittstelle;
		SPI_Schnittstelle* spi_schnittstelle;
		
		Winkelsensor(){};
		Winkelsensor(GPIO_Schnittstelle* gpio_schnittstelle, SPI_Schnittstelle* spi_schnittstelle, int gpio_cs)
			: gpio_schnittstelle(gpio_schnittstelle)
			, spi_schnittstelle(spi_schnittstelle)
			, gpio_cs(gpio_cs){}
			
		int daten_anfordern();
	private:
		unsigned char command_word[2];
		unsigned char data_word[2];
		unsigned char safety_word[2];
	
		uint16_t rxval;
		
		//Look-up table (LUT) for the TLE5012B with generator polynomial 100011101 (0x11D).
		//As this table will be checked byte by byte, each byte has 256 possible values (2^8)
		//for its CRC calculation with the given generator polynomial.
		static const unsigned char CRC_Tabelle[256];
		
		int transfer(unsigned char* buf, size_t len);
		
		bool zyklische_redundanzpruefung();
		static void erstelle_crc_tabelle(unsigned int polynom, unsigned char* crc_tabelle);
		static unsigned char CRC8(unsigned char *nachricht, unsigned char bytelaenge);
		static unsigned char CRC8(unsigned char *nachricht, unsigned char bytelaenge, unsigned char* crc_tabelle);		
};

#endif