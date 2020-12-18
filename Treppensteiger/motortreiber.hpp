#ifndef motortreiber_H
#define motortreiber_H

#include "uart_schnittstelle.hpp"

#define pololu_motortreiber_steig_links		12
#define pololu_motortreiber_steig_rechts	6
#define pololu_motortreiber_antrieb_links	11
#define pololu_motortreiber_antrieb_rechts	5
#define	pololu_motortreiber_lenkung_vorne	3
#define pololu_motortreiber_lenkung_hinten	9
#define pololu_motortreiber_stuhl_links		2
#define pololu_motortreiber_stuhl_rechts	8
#define pololu_stellglied_vorne				4
#define pololu_stellglied_hinten			15	//10 geht nicht?????!
#define bldc_motortreiber_vorne				14
#define bldc_motortreiber_hinten			13

/*
Yes, all of the B+'s GPIO pins have internal pull-up or pull-down resistors that can be controlled from your code.

Pull-up is 50K min – 65K max. Pull-down is 50K min – 60K max.
To round off your answer it might be worth mentioning that pins 3 and 5 have hard-wired 1k8 pull-ups to 3V3. 
These pull-ups are needed for the proper operation of the I2C bus. 
A consequence is that the internal pull-downs have no effect on these pins (the internal pull-downs are much weaker than the external pull-ups).
*/

// The power on state is defined in BCM2835 ARM Peripherals page 102.
// Basically all GPIO are in INPUT mode, 
// GPIO 0-8 have pull-ups to 3V3 enabled,
// GPIO 9-27 have pull-downs to 0V enabled.

//#define GPIO_UART_BLDC_OE 			21	// BCM 21, Pin 40, power on state: pull-down to 0V enabled

class Motortreiber{
	public:
		float leistungsbegrenzung = 1.0;
		float antriebsleistung;
		float antriebsleistung_motorkennlinie;
		int antriebsleistung_pololu_format;
		bool antrieb_invertieren = false;
		int geraetenummer = 0;
		UART_Schnittstelle* uart_schnittstelle;
		
		Motortreiber(){}
		Motortreiber(UART_Schnittstelle* uart_schnittstelle, int geraetenummer)
			: uart_schnittstelle(uart_schnittstelle)
			, geraetenummer(geraetenummer){}
		
		void setze_antriebsleistung(float antriebsleistung);		// antriebsleistung [-1.0, 1.0]
		void setze_antriebsleistung_bldc(float antriebsleistung);	// antriebsleistung [-1.0, 1.0]
		
		void bldc_position_anfordern(short &position_rohwert);
		
		//void stoppen();
		
		unsigned char getCRC(unsigned char message[], unsigned char length);
	private:	
		static const unsigned char CRC7_POLY = 0x91;
		static unsigned char* p_tx_buffer;		
		unsigned char tx_buffer[16];
		unsigned char rx_buffer[16];
		
		void daten_anfordern(unsigned char offset, unsigned char length);
};

#endif