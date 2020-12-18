#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>

#include "motortreiber.hpp"

unsigned char* Motortreiber::p_tx_buffer = NULL;

void Motortreiber::setze_antriebsleistung(float antriebsleistung){	// antriebsleistung [-1, 1]	
	if(antriebsleistung > leistungsbegrenzung)	antriebsleistung =  leistungsbegrenzung;
	if(antriebsleistung < -leistungsbegrenzung)	antriebsleistung = -leistungsbegrenzung;	
	
	this->antriebsleistung = antriebsleistung;
	antriebsleistung_motorkennlinie = antriebsleistung;
	
	antriebsleistung = antrieb_invertieren ? -antriebsleistung : antriebsleistung;
	
	antriebsleistung_pololu_format = (int)(antriebsleistung*600.0);
	antriebsleistung_pololu_format += 2048;

	p_tx_buffer = &tx_buffer[0];
	*p_tx_buffer++ = 0xAA;
	*p_tx_buffer++ = geraetenummer;
	*p_tx_buffer++ = 0x40 + (antriebsleistung_pololu_format & 0x1F);
	*p_tx_buffer++ = (antriebsleistung_pololu_format >> 5) & 0x7F;		
	*p_tx_buffer++ = getCRC(&tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
	
	#ifdef __linux__
		if (uart_schnittstelle->fd != -1){
			int count = write(uart_schnittstelle->fd, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));		//Filestream, bytes to write, number of bytes to write
			if (count < 0){
				printf("UART TX error\n");
			}
		}
		usleep(5000);
	#endif
}

void Motortreiber::setze_antriebsleistung_bldc(float antriebsleistung){	// antriebsleistung [-1, 1]	
	if(antriebsleistung > leistungsbegrenzung)	antriebsleistung =  leistungsbegrenzung;
	if(antriebsleistung < -leistungsbegrenzung)	antriebsleistung = -leistungsbegrenzung;	
	
	this->antriebsleistung = antriebsleistung;
	
	if(antriebsleistung < -0.15822){
		antriebsleistung_motorkennlinie = (312.53/4500.0)*antriebsleistung - 0.53;
	}
	else{
		if(antriebsleistung < -0.01788){
			antriebsleistung_motorkennlinie = (312.53/170.0)*antriebsleistung - 0.25;
		}
		else{
			if(antriebsleistung < 0.01788){
				antriebsleistung_motorkennlinie = (312.53/20.0)*antriebsleistung;
			}
			else{
				if(antriebsleistung < 0.15822){
					antriebsleistung_motorkennlinie = (312.53/170.0)*antriebsleistung + 0.25;
				}
				else{
					antriebsleistung_motorkennlinie = (312.53/4500.0)*antriebsleistung + 0.53;
				}				
			}
		}	
	}
	
	antriebsleistung_motorkennlinie = antrieb_invertieren ? -antriebsleistung_motorkennlinie : antriebsleistung_motorkennlinie;
	
	antriebsleistung_pololu_format = (int)(antriebsleistung_motorkennlinie*600.0);
	antriebsleistung_pololu_format += 2048;

	p_tx_buffer = &tx_buffer[0];
	*p_tx_buffer++ = 0xAA;
	*p_tx_buffer++ = geraetenummer;
	*p_tx_buffer++ = 0x40 + (antriebsleistung_pololu_format & 0x1F);
	*p_tx_buffer++ = (antriebsleistung_pololu_format >> 5) & 0x7F;		
	*p_tx_buffer++ = getCRC(&tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
	
	#ifdef __linux__
		if (uart_schnittstelle->fd != -1){
			int count = write(uart_schnittstelle->fd, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));		//Filestream, bytes to write, number of bytes to write
			if (count < 0){
				printf("UART TX error\n");
			}
		}
		usleep(5000);
	#endif
}
/*
void Motortreiber::stoppen(){
	p_tx_buffer = &tx_buffer[0];
	*p_tx_buffer++ = 0xAA;
	*p_tx_buffer++ = geraetenummer;
	*p_tx_buffer++ = 0xff;
	*p_tx_buffer++ = getCRC(&tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
	
	if (uart_schnittstelle->fd != -1){
		int count = write(uart_schnittstelle->fd, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));		//Filestream, bytes to write, number of bytes to write
		if (count < 0){
			printf("UART TX error\n");
		}
	}
	usleep(5000);
}*/

void Motortreiber::daten_anfordern(unsigned char offset, unsigned char length){
	p_tx_buffer = &tx_buffer[0];
	*p_tx_buffer++ = 0xAA;
	*p_tx_buffer++ = geraetenummer;
	*p_tx_buffer++ = 0x65;	// Variablenanforderungsbefehl
	*p_tx_buffer++ = offset;
	*p_tx_buffer++ = length;	
	*p_tx_buffer++ = getCRC(&tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
	
	#ifdef __linux__
		if (uart_schnittstelle->fd != -1){
			int count = write(uart_schnittstelle->fd, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));		//Filestream, bytes to write, number of bytes to write
			if (count < 0){
				printf("UART RX error\n");
			}
		}
		usleep(5000);
	#endif
}

void Motortreiber::bldc_position_anfordern(short &position_rohwert){
	daten_anfordern(0, 1);	
	
	//----- CHECK FOR ANY RX BYTES -----
	if(uart_schnittstelle->fd != -1){
		// Read up to 255 characters from the port if they are there
		#ifdef __linux__
			int rx_length = read(uart_schnittstelle->fd, (void*)rx_buffer, 255);		//Filestream, buffer to store in, number of bytes to read (max)
		
		if(rx_length < 0){
			//An error occured (will occur if there are no bytes)
		}
		else{
			if(rx_length == 0){
			//No data waiting
			}
			else{				
				//Bytes received				
				if(rx_length == 3){
					if(getCRC(&rx_buffer[0], 2) == rx_buffer[2]){
						position_rohwert = rx_buffer[1]*256+rx_buffer[0];
					}
				}
			}
		}
		#endif
	}
}

unsigned char Motortreiber::getCRC(unsigned char message[], unsigned char length){
	unsigned char i, j, crc = 0;
	for (i = 0; i < length; i++)
	{
		crc ^= message[i];
		for (j = 0; j < 8; j++)
		{
			if (crc & 1)
			crc ^= CRC7_POLY;
			crc >>= 1;
		}
	}
	return crc;
}