#include "winkelsensor.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <time.h>
#include <fcntl.h>
#include <iostream>
#include <string>

#ifdef __linux__
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <sys/ioctl.h>
	#include <linux/types.h>
	#include <linux/spi/spidev.h>
#endif

#define _USE_MATH_DEFINES
#include <cmath>
//#include <cstdio>

#define POW_2_15                    32768.0   //!< values used to for final calculations of angle speed, revolutions, range and value
#define ANGLE_360_VAL               360.0

using namespace std;

//Look-up table (LUT) for the TLE5012B with generator polynomial 100011101 (0x11D).
//As this table will be checked byte by byte, each byte has 256 possible values (2^8)
//for its CRC calculation with the given generator polynomial.
const unsigned char CRC_Tabelle[256]{
	//The “crc” of the position [1] (result from operation [crc ^*(message+Byteidx)])
	//is 0x00 -> 0x00 XOR 0x11D = 0x00 (1 byte).
	0x00,
	//The “crc” of the position [2] is 0x1D -> 0x01 XOR 0x11D = 0x1D (1 byte).
	0x1D,
	//The “crc” of the position [3] is 0x3A -> 0x02 XOR 0x11D = 0x3A (1 byte).
	0x3A,
	//For all the rest of the cases.
	0x27, 0x74, 0x69, 0x4E, 0x53, 0xE8, 0xF5, 0xD2, 0xCF, 0x9C, 0x81, 0xA6, 0xBB, 0xCD,
	0xD0, 0xF7, 0xEA, 0xB9, 0xA4, 0x83, 0x9E, 0x25, 0x38, 0x1F, 0x02, 0x51, 0x4C, 0x6B,
	0x76, 0x87, 0x9A, 0xBD, 0xA0, 0xF3, 0xEE, 0xC9, 0xD4, 0x6F, 0x72, 0x55, 0x48, 0x1B,
	0x06, 0x21, 0x3C, 0x4A, 0x57, 0x70, 0x6D, 0x3E, 0x23, 0x04, 0x19, 0xA2, 0xBF, 0x98,
	0x85, 0xD6, 0xCB, 0xEC, 0xF1, 0x13, 0x0E, 0x29, 0x34, 0x67, 0x7A, 0x5D, 0x40, 0xFB,
	0xE6, 0xC1, 0xDC, 0x8F, 0x92, 0xB5, 0xA8, 0xDE, 0xC3, 0xE4, 0xF9, 0xAA, 0xB7, 0x90,
	0x8D, 0x36, 0x2B, 0x0C, 0x11, 0x42, 0x5F, 0x78, 0x65, 0x94, 0x89, 0xAE, 0xB3, 0xE0,
	0xFD, 0xDA, 0xC7, 0x7C, 0x61, 0x46, 0x5B, 0x08, 0x15, 0x32, 0x2F, 0x59, 0x44, 0x63,
	0x7E, 0x2D, 0x30, 0x17, 0x0A, 0xB1, 0xAC, 0x8B, 0x96, 0xC5, 0xD8, 0xFF, 0xE2, 0x26,
	0x3B, 0x1C, 0x01, 0x52, 0x4F, 0x68, 0x75, 0xCE, 0xD3, 0xF4, 0xE9, 0xBA, 0xA7, 0x80,
	0x9D, 0xEB, 0xF6, 0xD1, 0xCC, 0x9F, 0x82, 0xA5, 0xB8, 0x03, 0x1E, 0x39, 0x24, 0x77,
	0x6A, 0x4D, 0x50, 0xA1, 0xBC, 0x9B, 0x86, 0xD5, 0xC8, 0xEF, 0xF2, 0x49, 0x54, 0x73,
	0x6E, 0x3D, 0x20, 0x07, 0x1A, 0x6C, 0x71, 0x56, 0x4B, 0x18, 0x05, 0x22, 0x3F, 0x84,
	0x99, 0xBE, 0xA3, 0xF0, 0xED, 0xCA, 0xD7, 0x35, 0x28, 0x0F, 0x12, 0x41, 0x5C, 0x7B,
	0x66, 0xDD, 0xC0, 0xE7, 0xFA, 0xA9, 0xB4, 0x93, 0x8E, 0xF8, 0xE5, 0xC2, 0xDF, 0x8C,
	0x91, 0xB6, 0xAB, 0x10, 0x0D, 0x2A, 0x37, 0x64, 0x79, 0x5E, 0x43, 0xB2, 0xAF, 0x88,
	0x95, 0xC6, 0xDB, 0xFC, 0xE1, 0x5A, 0x47, 0x60, 0x7D, 0x2E, 0x33, 0x14, 0x09, 0x7F,
	0x62, 0x45, 0x58, 0x0B, 0x16, 0x31, 0x2C, 0x97, 0x8A, 0xAD, 0xB0, 0xE3, 0xFe,
	//The “crc” of the position [255] is 0xD9 -> 0xFE XOR 0x11D = 0xD9 (1 byte).
	0xD9,
	//The “crc” of the position [256] is 0xC4 -> 0xFF XOR 0x11D = 0xC4 (1 byte).
	0xC4
};

// Available sensors: spi_devices contains the BCM pin numbers of the CS corresponding to the SPI devices available

// Timing according to specification of TLE5012BE5000
//struct timespec twr_delay = { .tv_sec = 0, .tv_nsec = 200 }; // Specification: min. 100ns -> twr_delay = 200ns
//struct timespec tCSoff = { .tv_sec = 0, .tv_nsec = 1000 }; // Specification: min. 600ns -> tCSoff = 1µs

int Winkelsensor::daten_anfordern(){
	bool daten_erfolgreich_empfangen = false;
	const int maximale_uebertragungsfehler_hintereinander = 3;
	int ausfallerkennungszaehler = 0;
	#ifdef __linux__
		while(daten_erfolgreich_empfangen == false && ausfallerkennungszaehler < maximale_uebertragungsfehler_hintereinander){
			//siehe TLE5012 Benutzerhandbuch Seite 33
			command_word[0] = 0x80;		//MSB
			command_word[1] = 0x21;		//LSB
			data_word[0] = 0x00;
			data_word[1] = 0x00;
			safety_word[0] = 0x00;
			safety_word[1] = 0x00;

			gpioWrite(gpio_cs, 0); 							// TLE5012BE5000 ueber Chip Select pin (CSQ) auswaehlen
			transfer(command_word, sizeof(command_word));	
			usleep(1); 										// twr_delay (min. 130 ns), Schreib-Lese-Umschaltpause
			transfer(data_word, sizeof(data_word));
			transfer(safety_word, sizeof(safety_word));	
			gpioWrite(gpio_cs, 1);							// Kommunikation mit TLE5012BE5000 ueber Chip Select pin (CSQ) beenden
		
			if(zyklische_redundanzpruefung()){	
				rxval = ntohs(*reinterpret_cast<__uint16_t*>(data_word));	// Ethernet Byte Order (Big Endian) to system endian (Little Endian)
				rxval &= 0x7fff;
				/*
				if(rxval != 32){
					winkel_grad = (ANGLE_360_VAL / POW_2_15) * ((double)rxval);
					winkel_radiant = ((2*M_PI) / POW_2_15) * ((double)rxval);
				
					rohwert = rxval;
				}*/
			
				winkel_grad = (ANGLE_360_VAL / POW_2_15) * ((double)rxval);
				winkel_radiant = ((2*M_PI) / POW_2_15) * ((double)rxval);
			
				rohwert = rxval;
				daten_erfolgreich_empfangen = true;
				ausfallerkennungszaehler = 0;			
			}
			else{
				ausfallerkennungszaehler++;
			
			}
		}
	#endif
	
	if(daten_erfolgreich_empfangen){
		return 0;
	}
	else{
		return -1;
	}
}

int Winkelsensor::transfer(unsigned char* buf, size_t len){
	#ifdef __linux__
		struct spi_ioc_transfer tr;
		memset(&tr, 0, sizeof(tr));
		tr.tx_buf = (unsigned long)buf;
		tr.rx_buf = (unsigned long)buf;
		tr.len = len;
		tr.delay_usecs = 0;
		tr.speed_hz = spi_schnittstelle->spi_speed;
		tr.bits_per_word = spi_schnittstelle->spi_bpw;

		if(ioctl(spi_schnittstelle->fd, SPI_IOC_MESSAGE(1), &tr) == -1){
			//perror("ioctl transfer");
			return -1;
		}
	#endif
	return 0;
}

bool Winkelsensor::zyklische_redundanzpruefung(){
	unsigned char nachricht[4];
	nachricht[0] = command_word[0];		//MSB
	nachricht[1] = command_word[1];		//LSB
	nachricht[2] = data_word[0];		//MSB
	nachricht[3] = data_word[1];		//LSB
	
	if(Winkelsensor::CRC8(nachricht, sizeof(nachricht)) == safety_word[1])
		return true;
	else{
		uebertragungsfehler++;
		return false;
	}
}

//*******************************************************************************************************************************************************************************************

//“message” is the data transfer for which a CRC has to be calculated.
//A typical “message” consists of 2 bytes for the command word plus 2 bytes for the
//data word plus 2 bytes for the safety word.
//“Bytelength” is the number of bytes in the “message”. A typical “message” has 6
//bytes.
unsigned char Winkelsensor::CRC8(unsigned char *nachricht, unsigned char bytelaenge){
	//“crc” defined as the 8-bits that will be generated through the message till the
	//final crc is generated. In the example above this are the blue lines out of the
	//XOR operation.
	unsigned char crc;
	//“Byteidx” is a counter to compare the bytes used for the CRC calculation
	unsigned char Byteidx, Bitidx;
	//Initially the CRC remainder has to be set with the original seed (0xFF for the
	//TLE5012B).
	crc = 0xFF;
	//For all the bytes of the message.
	for(Byteidx=0; Byteidx<bytelaenge; Byteidx++){
		//“crc” is calculated as the XOR operation from the previous “crc” and the “message”.
		//“^” is the XOR operator.
		crc ^= nachricht[Byteidx];
		//For each bit position in a 8-bit word
		for(Bitidx=0; Bitidx<8; Bitidx++){
			//If the MSB of the “crc” is 1(with the &0x80 mask we get the MSB of the crc).
			if((crc&0x80)!=0){
				//“crc” advances on position (“crc” is moved left 1 bit: the MSB is deleted since it
				//will be cancelled out with the first one of the generator polynomial and a new bit
				//from the “message” is taken as LSB.)
				crc <<=1;
				//“crc” is calculated as the XOR operation from the previous “crc” and the generator
				//polynomial (0x1D for TLE5012B). Be aware that here the x8 bit is not taken since
				//the MSB of the “crc” already has been deleted in the previous step.
				crc ^= 0x1D;
			}
			//In case the crc MSB is 0.
			else
				//“crc” advances one position (this step is to ensure that the XOR operation is only
				//done when the generator polynomial is aligned with a MSB of the message that is “1”.
				crc <<= 1;
		}
	}
	//Return the inverted “crc” remainder(“~” is the invertion operator). An alternative
	//to the “~” operator would be a XOR operation between “crc” and a 0xFF polynomial.
	return(~crc);
}

//*******************************************************************************************************************************************************************************************

//“message” is the data transfer for which a CRC has to be calculated.
//A typical “message” consists of 2 bytes for the command word plus 2 bytes for the
//data word plus 2 bytes for the safety word.
//“Bytelength” is the number of bytes in the “message”. A typical “message” has 6
//bytes.
//*Table CRC is the pointer to the look-up table (LUT)
unsigned char Winkelsensor::CRC8(unsigned char *nachricht, unsigned char bytelaenge, unsigned char* crc_tabelle){
	//“crc” defined as the 8-bits that will be generated through the message till the
	//final crc is generated. In the example above this are the blue lines out of the
	//XOR operation.
	unsigned char crc;
	//“Byteidx” is a counter to compare the bytes used for the CRC calculation and
	//“Bytelength”.
	unsigned char Byteidx;
	//Initially the CRC remainder has to be set with the original seed (0xFF for the
	//TLE5012B).
	crc = 0xFF;
	//For all the bytes of the message.
	for(Byteidx=0; Byteidx<bytelaenge; Byteidx++){
		//“crc” is the value in the look-up table TableCRC[x] at the position “x”.
		//The position “x” is determined as the XOR operation between the previous “crc” and
		//the next byte of the “message”.
		//“^” is the XOR operator.
		crc = crc_tabelle[crc ^ *(nachricht+Byteidx)];
	}
	//Return the inverted “crc” remainder(“~” is the invertion operator). An alternative
	//to the “~” operator would be a XOR operation between “crc” and a 0xFF polynomial.
	return(~crc);
}

//*******************************************************************************************************************************************************************************************

//Generation of a look-up table (LUT)
void Winkelsensor::erstelle_crc_tabelle(unsigned int polynom, unsigned char* crc_tabelle){
	//“ReducedPoly” is the generator polynomial
	unsigned char reduziertes_polynom;
	unsigned int nachricht;
	unsigned char crc;
	unsigned bitindex;
	//Only 8 bits are taken
	reduziertes_polynom = (unsigned char)(polynom&0x00FF);
	//For all the possible “message” combinations
	for (nachricht=0; nachricht <= 0xFF; nachricht++){
		crc=(unsigned char)nachricht;
		//For all the bits of the byte.
		for(bitindex=0; bitindex<8; bitindex++){
			//Calculation of the CRC
			if((crc&0x80)!=0)	{
				crc <<= 1;
				crc ^= reduziertes_polynom;
			}
			else
				crc <<=1;
		}
		//The value out of the CRC calculation for a certain “message” is saved in the
		//position of the “message”.
		*(crc_tabelle+nachricht) = crc;
	}
}

