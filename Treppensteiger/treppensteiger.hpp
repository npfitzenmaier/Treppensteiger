#ifndef treppensteiger_H
#define treppensteiger_H

#define _USE_MATH_DEFINES
#include <cmath>

#include "controller.hpp"
#include "dreistern_seitlich.hpp"
#include "dreistern_laengs.hpp"
#include "lenkung.hpp"
#include "matrix.hpp"
#include "motortreiber.hpp"
#include "netzwerk.hpp"
#include "telemetrie.hpp"
#include "treppe.hpp"
#include "vektor.hpp"
#include "winkelsensor.hpp"
#include "arm.hpp"
#include "imu.hpp"
#include "stellglied.hpp"
#include "i2c_schnittstelle.hpp"
#include "uart_schnittstelle.hpp"
#include "spi_schnittstelle.hpp"
#include "sequenzregler.hpp"
#include "stuhl.hpp"
#include "kraftsensor.hpp"
#include "rgb_led.hpp"
#include "datenlogger.hpp"
#include "systeminfo.hpp"

class Treppensteiger{
	public:
		Vektor pos;
		Vektor rot;		// XYZ Euler
		Vektor lage;
		Vektor richtung;
		Vektor mittel_achse;
		
		Dreistern_Seitlich dreistern_links;
		Dreistern_Seitlich dreistern_rechts;		
		
		Arm arm_vorne;
		Arm arm_hinten;
		
		Stuhl stuhl;
		
		IMU imu_fahrgestell;
		
		Datenlogger datenlogger;
		Systeminfo systeminfo;
		
		Sequenzregler sequenzregler;
		
		UART_Schnittstelle uart_schnittstelle;
		GPIO_Schnittstelle gpio_schnittstelle;
		SPI_Schnittstelle spi_schnittstelle;
		I2C_Schnittstelle i2c_schnittstelle;		
		
		Netzwerk netzwerk;
		Telemetrie telemetrie;
		Controller controller;
		
		Treppe treppe;
		
		RGB_LED rgb_led;
		
		int modus;
		bool moduswechsel = false;	
		bool winkelsensor_fehler = false;		
		
		bool arme_ausgerichtet = false;
		
		float momentanpol;
		float kompass_winkel;
		
		Treppensteiger(Vektor &pos, Vektor &lage, Vektor &richtung)
			: pos(pos)
			, lage(lage)
			, richtung(richtung){
				
			rot = Vektor(0.0, 0.0, 0.0);
				
			Motortreiber motortreiber_steig_links(&uart_schnittstelle, pololu_motortreiber_steig_links);
			Motortreiber motortreiber_steig_rechts(&uart_schnittstelle, pololu_motortreiber_steig_rechts);
			Motortreiber motortreiber_antrieb_links(&uart_schnittstelle, pololu_motortreiber_antrieb_links);
			Motortreiber motortreiber_antrieb_rechts(&uart_schnittstelle, pololu_motortreiber_antrieb_rechts);
			Motortreiber motortreiber_lenkung_vorne(&uart_schnittstelle, pololu_motortreiber_lenkung_vorne);
			Motortreiber motortreiber_lenkung_hinten(&uart_schnittstelle, pololu_motortreiber_lenkung_hinten);
			Motortreiber motortreiber_stuhl_links(&uart_schnittstelle, pololu_motortreiber_stuhl_links);
			Motortreiber motortreiber_stuhl_rechts(&uart_schnittstelle, pololu_motortreiber_stuhl_rechts);
			Motortreiber motortreiber_steig_vorne(&uart_schnittstelle, bldc_motortreiber_vorne);
			Motortreiber motortreiber_steig_hinten(&uart_schnittstelle, bldc_motortreiber_hinten);
			Motortreiber motortreiber_stellglied_vorne(&uart_schnittstelle, pololu_stellglied_vorne);
			Motortreiber motortreiber_stellglied_hinten(&uart_schnittstelle, pololu_stellglied_hinten);
			
			Winkelsensor winkelsensor_steig_links(&gpio_schnittstelle, &spi_schnittstelle, GPIO_CS_TLE5012_STEIG_LINKS);
			Winkelsensor winkelsensor_steig_rechts(&gpio_schnittstelle, &spi_schnittstelle, GPIO_CS_TLE5012_STEIG_RECHTS);
			Winkelsensor winkelsensor_antrieb_links(&gpio_schnittstelle, &spi_schnittstelle, GPIO_CS_TLE5012_ANTRIEB_LINKS);
			Winkelsensor winkelsensor_antrieb_rechts(&gpio_schnittstelle, &spi_schnittstelle, GPIO_CS_TLE5012_ANTRIEB_RECHTS);
			Winkelsensor winkelsensor_lenkung_vorne(&gpio_schnittstelle, &spi_schnittstelle, GPIO_CS_TLE5012_LENKUNG_VORNE);
			Winkelsensor winkelsensor_lenkung_hinten(&gpio_schnittstelle, &spi_schnittstelle, GPIO_CS_TLE5012_LENKUNG_HINTEN);
		
			mittel_achse = Vektor::kreuzprodukt(lage, richtung);
			mittel_achse.normieren();
			
			float dreistern_achse_rad_abstand1 = 0.14;	// Abstand vom Dreisternmittelpunkt zum Dreisternradmittelpunkt
			float dreistern_achse_rad_abstand2 = 0.13;
			
			Vektor rpos_dreistern_vorne ( 0.0,   0.095, 0.0 );	// x,y,z
			Vektor rpos_dreistern_hinten( 0.0,  -0.095, 0.0 );
			Vektor rpos_dreistern_links ( 0.0,   0.3  , 0.0 );
			Vektor rpos_dreistern_rechts( 0.0,  -0.3  , 0.0 );
			
			Vektor init_dreistern_achse( 0.0, 1.0, 0.0 );			// initiale Dreisternachslage
			Vektor init_dreistern_rad_richtung( 0.0, 0.0, 1.0 );	// Richtung eines Rades vom Dreisternmittelpunkt aus; orthogonal zur Dreisternachse
			
			Dreistern_Laengs dreistern_vorne 	= Dreistern_Laengs(rpos_dreistern_vorne, init_dreistern_achse, init_dreistern_rad_richtung
													, dreistern_achse_rad_abstand1, motortreiber_steig_vorne);
			Dreistern_Laengs dreistern_hinten 	= Dreistern_Laengs(rpos_dreistern_hinten, init_dreistern_achse, init_dreistern_rad_richtung
													, dreistern_achse_rad_abstand1, motortreiber_steig_hinten);
			
			dreistern_links  = 	Dreistern_Seitlich(rpos_dreistern_links, init_dreistern_achse, init_dreistern_rad_richtung, dreistern_achse_rad_abstand2
									, motortreiber_steig_links, motortreiber_antrieb_links, winkelsensor_steig_links, winkelsensor_antrieb_links);
			dreistern_rechts = 	Dreistern_Seitlich(rpos_dreistern_rechts, init_dreistern_achse, init_dreistern_rad_richtung, dreistern_achse_rad_abstand2
									, motortreiber_steig_rechts, motortreiber_antrieb_rechts, winkelsensor_steig_rechts, winkelsensor_antrieb_rechts);
			
			dreistern_vorne.steig_drehwinkel_invertieren = true;
			dreistern_links.steig_drehwinkel_invertieren = true;
			dreistern_links.antrieb_strecke_invertieren = true;			
									
			Vektor stuhl_rpos;
			stuhl_rpos = lage;
			stuhl_rpos.skalieren(0.6);	// Stuhl in Ausgangsstellung, 60cm ueber Mittelachse
			
			Vektor lenkachse_vorne_rpos (  0.382, -0.095, 0.0 );
			Vektor lenkachse_hinten_rpos( -0.382,  0.095, 0.0 );
			Vektor lenkachse_vorne		(  0.0 ,  0.0 , 1.0 );
			Vektor lenkachse_hinten		(  0.0 ,  0.0 , 1.0 );
			
			float drehwinkel_vorne = 0.0;
			float drehwinkel_hinten = 0.0;
			
			Lenkung lenkung_vorne	= Lenkung(lenkachse_vorne_rpos, lenkachse_vorne, drehwinkel_vorne, dreistern_vorne, motortreiber_lenkung_vorne
										, winkelsensor_lenkung_vorne);
			Lenkung lenkung_hinten	= Lenkung(lenkachse_hinten_rpos, lenkachse_hinten, drehwinkel_hinten, dreistern_hinten, motortreiber_lenkung_hinten
										, winkelsensor_lenkung_hinten);
										
			lenkung_vorne.lenk_drehwinkel_invertieren = true;
			lenkung_hinten.lenk_drehwinkel_invertieren = true;
			
			Kraftsensor::i2c_schnittstelle = &i2c_schnittstelle;
			Stellglied::i2c_schnittstelle = &i2c_schnittstelle;
			Stellglied stellglied_vorne(motortreiber_stellglied_vorne);
			Stellglied stellglied_hinten(motortreiber_stellglied_hinten);			
			
			arm_vorne	=	Arm(mittel_achse, lenkung_vorne, stellglied_vorne);
			arm_hinten	= 	Arm(mittel_achse, lenkung_hinten, stellglied_hinten);
			
			IMU imu_stuhl = IMU(&i2c_schnittstelle, ICM_20948_STUHL);
			imu_fahrgestell = IMU(&i2c_schnittstelle, ICM_20948_FAHRGESTELL);
			
			//Matrix mat_drehung_x = Matrix::rotation_mat(Vektor(1.0, 0.0, 0.0), (90/360.0)*(2.0*M_PI)); 	// 1. Drehung im Gegenuhrzeugersinn
			//Matrix mat_drehung_y = Matrix::rotation_mat(Vektor(0.0, 1.0, 0.0), (90/360.0)*(2.0*M_PI));	// 2. Drehung im Gegenuhrzeugersinn
			//Matrix mat_drehung_z = Matrix::rotation_mat(Vektor(0.0, 0.0, 1.0), (90/360.0)*(2.0*M_PI));	// 3. Drehung im Gegenuhrzeugersinn
			//Matrix mat_drehung_stuhl = berechne_imu_rotationsmatrix(Vektor(-0.6625, -0.7571, -0.0160), Vektor(-0.7919, -0.6214, -0.0148));
			Matrix mat_drehung_stuhl = berechne_imu_rotationsmatrix(Vektor(-0.649, -0.776, -0.022), Vektor(-0.875, -0.498, -0.024));
			
			Matrix mat_drehung_fahrgestell = berechne_imu_rotationsmatrix(Vektor(-1.002, -0.015, 0.001), Vektor(-0.965, -0.020, -0.258));
			//Matrix mat_drehung_fahrgestell = berechne_imu_rotationsmatrix(Vektor(-0.000, -1010, -0.070), Vektor(-0.016, -0.937, -0.373));
			//Matrix mat_drehung_fahrgestell = Matrix::nullmatrix();
			
			imu_stuhl.mat_imu_zu_sys_koordinaten = mat_drehung_stuhl; //mat_drehung_z * mat_drehung_y * mat_drehung_x;
			imu_fahrgestell.mat_imu_zu_sys_koordinaten = mat_drehung_fahrgestell;
			
			stuhl = Stuhl(stuhl_rpos, mittel_achse, imu_stuhl, motortreiber_stuhl_links, motortreiber_stuhl_rechts);
			
			treppe.erstelle_test_treppe();
			
			controller.rpi_rgb_led = &rgb_led;
			
			momentanpol = 1000.0; // [m]
		}
		
		int init();
		void starten();
		void abschalten();
		void motoren_stoppen();
		void manuelle_stuhlsteuerung();
		void automatische_stuhlsteuerung();
		void manuelle_lenkung();
		void halbautomatische_momentanpol_lenkung();
		void horizontaler_momentanpol_antrieb();
		
		void modus_halt();
		void modus_manuell_horizontal();
		void modus_manuell_vertikal();
		void modus_aktive_regelung();
		void modus_automatisch();
		
		void arme_ausrichten();
		
		void frontarm_rotieren(float theta);
		void rotieren(Matrix &mat_drehung);
		void drehen(Vektor achse, float theta);
		
		bool kollision_boden_rad(float bodenhoehe, Vektor &rad_pos);
		bool kollision_stufe_rad(Stufe &stufe, Vektor &rad_pos, float offset_vorne);
		bool kollision_treppe_rad(Treppe &stufe, Vektor rad_pos, float offset_vorne);
		bool kollision_treppe_raeder(Treppe &treppe, Dreistern_Seitlich &dreistern_seitlich, float offset_vorne);
		bool kollision_treppe_raeder(Treppe &treppe, Arm &arm, float offset_vorne);
		bool kollision_treppe_raeder(Treppe &treppe, float offset_vorne);
		
		void simuliere_treppenaufstieg();
		//void simulation_uebertragen();
		//void rechner_datenuebertragung();
		
		void fuehrungsgroessen_uebernehmen();
		void stellgroessen_setzen();
		
		Matrix berechne_imu_rotationsmatrix(Vektor rohwert_ausgangsstellung, Vektor rohwert_schieflage_vorne);		
		
		void berechne_fahrgestell_stellung();
		
		void pid_parametrisierung_lenkung();
		void pid_parametrisierung_stuhl();		
		void pid_parametrisierung_dreistern_seitlich_steig();
		void pid_parametrisierung_dreistern_seitlich_antrieb();
		void pid_parametrisierung_bldc();
		void pid_parametrisierung_stellglied();
		void motor_kennlinienbestimmung();
	private:	
};

#endif