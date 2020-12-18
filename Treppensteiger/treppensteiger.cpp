#ifdef __linux__
	#include <unistd.h>
#endif

#include "treppensteiger.hpp"

int Treppensteiger::init(){
	if(systeminfo.init() == -1){
		printf("Fehler in systeminfo.init()\n");
		return -1;
	}
	if(datenlogger.init() == -1){
		printf("Fehler in datenlogger.init()\n");
		return -1;
	}
	if(uart_schnittstelle.init() == -1){
		printf("Fehler in uart_schnittstelle.init()\n");
		return -1;
	}
	if(gpio_schnittstelle.init() == -1){
		printf("Fehler in gpio_schnittstelle.init()\n");
		return -1;
	}
	if(rgb_led.init() == -1){
		printf("Fehler in rgb_led.init()\n");
		return -1;
	}
	if(spi_schnittstelle.init() == -1){
		printf("Fehler in spi_schnittstelle.init()\n");
		return -1;
	}	
	if(i2c_schnittstelle.init() == -1){
		printf("Fehler in i2c_schnittstelle.init()\n");
		return -1;
	}
	if(stuhl.imu.init() == -1){
		printf("Fehler in stuhl.imu.init()\n");
		return -1;
	}	
	if(imu_fahrgestell.init() == -1){
		printf("Fehler in imu_fahrgestell.init()\n");
		return -1;
	}
	if(netzwerk.init() == -1){
		printf("Fehler in netzwerk.init()\n");
		return -1;
	}
	if(controller.init() == -1){
		printf("Fehler in controller.init()\n");
		return -1;
	}	
	
	simuliere_treppenaufstieg();
	
	controller.eingabesperre();
	
	telemetrie.starten();
	
	motoren_stoppen();
	
	dreistern_links.motortreiber_steig.leistungsbegrenzung				= 0.25;
	dreistern_links.motortreiber_antrieb.leistungsbegrenzung			= 0.25;
	dreistern_rechts.motortreiber_steig.leistungsbegrenzung				= 0.25;
	dreistern_rechts.motortreiber_antrieb.leistungsbegrenzung			= 0.25;
	arm_vorne.lenkung.dreistern.motortreiber_steig.leistungsbegrenzung	= 0.60;
	arm_hinten.lenkung.dreistern.motortreiber_steig.leistungsbegrenzung	= 0.60;
	arm_vorne.lenkung.motortreiber.leistungsbegrenzung					= 1.0;
	arm_hinten.lenkung.motortreiber.leistungsbegrenzung					= 1.0;
	arm_vorne.stellglied.motortreiber.leistungsbegrenzung				= 1.0;
	arm_hinten.stellglied.motortreiber.leistungsbegrenzung				= 1.0;
	stuhl.motortreiber_links.leistungsbegrenzung						= 1.0;
	stuhl.motortreiber_rechts.leistungsbegrenzung						= 1.0;
	
	dreistern_links.motortreiber_steig.antrieb_invertieren				= true;
	dreistern_links.motortreiber_antrieb.antrieb_invertieren			= true;
	dreistern_rechts.motortreiber_steig.antrieb_invertieren				= true;
	dreistern_rechts.motortreiber_antrieb.antrieb_invertieren			= false;
	arm_vorne.lenkung.dreistern.motortreiber_steig.antrieb_invertieren	= false;
	arm_hinten.lenkung.dreistern.motortreiber_steig.antrieb_invertieren	= true;
	arm_vorne.lenkung.motortreiber.antrieb_invertieren					= false;
	arm_hinten.lenkung.motortreiber.antrieb_invertieren					= false;
	arm_vorne.stellglied.motortreiber.antrieb_invertieren				= false;
	arm_hinten.stellglied.motortreiber.antrieb_invertieren				= false;
	stuhl.motortreiber_links.antrieb_invertieren						= true;
	stuhl.motortreiber_rechts.antrieb_invertieren						= false;
	
	dreistern_links.minimale_stellgroesse_steig			= 0.00;
	dreistern_links.minimale_stellgroesse_antrieb		= 0.00;
	dreistern_rechts.minimale_stellgroesse_steig		= 0.00;
	dreistern_rechts.minimale_stellgroesse_antrieb		= 0.00;
	arm_vorne.lenkung.dreistern.minimale_stellgroesse	= 0.00;
	arm_hinten.lenkung.dreistern.minimale_stellgroesse	= 0.00;
	arm_vorne.lenkung.minimale_stellgroesse				= 0.00;
	arm_hinten.lenkung.minimale_stellgroesse			= 0.00;
	arm_vorne.stellglied.minimale_stellgroesse			= 0.00;
	arm_hinten.stellglied.minimale_stellgroesse			= 0.00;
	stuhl.minimale_stellgroesse							= 0.00;
	
	arm_vorne.stellglied.kraftsensor.nullabweichung = 57719;
	arm_hinten.stellglied.kraftsensor.nullabweichung = 28803;
	arm_vorne.stellglied.kraftsensor.skalierung = 330.8 / 1782291.0;	// skaliert mit 340N - 140N = 200N Armgewicht -> Rohwert: 1444294 <-> (200N * 1,654)
	arm_hinten.stellglied.kraftsensor.skalierung = 330.8 / 1856688.0; 	// skaliert mit 340N - 140N = 200N Armgewicht -> Rohwert: 1470658 <-> (200N * 1,654)
	
	modus = 1;

	return 0;
}

void Treppensteiger::starten(){
	while(modus != 0){
		controller.eingaben_abrufen();
		controller.moduswechsel();
		
		if(winkelsensor_fehler && controller.modus > 1){
			controller.modus = 1;
		}
		
		if(modus != controller.modus){
			modus = controller.modus;
			motoren_stoppen();
			moduswechsel = true;
		}
		else{
			moduswechsel = false;
		}
		
		controller.led_farbe_aktualisieren(moduswechsel);
		
		arm_vorne.lenkung.dreistern.position_anfordern();
		arm_hinten.lenkung.dreistern.position_anfordern();
		
		if(	dreistern_links.winkelsensor_steig.daten_anfordern()	||
			dreistern_links.winkelsensor_antrieb.daten_anfordern()	||
			dreistern_rechts.winkelsensor_steig.daten_anfordern()	||
			dreistern_rechts.winkelsensor_antrieb.daten_anfordern()	||
			arm_vorne.lenkung.winkelsensor.daten_anfordern()		||
			arm_hinten.lenkung.winkelsensor.daten_anfordern()		){
			winkelsensor_fehler = true;
			datenlogger.schreibe_fehlerprotokoll_eintrag(*this, "treppensteiger.cpp: Schwerer Winkelsensorfehler");
		}
		else{
			winkelsensor_fehler = false;
		}
		
		dreistern_links.berechne_antrieb_strecke();
		dreistern_links.berechne_steig_drehwinkel();
		dreistern_rechts.berechne_antrieb_strecke();		
		dreistern_rechts.berechne_steig_drehwinkel();
		
		arm_vorne.lenkung.berechne_stellung();
		arm_hinten.lenkung.berechne_stellung();
		
		pos.x = (dreistern_links.antrieb_strecke + dreistern_rechts.antrieb_strecke) / 2.0;		
		
		Stellglied::daten_anfordern(arm_vorne.stellglied, arm_hinten.stellglied);
		Kraftsensor::daten_anfordern(arm_vorne.stellglied.kraftsensor, arm_hinten.stellglied.kraftsensor);

		arm_vorne.berechne_rotation();
		arm_hinten.berechne_rotation();

		imu_fahrgestell.daten_anfordern();
		imu_fahrgestell.sys_koordinaten_berechnen();	

		berechne_fahrgestell_stellung();
		
		stuhl.imu.daten_anfordern();
		stuhl.imu.sys_koordinaten_berechnen();		
		
		stuhl.berechne_stellung(rot.y);
		
		systeminfo.berechne_prozessorauslastung();
		systeminfo.berechne_schleifenzeit();
		
		switch(modus){
			case 0:
				abschalten();
				break;
			case 1:
				modus_halt();
				break;
			case 2:				
				modus_manuell_horizontal();
				break;
			case 3:
				modus_manuell_vertikal();
				break;
			case 4:
				break;
			case 5:
				if(arme_ausgerichtet){
					modus_automatisch();
				}
				else{
					arme_ausrichten();
				}				
				
				//automatische_stuhlsteuerung();
				
				break;
			default:
				abschalten();
		}
		
		netzwerk.daten_empfangen();
		netzwerk.daten_senden(*this);
		
		datenlogger.aktiv = controller.share_schalter;
		datenlogger.schreibe_winkelsensoren_logeintrag(*this);
		
		telemetrie.aktualisieren(*this);		
	}	
}

void Treppensteiger::motoren_stoppen(){
	dreistern_links.motortreiber_steig.setze_antriebsleistung(0.0);
	dreistern_links.motortreiber_antrieb.setze_antriebsleistung(0.0);
	dreistern_rechts.motortreiber_steig.setze_antriebsleistung(0.0);
	dreistern_rechts.motortreiber_antrieb.setze_antriebsleistung(0.0);
	arm_vorne.lenkung.dreistern.motortreiber_steig.setze_antriebsleistung_bldc(0.0);
	arm_hinten.lenkung.dreistern.motortreiber_steig.setze_antriebsleistung_bldc(0.0);
	arm_vorne.lenkung.motortreiber.setze_antriebsleistung(0.0);
	arm_hinten.lenkung.motortreiber.setze_antriebsleistung(0.0);	
	arm_vorne.stellglied.motortreiber.setze_antriebsleistung(0.0);
	arm_hinten.stellglied.motortreiber.setze_antriebsleistung(0.0);
	stuhl.motortreiber_links.setze_antriebsleistung(0.0);
	stuhl.motortreiber_rechts.setze_antriebsleistung(0.0);
}

void Treppensteiger::abschalten(){
	motoren_stoppen();
	telemetrie.aktualisieren(*this);
	telemetrie.stoppen();
}

void Treppensteiger::modus_halt(){
}

void Treppensteiger::modus_manuell_horizontal(){
	//manuelle_lenkung();
	halbautomatische_momentanpol_lenkung();
	horizontaler_momentanpol_antrieb();
	
	//dreistern_links.motortreiber_antrieb.setze_antriebsleistung(controller.achse[ds4_achse_rechts_vertikal]);
	//dreistern_rechts.motortreiber_antrieb.setze_antriebsleistung(controller.achse[ds4_achse_rechts_vertikal]);
	
	manuelle_stuhlsteuerung();
}

void Treppensteiger::modus_manuell_vertikal(){
	if(controller.ps_knopf_schalter){
		arm_vorne.lenkung.dreistern.motortreiber_steig.setze_antriebsleistung_bldc(controller.achse[ds4_achse_links_vertikal]*arm_vorne.lenkung.dreistern.motortreiber_steig.leistungsbegrenzung);
		arm_hinten.lenkung.dreistern.motortreiber_steig.setze_antriebsleistung_bldc(controller.achse[ds4_achse_rechts_vertikal]*arm_hinten.lenkung.dreistern.motortreiber_steig.leistungsbegrenzung);
		dreistern_links.motortreiber_steig.setze_antriebsleistung(0.0);
		dreistern_rechts.motortreiber_steig.setze_antriebsleistung(0.0);
		
	}
	else{
		arm_vorne.lenkung.dreistern.motortreiber_steig.setze_antriebsleistung_bldc(0.0);
		arm_hinten.lenkung.dreistern.motortreiber_steig.setze_antriebsleistung_bldc(0.0);
		dreistern_links.motortreiber_steig.setze_antriebsleistung(controller.achse[ds4_achse_links_vertikal]*dreistern_links.motortreiber_steig.leistungsbegrenzung);
		dreistern_rechts.motortreiber_steig.setze_antriebsleistung(controller.achse[ds4_achse_rechts_vertikal]*dreistern_rechts.motortreiber_steig.leistungsbegrenzung);
	}
	
	arm_vorne.stellglied.motortreiber.setze_antriebsleistung(controller.achse[ds4_achse_links_horizontal]);
	arm_hinten.stellglied.motortreiber.setze_antriebsleistung(controller.achse[ds4_achse_rechts_horizontal]);
	
	manuelle_stuhlsteuerung();
}

void Treppensteiger::manuelle_stuhlsteuerung(){
	/*
	if(controller.achse[ds4_hebel_r2] < controller.achse[ds4_hebel_l2]){
		stuhl.motortreiber_rechts.setze_antriebsleistung(-controller.achse[ds4_hebel_l2]);		// nach Hinten kippen
		stuhl.motortreiber_links.setze_antriebsleistung(-controller.achse[ds4_hebel_l2]);
	}
	else{
		if(controller.achse[ds4_hebel_r2] > controller.achse[ds4_hebel_l2]){	
			stuhl.motortreiber_rechts.setze_antriebsleistung(controller.achse[ds4_hebel_r2]);	// nach Vorne kippen
			stuhl.motortreiber_links.setze_antriebsleistung(controller.achse[ds4_hebel_r2]);
		}
		else{
			stuhl.motortreiber_rechts.setze_antriebsleistung(0.0);			
			stuhl.motortreiber_links.setze_antriebsleistung(0.0);
		}
	}*/
	
	if(controller.achse[ds4_hebel_r2] < controller.achse[ds4_hebel_l2] && stuhl.drehwinkel > -stuhl.maximaler_drehwinkel){
		stuhl.motortreiber_rechts.setze_antriebsleistung(-controller.achse[ds4_hebel_l2]);		// nach Hinten kippen
		stuhl.motortreiber_links.setze_antriebsleistung(-controller.achse[ds4_hebel_l2]);
	}
	else{
		if(controller.achse[ds4_hebel_r2] > controller.achse[ds4_hebel_l2] && stuhl.drehwinkel < stuhl.maximaler_drehwinkel){	
			stuhl.motortreiber_rechts.setze_antriebsleistung(controller.achse[ds4_hebel_r2]);	// nach Vorne kippen
			stuhl.motortreiber_links.setze_antriebsleistung(controller.achse[ds4_hebel_r2]);
		}
		else{
			stuhl.motortreiber_rechts.setze_antriebsleistung(0.0);			
			stuhl.motortreiber_links.setze_antriebsleistung(0.0);
		}
	}
}

void Treppensteiger::automatische_stuhlsteuerung(){	
	stuhl.regelgroesse = stuhl.drehwinkel;
	stuhl.fuehrungsgroesse = 0.0;
	
	stuhl.pid_regler_setze_stellgroesse(systeminfo.schleifenzeit);
	
	// TODO
	// maximale Auslenkung implementieren
}

void Treppensteiger::manuelle_lenkung(){	
	if(arm_vorne.lenkung.drehwinkel >= -Lenkung::maximaler_drehwinkel && -controller.achse[ds4_achse_links_horizontal] < 0.0){
		arm_vorne.lenkung.motortreiber.setze_antriebsleistung(-controller.achse[ds4_achse_links_horizontal]);	// nach Rechts lenken
	}
	else{
		if(arm_vorne.lenkung.drehwinkel <= Lenkung::maximaler_drehwinkel && -controller.achse[ds4_achse_links_horizontal] >= 0.0){
			arm_vorne.lenkung.motortreiber.setze_antriebsleistung(-controller.achse[ds4_achse_links_horizontal]);	// nach Links lenken
		}
		else{
			arm_vorne.lenkung.motortreiber.setze_antriebsleistung(0.0);	
		}
	}
	if(arm_hinten.lenkung.drehwinkel >= -Lenkung::maximaler_drehwinkel && controller.achse[ds4_achse_links_horizontal] < 0.0){
		arm_hinten.lenkung.motortreiber.setze_antriebsleistung(controller.achse[ds4_achse_links_horizontal]);	// nach Rechts lenken
	}
	else{
		if(arm_hinten.lenkung.drehwinkel <= Lenkung::maximaler_drehwinkel && controller.achse[ds4_achse_links_horizontal] >= 0.0){
			arm_hinten.lenkung.motortreiber.setze_antriebsleistung(controller.achse[ds4_achse_links_horizontal]);	// nach Links lenken
		}
		else{
			arm_hinten.lenkung.motortreiber.setze_antriebsleistung(0.0);	
		}
	}
}

void Treppensteiger::halbautomatische_momentanpol_lenkung(){
	const float abstand_lenkachse_mittelachse = 0.382;
	const float abstand_lenkachse_laengsmittelebene = 0.095;
	
	// PT1-Element
	float T = systeminfo.schleifenzeit;
	const float T_1 = 0.5;
	const float K_P = 1.0;
	static float x_ak = 0.0;	
	float x_ek;
	
	if((-controller.achse[ds4_achse_links_horizontal]) >= 0.0){	// nach links lenken
		x_ek = -controller.achse[ds4_achse_links_horizontal] * (28.8/360.0) * (2*M_PI);
	}
	else{	// nach rechts lenken
		x_ek = -controller.achse[ds4_achse_links_horizontal] * (36.9/360.0) * (2*M_PI);
	}
	
	x_ak = (T_1/(T_1+T))*x_ak + K_P*(T/(T_1+T))*x_ek;
	arm_vorne.lenkung.fuehrungsgroesse = x_ak;
	
	float winkel = 0.0;
	
	if(std::abs(arm_vorne.lenkung.fuehrungsgroesse) > (0.1/360.0) * (2*M_PI)){
		momentanpol = abstand_lenkachse_mittelachse / std::tan(arm_vorne.lenkung.fuehrungsgroesse) - abstand_lenkachse_laengsmittelebene;
		winkel = -std::atan(abstand_lenkachse_mittelachse / (momentanpol - abstand_lenkachse_laengsmittelebene));
	}
	
	winkel = winkel >= (28.8/360.0) * (2*M_PI) ? (28.8/360.0) * (2*M_PI) : winkel;
	winkel = winkel < (-36.9/360.0) * (2*M_PI) ? (-36.9/360.0) * (2*M_PI) : winkel;
	
	arm_hinten.lenkung.fuehrungsgroesse = winkel;
	
	arm_vorne.lenkung.pid_regler_setze_stellgroesse(systeminfo.schleifenzeit);
	arm_hinten.lenkung.pid_regler_setze_stellgroesse(systeminfo.schleifenzeit);
	
	//arm_vorne.lenkung.motortreiber.setze_antriebsleistung(controller.achse[ds4_achse_links_horizontal]);
}

void Treppensteiger::horizontaler_momentanpol_antrieb(){
	dreistern_links.motortreiber_antrieb.leistungsbegrenzung	= 0.5;
	dreistern_rechts.motortreiber_antrieb.leistungsbegrenzung	= 0.5;
	// PT1-Element
	float T = systeminfo.schleifenzeit;
	const float T_1 = 0.5;
	const float K_P = 1.0;
	static float x_ak = 0.0;
	float x_ek = controller.achse[ds4_achse_rechts_vertikal]*dreistern_rechts.motortreiber_antrieb.leistungsbegrenzung;
	
	x_ak = (T_1/(T_1+T))*x_ak + K_P*(T/(T_1+T))*x_ek;
	
	float antriebsleistung_aussen = x_ak;
	float antriebsleistung_innen;
	
	if(momentanpol >= 0){	// Linkskurve
		antriebsleistung_innen = antriebsleistung_aussen * ((0.3 - momentanpol)/(-0.3 - momentanpol));
		
		dreistern_links.motortreiber_antrieb.setze_antriebsleistung(antriebsleistung_innen);
		dreistern_rechts.motortreiber_antrieb.setze_antriebsleistung(antriebsleistung_aussen);				
	}
	else{	// Rechtskurve
		antriebsleistung_innen = antriebsleistung_aussen * ((-0.3 - momentanpol)/(0.3 - momentanpol));
		
		dreistern_links.motortreiber_antrieb.setze_antriebsleistung(antriebsleistung_aussen);
		dreistern_rechts.motortreiber_antrieb.setze_antriebsleistung(antriebsleistung_innen);	
	}
	dreistern_links.motortreiber_antrieb.leistungsbegrenzung	= 0.25;
	dreistern_rechts.motortreiber_antrieb.leistungsbegrenzung	= 0.25;
}

void Treppensteiger::modus_aktive_regelung(){
	/*
	// Steuerung Steigmotor vorne	
	if(bldc_hinten_pos_sollwert < bldc_hinten_pos_istwert + 10 && bldc_hinten_pos_sollwert > bldc_hinten_pos_istwert - 10){
		bldc_hinten_pos_sollwert += -controller.axis[ds4_axis_left_vertical]*0.00001;
	}	
	int stellgroesse = regelung.berechne_stellgroesse_bldc_hinten(bldc_hinten_pos_istwert, (short) bldc_hinten_pos_sollwert, bldc_hinten_pos_diff);
	motorsteuerung.pololu_uart_drive(bldc_controller_hinten, stellgroesse, 2000);			
	usleep(1000);
	
	if(steig_links_sollwert < winkelsensor_dreistern_links_rotation + 5 && steig_links_sollwert > winkelsensor_dreistern_links_rotation - 5){
		steig_links_sollwert += (-controller.axis[ds4_axis_right_vertical]/32767.0)/10.0;
		steig_rechts_sollwert = steig_links_sollwert;
	}
	if(steig_links_sollwert >= 360.0){
		steig_links_sollwert -= 360.0;
	}
	if(steig_links_sollwert < 0.0){
		steig_links_sollwert += 360.0;
	}
	if(steig_rechts_sollwert >= 360.0){
		steig_rechts_sollwert -= 360.0;
	}
	if(steig_rechts_sollwert < 0.0){
		steig_rechts_sollwert += 360.0;
	}
	int stellgroesse_links = regelung.berechne_stellgroesse_steig_links(winkelsensor_dreistern_links_rotation, steig_links_sollwert);
	int stellgroesse_rechts = regelung.berechne_stellgroesse_steig_rechts(winkelsensor_dreistern_rechts_rotation, steig_rechts_sollwert);
	motorsteuerung.pololu_uart_drive(pololu_controller_steig_links, stellgroesse_links, 2000);
	usleep(1000);
	motorsteuerung.pololu_uart_drive(pololu_controller_steig_rechts, stellgroesse_rechts, 2000);	
	usleep(1000);
	*/
}

void Treppensteiger::arme_ausrichten(){
	const float kraft_fuehrungsgroesse = 50.0;	// [N]
	const float maximale_regelabweichung = 50.0;	//[N]
	
	arm_vorne.stellglied.regelgroesse = arm_vorne.stellglied.kraftsensor.kraft;
	arm_hinten.stellglied.regelgroesse = arm_hinten.stellglied.kraftsensor.kraft;
	
	arm_vorne.stellglied.fuehrungsgroesse = kraft_fuehrungsgroesse;
	arm_hinten.stellglied.fuehrungsgroesse = kraft_fuehrungsgroesse;
	
	arm_vorne.stellglied.pid_regler_setze_stellgroesse(systeminfo.schleifenzeit);
	arm_hinten.stellglied.pid_regler_setze_stellgroesse(systeminfo.schleifenzeit);
	
	if(std::abs(arm_vorne.stellglied.regelabweichung) < maximale_regelabweichung && std::abs(arm_hinten.stellglied.regelabweichung) < maximale_regelabweichung){
		arme_ausgerichtet = true;		
	}	
}


void Treppensteiger::modus_automatisch(){	
	sequenzregler.regelgroessen.bldc_vorne_rotation		= arm_vorne.lenkung.dreistern.drehwinkel;
	sequenzregler.regelgroessen.bldc_hinten_rotation	= arm_hinten.lenkung.dreistern.drehwinkel;
	sequenzregler.regelgroessen.steig_links_rotation	= dreistern_links.drehwinkel;
	sequenzregler.regelgroessen.steig_rechts_rotation	= dreistern_rechts.drehwinkel;
	sequenzregler.regelgroessen.antrieb_links_strecke	= pos.x;
	sequenzregler.regelgroessen.antrieb_rechts_strecke	= pos.x;
	sequenzregler.regelgroessen.stuhl_rotation			= stuhl.drehwinkel;
	sequenzregler.regelgroessen.fahrgestell_rotation;
	sequenzregler.regelgroessen.lenkung_vorne_rotation	= arm_vorne.lenkung.drehwinkel;
	sequenzregler.regelgroessen.lenkung_hinten_rotation = arm_hinten.lenkung.drehwinkel;
	sequenzregler.regelgroessen.stellglied_vorne_kraft	= arm_vorne.stellglied.kraftsensor.kraft;
	sequenzregler.regelgroessen.stellglied_hinten_kraft = arm_hinten.stellglied.kraftsensor.kraft;
	//sequenzregler.regelgroessen.arm_vorne_rotation	= arm_vorne.drehwinkel;
	//sequenzregler.regelgroessen.arm_hinten_rotation	= arm_hinten.drehwinkel;
	
	static float inkrementier_zaehler_hemmer = 0.0;
	static int inkrementier_zaehler = 0;
	
	if(sequenzregler.fuehrungsgroessen_datensatz.size() > 0){		
		if(sequenzregler.regeldifferenzen_ausgeregelt()){
			inkrementier_zaehler_hemmer += controller.achse[ds4_hebel_r2];
			inkrementier_zaehler_hemmer -= controller.achse[ds4_hebel_l2];
			
			if(inkrementier_zaehler_hemmer >= 1.0){			
				inkrementier_zaehler = (int)(std::floor(inkrementier_zaehler_hemmer));			
				inkrementier_zaehler_hemmer -= std::floor(inkrementier_zaehler_hemmer);
				
				if((sequenzregler.ausgewaehlter_regeldatensatz + 1) < sequenzregler.fuehrungsgroessen_datensatz.size()){
					sequenzregler.ausgewaehlter_regeldatensatz++;
					fuehrungsgroessen_uebernehmen();
				}
			}
			if(inkrementier_zaehler_hemmer <= -1.0){			
				inkrementier_zaehler = (int)(std::ceil(inkrementier_zaehler_hemmer));			
				inkrementier_zaehler_hemmer -= std::ceil(inkrementier_zaehler_hemmer);
				
				if(sequenzregler.ausgewaehlter_regeldatensatz > 0){
					sequenzregler.ausgewaehlter_regeldatensatz--;
					fuehrungsgroessen_uebernehmen();
				}
			}	
		}
	}
	stellgroessen_setzen();
}

void Treppensteiger::fuehrungsgroessen_uebernehmen(){
	struct zustandsgroessen fuehrungsgroessen = sequenzregler.fuehrungsgroessen_datensatz.at(sequenzregler.ausgewaehlter_regeldatensatz);
	
	arm_vorne.lenkung.dreistern.fuehrungsgroesse	= fuehrungsgroessen.bldc_vorne_rotation;
	arm_hinten.lenkung.dreistern.fuehrungsgroesse	= fuehrungsgroessen.bldc_hinten_rotation;	
	dreistern_links.fuehrungsgroesse_steig			= fuehrungsgroessen.steig_links_rotation;
	dreistern_rechts.fuehrungsgroesse_steig			= fuehrungsgroessen.steig_rechts_rotation;	
	dreistern_links.fuehrungsgroesse_antrieb		= fuehrungsgroessen.antrieb_links_strecke;
	dreistern_rechts.fuehrungsgroesse_antrieb		= fuehrungsgroessen.antrieb_rechts_strecke;	
	stuhl.fuehrungsgroesse							= fuehrungsgroessen.stuhl_rotation;	
	//fuehrungsgroessen.fahrgestell_rotation;	
	arm_vorne.lenkung.fuehrungsgroesse				= fuehrungsgroessen.lenkung_vorne_rotation;
	arm_hinten.lenkung.fuehrungsgroesse				= fuehrungsgroessen.lenkung_hinten_rotation;
	arm_vorne.stellglied.fuehrungsgroesse			= fuehrungsgroessen.stellglied_vorne_kraft;
	arm_hinten.stellglied.fuehrungsgroesse			= fuehrungsgroessen.stellglied_hinten_kraft;
}

void Treppensteiger::stellgroessen_setzen(){
	//dreistern_links.motortreiber_steig.setze_antriebsleistung(dreistern_links.stellgroesse_steig);
	//dreistern_rechts.motortreiber_steig.setze_antriebsleistung(dreistern_rechts.stellgroesse_steig);
	
	//dreistern_links.pid_regler_setze_stellgroesse_steig(systeminfo.schleifenzeit);
	//dreistern_rechts.pid_regler_setze_stellgroesse_steig(systeminfo.schleifenzeit);
	
	dreistern_links.pid_regler_setze_stellgroesse_antrieb(systeminfo.schleifenzeit);
	dreistern_rechts.pid_regler_setze_stellgroesse_antrieb(systeminfo.schleifenzeit);
	arm_vorne.lenkung.dreistern.pid_regler_setze_stellgroesse(systeminfo.schleifenzeit);
	//arm_hinten.lenkung.dreistern.pid_regler_setze_stellgroesse(systeminfo.schleifenzeit);
	
	arm_vorne.stellglied.pid_regler_setze_stellgroesse(systeminfo.schleifenzeit);
	arm_hinten.stellglied.pid_regler_setze_stellgroesse(systeminfo.schleifenzeit);
}

void Treppensteiger::pid_parametrisierung_lenkung(){
	static bool erster_durchlauf = true;	
	if(erster_durchlauf){
		erster_durchlauf = false;		
		arm_vorne.lenkung.Kp = 0.0;
		arm_vorne.lenkung.Ki = 0.0;
		arm_vorne.lenkung.Kd = 0.0;	
	}
	
	arm_vorne.lenkung.fuehrungsgroesse = -controller.achse[ds4_achse_links_horizontal] * Lenkung::maximaler_drehwinkel;	
		
	arm_vorne.lenkung.pid_regler_setze_stellgroesse(systeminfo.schleifenzeit);
	
	arm_vorne.lenkung.Kp += 0.1*controller.achse[ds4_hebel_r2];
	if(arm_vorne.lenkung.Kp >= 0.1*controller.achse[ds4_hebel_l2]){
		arm_vorne.lenkung.Kp -= 0.1*controller.achse[ds4_hebel_l2];
	}
}
void Treppensteiger::pid_parametrisierung_stuhl(){	
	float drehwinkel = stuhl.fuehrungsgroesse;
	float theta = 0.01*controller.achse[ds4_achse_rechts_vertikal];
	
	drehwinkel += theta;	
	if(std::abs(drehwinkel) > Stuhl::maximaler_drehwinkel){
		drehwinkel = (drehwinkel/std::abs(drehwinkel))*Stuhl::maximaler_drehwinkel;
	}
	
	stuhl.fuehrungsgroesse = drehwinkel;
	
	
	arm_vorne.lenkung.pid_regler_setze_stellgroesse(systeminfo.schleifenzeit);
	
	stuhl.Kp += 0.1*controller.achse[ds4_hebel_r2];
	stuhl.Kp -= 0.1*controller.achse[ds4_hebel_l2];
}
void Treppensteiger::pid_parametrisierung_dreistern_seitlich_steig(){	
	float drehwinkel = dreistern_links.fuehrungsgroesse_steig;
	float theta = 0.1*controller.achse[ds4_achse_rechts_vertikal];
	
	drehwinkel += theta;
	if(drehwinkel < 0.0)
		drehwinkel += 2*M_PI;
	if(drehwinkel > (2*M_PI))
		drehwinkel -= 2*M_PI;
	
	dreistern_links.fuehrungsgroesse_steig = drehwinkel;	
	
	dreistern_links.pid_regler_setze_stellgroesse_steig(systeminfo.schleifenzeit);
	
	dreistern_links.Kp_steig += 0.1*controller.achse[ds4_hebel_r2];
	dreistern_links.Kp_steig -= 0.1*controller.achse[ds4_hebel_l2];
}
void Treppensteiger::pid_parametrisierung_dreistern_seitlich_antrieb(){
	static bool erster_durchlauf = true;	
	if(erster_durchlauf){
		erster_durchlauf = false;		
		dreistern_links.Kp_antrieb = 0.0;
		dreistern_links.Ki_antrieb = 0.0;
		dreistern_links.Kd_antrieb = 0.0;		
		dreistern_rechts.Kp_antrieb = 0.0;
		dreistern_rechts.Ki_antrieb = 0.0;
		dreistern_rechts.Kd_antrieb = 0.0;
	}	
	
	float strecke = dreistern_links.fuehrungsgroesse_antrieb;
	float delta = 0.01*controller.achse[ds4_achse_rechts_vertikal];
	
	strecke += delta;
	
	dreistern_links.fuehrungsgroesse_antrieb = strecke;
	dreistern_rechts.fuehrungsgroesse_antrieb = strecke;
	
	dreistern_links.pid_regler_setze_stellgroesse_antrieb(systeminfo.schleifenzeit);
	dreistern_rechts.pid_regler_setze_stellgroesse_antrieb(systeminfo.schleifenzeit);
	
	dreistern_links.Kp_antrieb += 0.1*controller.achse[ds4_hebel_r2];
	dreistern_links.Kp_antrieb -= 0.1*controller.achse[ds4_hebel_l2];	
	dreistern_rechts.Kp_antrieb += 0.1*controller.achse[ds4_hebel_r2];
	dreistern_rechts.Kp_antrieb -= 0.1*controller.achse[ds4_hebel_l2];
}
void Treppensteiger::pid_parametrisierung_bldc(){
	float drehwinkel = arm_vorne.lenkung.dreistern.fuehrungsgroesse;
	float theta = 0.1*controller.achse[ds4_achse_rechts_vertikal];
	
	drehwinkel += theta;
	if(drehwinkel < 0.0)
		drehwinkel += 2*M_PI;
	if(drehwinkel > (2*M_PI))
		drehwinkel -= 2*M_PI;
	
	arm_vorne.lenkung.dreistern.fuehrungsgroesse = drehwinkel;
	
	arm_vorne.lenkung.dreistern.pid_regler_setze_stellgroesse(systeminfo.schleifenzeit);
	
	arm_vorne.lenkung.dreistern.Kp += 0.1*controller.achse[ds4_hebel_r2];
	arm_vorne.lenkung.dreistern.Kp -= 0.1*controller.achse[ds4_hebel_l2];
}
void Treppensteiger::pid_parametrisierung_stellglied(){
	static bool erster_durchlauf = true;	
	if(erster_durchlauf){
		erster_durchlauf = false;		
		arm_vorne.stellglied.Kp = 0.0;
		arm_vorne.stellglied.Ki = 0.0;
		arm_vorne.stellglied.Kd = 0.0;
		arm_hinten.stellglied.Kp = 0.0;
		arm_hinten.stellglied.Ki = 0.0;
		arm_hinten.stellglied.Kd = 0.0;				
	}	
	
	arm_vorne.stellglied.regelgroesse = arm_vorne.stellglied.kraftsensor.kraft;
	arm_hinten.stellglied.regelgroesse = arm_hinten.stellglied.kraftsensor.kraft;
	
	float kraft = arm_vorne.stellglied.fuehrungsgroesse;
	float delta_f = 10.0*controller.achse[ds4_achse_rechts_vertikal];
	
	kraft += delta_f;
	
	if(std::abs(kraft) > Kraftsensor::maximale_kraft){
		kraft = (kraft/std::abs(kraft))*Kraftsensor::maximale_kraft;
	}	
	
	arm_vorne.stellglied.fuehrungsgroesse = kraft;
	arm_hinten.stellglied.fuehrungsgroesse = kraft;
	
	arm_vorne.stellglied.pid_regler_setze_stellgroesse(systeminfo.schleifenzeit);
	arm_hinten.stellglied.pid_regler_setze_stellgroesse(systeminfo.schleifenzeit);
	
	arm_vorne.stellglied.Kp += 0.0001*controller.achse[ds4_hebel_r2];
	arm_hinten.stellglied.Kp += 0.0001*controller.achse[ds4_hebel_r2];
	
	if(arm_vorne.stellglied.Kp >= 0.0001*controller.achse[ds4_hebel_l2]){
		arm_vorne.stellglied.Kp -= 0.0001*controller.achse[ds4_hebel_l2];			
		arm_hinten.stellglied.Kp -= 0.0001*controller.achse[ds4_hebel_l2];
	}
}

void Treppensteiger::motor_kennlinienbestimmung(){
	static bool kraft_aufbauen = true;
	static float leistung = 0.0;
	const float maximale_leistung = 0.7;
	
	if(kraft_aufbauen){
		if(leistung < maximale_leistung){
			leistung += 0.0005;
		}
		else{
			kraft_aufbauen = false;
		}
	}
	else{
		if(leistung > 0.0){
			leistung -= 0.0005;
		}
		else{
			leistung = 0.0;
		}
	}
	
	arm_vorne.lenkung.dreistern.motortreiber_steig.setze_antriebsleistung(leistung);
}

void Treppensteiger::rotieren(Matrix &mat_drehung){
	lage = Matrix::mat_vek_mult(mat_drehung, lage);
	richtung = Matrix::mat_vek_mult(mat_drehung, richtung);
	mittel_achse = Matrix::mat_vek_mult(mat_drehung, mittel_achse);
	
	stuhl.rotieren(mat_drehung);
	
	arm_vorne.rotieren(mat_drehung);
	arm_hinten.rotieren(mat_drehung);
	
	dreistern_links.rotieren(mat_drehung);
	dreistern_rechts.rotieren(mat_drehung);       
}

void Treppensteiger::drehen(Vektor achse, float theta){
	Matrix mat_drehung = Matrix::rotation_mat(achse, theta);
	
	lage = Matrix::mat_vek_mult(mat_drehung, lage);
	richtung = Matrix::mat_vek_mult(mat_drehung, richtung);
	mittel_achse = Matrix::mat_vek_mult(mat_drehung, mittel_achse);
	
	// unsafe
	rot.y += theta;
	//
	
	stuhl.rotieren(mat_drehung);
	
	arm_vorne.rotieren(mat_drehung);
	arm_hinten.rotieren(mat_drehung);
	
	dreistern_links.rotieren(mat_drehung);
	dreistern_rechts.rotieren(mat_drehung);       
}

bool Treppensteiger::kollision_boden_rad(float bodenhoehe, Vektor &rad_pos){
	Vektor normalenvektor_boden(0.0, 0.0, 1.0);	// ebener Boden
	
	float abstand = Vektor::abstand_punkt_ebene(rad_pos, normalenvektor_boden, -bodenhoehe);
    
    if(abstand < 0.0625)
        return true;
    else
        return false;
}

bool Treppensteiger::kollision_stufe_rad(Stufe &stufe, Vektor &rad_pos, float offset_vorne){
	Vektor normalenvektor_trittflaeche(0.0, 0.0, 1.0);	// planebene Trittflaeche

    float abstand_stirnflaeche_ursprung = -Vektor::skalarprodukt(stufe.richtung, stufe.position);
    float abstand_trittflaeche_ursprung = -Vektor::skalarprodukt(normalenvektor_trittflaeche, stufe.position);
    
    float abstand1 = Vektor::abstand_punkt_ebene(rad_pos, stufe.richtung, abstand_stirnflaeche_ursprung);
    float abstand2 = Vektor::abstand_punkt_ebene(rad_pos, normalenvektor_trittflaeche, abstand_trittflaeche_ursprung);
	
	//printf("abstand_stirnflaeche_ursprung: %f", abstand1);
	//printf("abstand_trittflaeche_ursprung: %f\n", abstand2);
        
    if(abstand1 < (0.0625 + offset_vorne) && abstand2 < 0.0625)
        return true;
    else
        return false;
}

bool Treppensteiger::kollision_treppe_rad(Treppe &treppe, Vektor rad_pos, float offset_vorne){
	bool kollision = kollision_boden_rad(treppe.bodenhoehe, rad_pos);
    
	for(int i=0; i<treppe.stufen.size(); i++){
		kollision = kollision || kollision_stufe_rad(treppe.stufen[i], rad_pos, offset_vorne);
	}
        
    return kollision;
}

bool Treppensteiger::kollision_treppe_raeder(Treppe &treppe, Dreistern_Seitlich &dreistern_seitlich, float offset_vorne){
	bool kollision = false;
	
	for(int i=0; i<3; i++){
		kollision = kollision || kollision_treppe_rad(treppe, dreistern_seitlich.rad_pos(i, pos), offset_vorne);
	}
	return kollision;
}

bool Treppensteiger::kollision_treppe_raeder(Treppe &treppe, Arm &arm, float offset_vorne){
	bool kollision = false;
	
	for(int i=0; i<3; i++){
		kollision = kollision || kollision_treppe_rad(treppe, arm.rad_pos(i, pos), offset_vorne);
	}
	return kollision;
}

bool Treppensteiger::kollision_treppe_raeder(Treppe &treppe, float offset_vorne){
	bool kollision = false;
	
	for(int i=0; i<3; i++){
		kollision = kollision || kollision_treppe_rad(treppe, dreistern_links.rad_pos(i, pos), offset_vorne);
	}
	for(int i=0; i<3; i++){
		kollision = kollision || kollision_treppe_rad(treppe, dreistern_rechts.rad_pos(i, pos), offset_vorne);
	}
	for(int i=0; i<3; i++){
		kollision = kollision || kollision_treppe_rad(treppe, arm_vorne.rad_pos(i, pos), offset_vorne);
	}
	for(int i=0; i<3; i++){
		kollision = kollision || kollision_treppe_rad(treppe, arm_hinten.rad_pos(i, pos), offset_vorne);
	}
	
	return kollision;
}

void Treppensteiger::simuliere_treppenaufstieg(){	
	printf("\nBeginne Treppensteiger Simulation...\n");
	fflush(stdout);
	
	stuhl.drehwinkel = 0.0;
	
	bool kollision;
	int anzahl_schritte = 0;
	int timeout = 0;
	const int max_schritte = 10000;
	float delta_theta_dreistern_rotation = 0.5 * (M_PI/180.0);	// 0,5°
	float delta_theta_arm_rotation = 0.25 * (M_PI/180.0);
	float delta_theta_treppensteiger_rotation = 0.125 * (M_PI/180.0);
	float maximaler_arm_drehwinkel_zum_fahrgestell = 20.0 * (M_PI/180.0);	// 20,0°
	float rotation_kompensationsstrecke = 0.0;
	
	
	while(anzahl_schritte < max_schritte && pos.x < 3.2){
		// Kollision fuer alle Raeder berechnen und Treppensteiger vorfahren, falls keine Kollision
		//********************************************************************************************
		kollision = false;
		kollision = kollision || kollision_treppe_raeder(treppe, 0.08);		// Kollisionstest Treppe <-> Raeder vom vorderen Dreistern, Back-off von 8cm
				
		if(!kollision){
			pos.x += 0.002;	// Simulierten Treppensteiger um 5mm vorfahren			
		}
		//********************************************************************************************
		
		// Vorderen Arm ausrichten
		//********************************************************************************************    
		timeout = 0; // Arm runterfahren bis Kollision
		//while(!kollision_treppe_raeder(treppe, 0.0) && timeout < 2){		// timeout fuer Fehlerhafte Simulation
		while(!kollision_treppe_raeder(treppe, arm_vorne, 0.0) && timeout < 2){		// timeout fuer Fehlerhafte Simulation
			arm_vorne.stellen(delta_theta_arm_rotation);								// Frontarm des Simulationsmodells drehen
			arm_vorne.lenkung.dreistern.drehen(-delta_theta_arm_rotation);			// Vorderen Dreistern rotieren, um Drehung durch Armrotation zu kompensieren
			timeout += 1;
		}
			
		timeout = 0; // Arm hochfahren bis keine Kollision
		//while(kollision_treppe_raeder(treppe, 0.0) && timeout < 2){
		while(kollision_treppe_raeder(treppe, arm_vorne, 0.0) && timeout < 2){
			arm_vorne.stellen(-delta_theta_arm_rotation);
			arm_vorne.lenkung.dreistern.drehen(delta_theta_arm_rotation);
			timeout += 1;
		}
		//********************************************************************************************   
		
			
		// Teste Kollision am vorderen Dreistern und drehe falls Kollision vorhanden
		//********************************************************************************************
		kollision = kollision_treppe_raeder(treppe, arm_vorne, 0.08);
		if(kollision){
			arm_vorne.lenkung.dreistern.drehen(delta_theta_dreistern_rotation);
		}
		//********************************************************************************************

		
		// Vorderen Arm ausrichten
		//********************************************************************************************     
		timeout = 0; // Arm runterfahren bis Kollision
		while(!kollision_treppe_raeder(treppe, arm_vorne, 0.0) && timeout < 2){
			arm_vorne.stellen(delta_theta_arm_rotation);
			arm_vorne.lenkung.dreistern.drehen(-delta_theta_arm_rotation);
			timeout += 1;
		}
			
		timeout = 0; // Arm hochfahren bis keine Kollision
		while(kollision_treppe_raeder(treppe, arm_vorne, 0.0) && timeout < 2){
			arm_vorne.stellen(-delta_theta_arm_rotation);
			arm_vorne.lenkung.dreistern.drehen(delta_theta_arm_rotation);
			timeout += 1;
		}
		//********************************************************************************************
		
		
		// Teste Kollision am linken/rechten Dreistern und drehe falls Kollision vorhanden    
		//********************************************************************************************
		kollision = kollision_treppe_raeder(treppe, dreistern_links, 0.08);
		if(kollision){
			dreistern_links.drehen(delta_theta_dreistern_rotation);
			dreistern_rechts.drehen(delta_theta_dreistern_rotation);
			rotation_kompensationsstrecke = -(delta_theta_dreistern_rotation/(2*M_PI)) * Dreistern::rad_umfang;
		}
		//********************************************************************************************
		
			
		// Teste Kollision am linken/rechten Dreistern und hebe den Treppensteiger falls Kollision vorhanden
		//********************************************************************************************
		kollision = kollision_treppe_raeder(treppe, dreistern_links, 0.0);
		if(kollision){
			pos.z += 0.002;
		}
		//********************************************************************************************
		
		// Fahrgestell kippen, falls Arme zu weit gewinkelt
		//********************************************************************************************
		timeout = 0; // Treppensteiger kippen, falls vorderer Arm zu weit angewinkelt (nach hinten)
		while(arm_vorne.drehwinkel < -maximaler_arm_drehwinkel_zum_fahrgestell && timeout < 2){
			drehen(mittel_achse, -delta_theta_treppensteiger_rotation);
			stuhl.stellen(delta_theta_treppensteiger_rotation);								// Kippen am Stuhl kompensieren			
			arm_hinten.lenkung.dreistern.drehen(delta_theta_treppensteiger_rotation);			
			arm_vorne.lenkung.dreistern.drehen(delta_theta_treppensteiger_rotation);			
			dreistern_links.drehen(delta_theta_treppensteiger_rotation);
			dreistern_rechts.drehen(delta_theta_treppensteiger_rotation);
			rotation_kompensationsstrecke = -(delta_theta_dreistern_rotation/(2*M_PI)) * Dreistern::rad_umfang;
			rotation_kompensationsstrecke /= 1000.0;
			timeout += 1;
		}
			
		timeout = 0; // Treppensteiger aufrichten, falls vorderer Arm weiter anwinkelbar (nach vorne)
		while(arm_vorne.drehwinkel > -maximaler_arm_drehwinkel_zum_fahrgestell && arm_vorne.drehwinkel < 0.0 && rot.y < 0.0 && timeout < 2){
			drehen(mittel_achse, delta_theta_treppensteiger_rotation);			
			stuhl.stellen(-delta_theta_treppensteiger_rotation);							// Kippen am Stuhl kompensieren			
			arm_hinten.lenkung.dreistern.drehen(-delta_theta_treppensteiger_rotation);			
			arm_vorne.lenkung.dreistern.drehen(-delta_theta_treppensteiger_rotation);			
			dreistern_links.drehen(-delta_theta_treppensteiger_rotation);   
			dreistern_rechts.drehen(-delta_theta_treppensteiger_rotation);
			rotation_kompensationsstrecke = (delta_theta_dreistern_rotation/(2*M_PI)) * Dreistern::rad_umfang;
			rotation_kompensationsstrecke /= 1000.0;
			timeout += 1;
		}
		//********************************************************************************************
		
		// Hinteren Arm ausrichten
		//********************************************************************************************    
		timeout = 0; // Arm runterfahren bis Kollision
		//while(!kollision_treppe_raeder(treppe, 0.0) && timeout < 2){							// timeout fuer Fehlerhafte Simulation
		while(!kollision_treppe_raeder(treppe, arm_hinten, 0.0) && timeout < 2){				// timeout fuer Fehlerhafte Simulation
			arm_hinten.stellen(-delta_theta_arm_rotation);										// Heckarm des Simulationsmodells drehen
			arm_hinten.lenkung.dreistern.drehen(delta_theta_arm_rotation);						// Hinteren Dreistern rotieren, um Drehung durch Armrotation zu kompensieren
			timeout += 1;
		}
			
		timeout = 0; // Arm hochfahren bis keine Kollision
		//while(kollision_treppe_raeder(treppe, 0.0) && timeout < 2){
		while(kollision_treppe_raeder(treppe, arm_hinten, 0.0) && timeout < 2){
			arm_hinten.stellen(delta_theta_arm_rotation);
			arm_hinten.lenkung.dreistern.drehen(-delta_theta_arm_rotation);
			timeout += 1;
		}
		//********************************************************************************************   
		
			
		// Teste Kollision am hinteren Dreistern und drehe falls Kollision vorhanden
		//********************************************************************************************
		kollision = kollision_treppe_raeder(treppe, arm_hinten, 0.08);
		if(kollision){
			arm_hinten.lenkung.dreistern.drehen(delta_theta_dreistern_rotation);
		}
		//********************************************************************************************

		
		// Hinteren Arm ausrichten
		//********************************************************************************************     
		timeout = 0; // Arm runterfahren bis Kollision
		while(!kollision_treppe_raeder(treppe, arm_hinten, 0.0) && timeout < 2){
			arm_hinten.stellen(-delta_theta_arm_rotation);
			arm_hinten.lenkung.dreistern.drehen(delta_theta_arm_rotation);
			timeout += 1;
		}
			
		timeout = 0; // Arm hochfahren bis keine Kollision
		while(kollision_treppe_raeder(treppe, arm_hinten, 0.0) && timeout < 2){
			arm_hinten.stellen(delta_theta_arm_rotation);
			arm_hinten.lenkung.dreistern.drehen(-delta_theta_arm_rotation);
			timeout += 1;
		}
		//********************************************************************************************
		
		
		struct zustandsgroessen fuehrungsgroessen;
		fuehrungsgroessen.bldc_vorne_rotation = arm_vorne.lenkung.dreistern.drehwinkel;
		fuehrungsgroessen.bldc_hinten_rotation = arm_hinten.lenkung.dreistern.drehwinkel;
		fuehrungsgroessen.steig_links_rotation = dreistern_links.drehwinkel;
		fuehrungsgroessen.steig_rechts_rotation = dreistern_rechts.drehwinkel;
		fuehrungsgroessen.antrieb_links_strecke = pos.x + rotation_kompensationsstrecke;
		fuehrungsgroessen.antrieb_rechts_strecke = pos.x + rotation_kompensationsstrecke;
		fuehrungsgroessen.stuhl_rotation = stuhl.drehwinkel;
		fuehrungsgroessen.fahrgestell_rotation;
		fuehrungsgroessen.lenkung_vorne_rotation = arm_vorne.lenkung.drehwinkel;
		fuehrungsgroessen.lenkung_hinten_rotation = arm_hinten.lenkung.drehwinkel;
		fuehrungsgroessen.arm_vorne_rotation = arm_vorne.drehwinkel;
		fuehrungsgroessen.arm_hinten_rotation = arm_hinten.drehwinkel;
		fuehrungsgroessen.stellglied_vorne_kraft = 50.0;
		fuehrungsgroessen.stellglied_hinten_kraft = 50.0;
		
		fuehrungsgroessen.pos_x = pos.x;
		fuehrungsgroessen.pos_y = pos.y;
		fuehrungsgroessen.pos_z = pos.z;
		fuehrungsgroessen.rot_x = rot.x;
		fuehrungsgroessen.rot_y = rot.y;
		fuehrungsgroessen.rot_z = rot.z;
		
		sequenzregler.fuehrungsgroessen_datensatz.push_back(fuehrungsgroessen);		
		
		anzahl_schritte++;
	}
	printf("Simulation mit %d Schritten beendet.", anzahl_schritte);
	fflush(stdout);
	
	pos = Vektor(0.0, 0.0, 0.135);
	rot = Vektor(0.0, 0.0, 0.0);
	lage = Vektor(0.0, 0.0, 1.0);
	richtung = Vektor(1.0, 0.0, 0.0);
}

Matrix Treppensteiger::berechne_imu_rotationsmatrix(Vektor rohwert_ausgangsstellung, Vektor rohwert_schieflage_vorne){
	static bool erste_kommandozeilenausgabe = true;
	if(erste_kommandozeilenausgabe){
		printf(HIDE_CURSOR);
		printf(CLEAR_SCREEN);
		printf(CURSOR_TO_BEGINNING);
		fflush(stdout);
		erste_kommandozeilenausgabe = false;
	}
	
	printf("Berechne Rotationsmatrix...");
	fflush(stdout);	
	
	double theta;
	double aufloesung = ((0.1/360.0)*(2*M_PI));	// 0.1°
	Vektor zielausrichtung(0.0, 0.0, 1.0);
	Vektor v_tmp1, v_tmp2, v_tmp3;
	Matrix rot_mat1, rot_mat2, rot_mat3, zwischenergebnis;
	Matrix ergebnis = Matrix::nullmatrix();
	
	Vektor orthogonaler_vektor = Vektor::kreuzprodukt(zielausrichtung, rohwert_ausgangsstellung);
	orthogonaler_vektor.ausgeben();
	theta = Vektor::winkel(zielausrichtung, rohwert_ausgangsstellung);
	rot_mat1 = Matrix::rotation_mat(orthogonaler_vektor, -theta);
	printf("%12.10f\n", theta);
	rot_mat1.ausgeben();
	//v_tmp = Matrix::mat_vek_mult(rot_mat, rohwert_ausgangsstellung);
	v_tmp1 = Matrix::mat_vek_mult(rot_mat1, rohwert_schieflage_vorne);
	//printf("Beginne Berechnung der Roationsmatrix...\n");
	for(int i = 0; i<3600; i++){
		theta = i * aufloesung;
		rot_mat2 = Matrix::rotation_mat(zielausrichtung, theta);
		v_tmp2 = Matrix::mat_vek_mult(rot_mat2, v_tmp1);
		if(v_tmp2.x < 0.0){
			if(i == 0){
				v_tmp3 = v_tmp2;
			}
			else{
				if(std::abs(v_tmp2.y) < std::abs(v_tmp3.y)){
					v_tmp3 = v_tmp2;
					ergebnis = rot_mat2 * rot_mat1;
				}
			}
		}
	}
	printf("Approximierte Rotationsmatrix mit absoluter Abweichung von y = %12.10f\n", v_tmp3.y);
	ergebnis.ausgeben();
	printf("Berechnung beendet.\n");
	fflush(stdout);
	
	return ergebnis;
}

void Treppensteiger::berechne_fahrgestell_stellung(){
	Vektor v1(0.0, 0.0, 0.0);
	Vektor z_achse(0.0, 0.0, 1.0);
	
	if(std::abs(imu_fahrgestell.sys_acc.betrag() - 1000.0) < 300.0){
		v1 = imu_fahrgestell.sys_acc;
		v1.y = 0.0;
		v1.normieren();
		
		if(v1.x >= 0.0){
			rot.y = -Vektor::winkel(v1, z_achse);	// nach vorne kippen
		}
		else{
			rot.y = Vektor::winkel(v1, z_achse);	// nach hinten kippen
		}		
	}
}