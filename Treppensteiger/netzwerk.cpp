/*#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring> // memcpy
#include <cmath>*/

#define _USE_MATH_DEFINES
#include <cmath>

#include "netzwerk.hpp"
#include "treppensteiger.hpp"

const double Netzwerk::uebertragungs_timeout = 10.0;

int Netzwerk::init(){	
	sendepufferzeiger = &sendepuffer[0];
	empfangspufferzeiger = &empfangspuffer[0];
	
	#ifdef __linux__
		sockfd = socket(PF_INET, SOCK_DGRAM, 0);
		if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
			//printf ("%s: Kann Socket nicht öffnen ...(%s)\n",
			//argv[0], strerror(errno));
			//exit(EXIT_FAILURE);		
			return -1;
		}
		if(fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0){		// NONBLOCKING
			close(sockfd);
			return -1;
		}
	
		const int y = 1;
	
		server_adresse.sin_family = AF_INET;
		server_adresse.sin_addr.s_addr = htonl(INADDR_ANY);
		server_adresse.sin_port = htons(server_port);
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int));
	
		if(bind(sockfd, (struct sockaddr *) &server_adresse, sizeof(server_adresse)) < 0) {
			//printf ("%s: Kann Portnummern %d nicht binden (%s)\n",
			//argv[0], LOCAL_SERVER_PORT, strerror(errno));
			//exit(EXIT_FAILURE);
			close(sockfd);
			return -1;
		}
		//printf ("%s: Wartet auf Daten am Port (UDP) %u\n", argv[0], LOCAL_SERVER_PORT);*/
	
		//memset(puffer, 0, BUF);
	#endif
	
	return 0;
}

void Netzwerk::blender_uebertragung_starten(){
	#ifdef __linux__
		blender_adresse = client_adresse;
	#endif
	blender_uebertragung = true;
	blender_uebertragungsdauer = 0.0;
}

void Netzwerk::analyse_uebertragung_starten(){
	#ifdef __linux__
		analyse_adresse = client_adresse;
	#endif
	analyse_uebertragung = true;
	analyse_uebertragungsdauer = 0.0;
}

void Netzwerk::display_uebertragung_starten(){
	#ifdef __linux__
		display_adresse = client_adresse;
	#endif
	display_uebertragung = true;
	display_uebertragungsdauer = 0.0;
}

void Netzwerk::blenderdaten_senden(Treppensteiger &ts){
	if(ts.modus == 4){
		static float inkrementier_zaehler_hemmer = 0.0;
		static int inkrementier_zaehler = 0;
		
		if(ts.sequenzregler.fuehrungsgroessen_datensatz.size() > 0){
			inkrementier_zaehler_hemmer += 5.0*ts.controller.achse[ds4_hebel_r2];
			inkrementier_zaehler_hemmer -= 5.0*ts.controller.achse[ds4_hebel_l2];
			
			if(inkrementier_zaehler_hemmer >= 1.0){			
				inkrementier_zaehler = (int)(std::floor(inkrementier_zaehler_hemmer));			
				inkrementier_zaehler_hemmer -= std::floor(inkrementier_zaehler_hemmer);
				
				if((ts.sequenzregler.ausgewaehlter_simulationsdatensatz + inkrementier_zaehler) < ts.sequenzregler.fuehrungsgroessen_datensatz.size()){
					ts.sequenzregler.ausgewaehlter_simulationsdatensatz += inkrementier_zaehler;
				}
				else{
					ts.sequenzregler.ausgewaehlter_simulationsdatensatz = ts.sequenzregler.fuehrungsgroessen_datensatz.size() - 1;
				}
			}
			if(inkrementier_zaehler_hemmer <= -1.0){			
				inkrementier_zaehler = (int)(std::ceil(inkrementier_zaehler_hemmer));			
				inkrementier_zaehler_hemmer -= std::ceil(inkrementier_zaehler_hemmer);
				
				if((ts.sequenzregler.ausgewaehlter_simulationsdatensatz + inkrementier_zaehler) >= 0){
					ts.sequenzregler.ausgewaehlter_simulationsdatensatz += inkrementier_zaehler;
				}
				else{
					ts.sequenzregler.ausgewaehlter_simulationsdatensatz = 0;
				}
			}
		}
		else{
			return;
		}
		
		struct zustandsgroessen &datensatz = ts.sequenzregler.fuehrungsgroessen_datensatz.at(ts.sequenzregler.ausgewaehlter_simulationsdatensatz);
		
		sendepuffer_fuellen(datensatz.bldc_vorne_rotation);
		sendepuffer_fuellen(-datensatz.bldc_hinten_rotation);
		sendepuffer_fuellen(datensatz.steig_links_rotation);
		sendepuffer_fuellen(-datensatz.steig_rechts_rotation);
		sendepuffer_fuellen(-datensatz.lenkung_vorne_rotation);
		sendepuffer_fuellen(datensatz.lenkung_hinten_rotation);
		sendepuffer_fuellen(-datensatz.stuhl_rotation);
		
		sendepuffer_fuellen(-datensatz.arm_vorne_rotation);
		sendepuffer_fuellen(datensatz.arm_hinten_rotation);
		
		sendepuffer_fuellen(datensatz.pos_x);
		sendepuffer_fuellen(datensatz.pos_y);
		sendepuffer_fuellen(datensatz.pos_z);
		
		sendepuffer_fuellen(datensatz.rot_x);		// XYZ Euler
		sendepuffer_fuellen(datensatz.rot_y);
		sendepuffer_fuellen(datensatz.rot_z);
	}
	else{
		sendepuffer_fuellen(ts.arm_vorne.lenkung.dreistern.drehwinkel);
		sendepuffer_fuellen(-ts.arm_hinten.lenkung.dreistern.drehwinkel);
		sendepuffer_fuellen(ts.dreistern_links.drehwinkel);
		sendepuffer_fuellen(ts.dreistern_rechts.drehwinkel);
		sendepuffer_fuellen(ts.arm_vorne.lenkung.drehwinkel);
		sendepuffer_fuellen(ts.arm_hinten.lenkung.drehwinkel);
		//sendepuffer_fuellen(-ts.stuhl.drehwinkel);
		sendepuffer_fuellen((float)(0.0));
		
		float tmp = 0.0;
		
		sendepuffer_fuellen(tmp);
		sendepuffer_fuellen(tmp);
		
		sendepuffer_fuellen(ts.pos.x);
		sendepuffer_fuellen(ts.pos.y);
		sendepuffer_fuellen(ts.pos.z);
		
		/*sendepuffer_fuellen(ts.rot.x);	// XYZ Euler
		sendepuffer_fuellen(ts.rot.y);
		sendepuffer_fuellen(ts.rot.z);*/
		sendepuffer_fuellen((float)(0.0));
		sendepuffer_fuellen((float)(0.0));
		sendepuffer_fuellen((float)(0.0));
	}
		
	float fahrgestell_farbe_r;
	float fahrgestell_farbe_g;
	float fahrgestell_farbe_b;
	
	switch(ts.modus){
		case 0:
			fahrgestell_farbe_r = 0.73;
			fahrgestell_farbe_g = 0.0;
			fahrgestell_farbe_b = 0.0;
			break;
		case 1:
			fahrgestell_farbe_r = 0.73;
			fahrgestell_farbe_g = 0.0;
			fahrgestell_farbe_b = 0.0;
			break;
		case 2:
			fahrgestell_farbe_r = 0.0;
			fahrgestell_farbe_g = 0.73;
			fahrgestell_farbe_b = 0.73;
			break;
		case 3:
			fahrgestell_farbe_r = 0.73;
			fahrgestell_farbe_g = 0.0;
			fahrgestell_farbe_b = 0.73;
			break;
		case 4:							
			fahrgestell_farbe_r = 0.73;
			fahrgestell_farbe_g = 0.73;
			fahrgestell_farbe_b = 0.0;
			break;
		case 5:
			fahrgestell_farbe_r = 0.73;
			fahrgestell_farbe_g = 0.73;
			fahrgestell_farbe_b = 0.0;
			break;
		default:
			fahrgestell_farbe_r = 0.0;
			fahrgestell_farbe_g = 0.0;
			fahrgestell_farbe_b = 0.0;
	}	
	
	sendepuffer_fuellen(fahrgestell_farbe_r);
	sendepuffer_fuellen(fahrgestell_farbe_g);
	sendepuffer_fuellen(fahrgestell_farbe_b);

#ifdef __linux__
	sendto(sockfd, (void*) sendepuffer, (sendepufferzeiger - &sendepuffer[0]), 0, (struct sockaddr*) &blender_adresse, sizeof(blender_adresse));
#endif
	sendepufferzeiger = &sendepuffer[0];
}

void Netzwerk::analysedaten_senden(Treppensteiger &ts){
	analysedatenauswahl = ts.controller.menueauswahl_kreuz_vertikal;
	
	sendepuffer_fuellen(ts.systeminfo.vergangene_us_seit_start);
	sendepuffer_fuellen(analysedatenauswahl);
	
	sendepuffer_fuellen(ts.dreistern_links.fuehrungsgroesse_steig);
	sendepuffer_fuellen(ts.dreistern_links.regelgroesse_steig);
	sendepuffer_fuellen(ts.dreistern_links.stellgroesse_steig);
	
	sendepuffer_fuellen(ts.dreistern_links.fuehrungsgroesse_antrieb);
	sendepuffer_fuellen(ts.dreistern_links.regelgroesse_antrieb);
	sendepuffer_fuellen(ts.dreistern_links.stellgroesse_antrieb);
	
	sendepuffer_fuellen(ts.arm_vorne.lenkung.dreistern.fuehrungsgroesse);	//blau
	sendepuffer_fuellen(ts.arm_vorne.lenkung.dreistern.regelgroesse);		//rot
	sendepuffer_fuellen(ts.arm_vorne.lenkung.dreistern.stellgroesse);		//gelb
	
	sendepuffer_fuellen(ts.arm_vorne.lenkung.fuehrungsgroesse);
	sendepuffer_fuellen(ts.arm_vorne.lenkung.regelgroesse);
	sendepuffer_fuellen(ts.arm_vorne.lenkung.stellgroesse);
	
	sendepuffer_fuellen(ts.arm_vorne.stellglied.fuehrungsgroesse);
	sendepuffer_fuellen(ts.arm_vorne.stellglied.regelgroesse);
	sendepuffer_fuellen(ts.arm_vorne.stellglied.stellgroesse);
	
	sendepuffer_fuellen(ts.stuhl.fuehrungsgroesse);
	sendepuffer_fuellen(ts.stuhl.regelgroesse);
	sendepuffer_fuellen(ts.stuhl.stellgroesse);

	#ifdef __linux__
		sendto(sockfd, (void*) sendepuffer, (sendepufferzeiger - &sendepuffer[0]), 0, (struct sockaddr*) &analyse_adresse, sizeof(analyse_adresse));
	#endif
	sendepufferzeiger = &sendepuffer[0];
}

void Netzwerk::displaydaten_senden(Treppensteiger &ts){
	sendepuffer_fuellen(ts.arm_vorne.lenkung.dreistern.drehwinkel);
	sendepuffer_fuellen(ts.arm_vorne.lenkung.drehwinkel);
	sendepuffer_fuellen(ts.arm_vorne.drehwinkel);
	sendepuffer_fuellen(ts.arm_vorne.stellglied.kraftsensor.kraft);
	
	sendepuffer_fuellen(ts.arm_hinten.lenkung.dreistern.drehwinkel);
	sendepuffer_fuellen(ts.arm_hinten.lenkung.drehwinkel);
	sendepuffer_fuellen(ts.arm_hinten.drehwinkel);
	sendepuffer_fuellen(ts.arm_hinten.stellglied.kraftsensor.kraft);
	
	sendepuffer_fuellen(ts.dreistern_links.drehwinkel);
	sendepuffer_fuellen(ts.dreistern_links.antrieb_strecke);
	sendepuffer_fuellen(ts.dreistern_rechts.drehwinkel);
	sendepuffer_fuellen(ts.dreistern_rechts.antrieb_strecke);
	
	sendepuffer_fuellen(ts.momentanpol);	
	
	if(ts.stuhl.imu.sys_mag.x >= 0.0){
		if(ts.stuhl.imu.sys_mag.y >= 0.0){
			ts.kompass_winkel = std::atan(ts.stuhl.imu.sys_mag.y/ts.stuhl.imu.sys_mag.x);
		}
		else{
			ts.kompass_winkel = std::atan(ts.stuhl.imu.sys_mag.y/ts.stuhl.imu.sys_mag.x) + 2*M_PI;
		}
	}
	else{
		if(ts.stuhl.imu.sys_mag.y >= 0.0){
			ts.kompass_winkel = std::atan(ts.stuhl.imu.sys_mag.y/ts.stuhl.imu.sys_mag.x) + M_PI;
		}
		else{
			ts.kompass_winkel = std::atan(ts.stuhl.imu.sys_mag.y/ts.stuhl.imu.sys_mag.x) + M_PI;
		}
	}
	
	sendepuffer_fuellen(ts.kompass_winkel);
	
	//PT1 Glied einbauen
	
	float fahrgestell_strecke = (ts.dreistern_links.antrieb_strecke + ts.dreistern_rechts.antrieb_strecke) / 2.0;
	static float fahrgestell_strecke_vorher = fahrgestell_strecke;	
	float fahrgestell_geschwindigkeit = ((fahrgestell_strecke - fahrgestell_strecke_vorher)/ts.systeminfo.schleifenzeit) * 3.6;
	fahrgestell_strecke_vorher = fahrgestell_strecke;
	
	
	// PT1-Element
	float T = ts.systeminfo.schleifenzeit;
	const float T_1 = 0.5;
	const float K_P = 1.0;
	static float x_ak = 0.0;	
	float x_ek = ts.controller.achse[ds4_achse_rechts_vertikal] * 10.0;
	
	x_ak = (T_1/(T_1+T))*x_ak + K_P*(T/(T_1+T))*x_ek;
	
	fahrgestell_geschwindigkeit = x_ak;
	sendepuffer_fuellen(fahrgestell_geschwindigkeit);	
	
	#ifdef __linux__
		sendto(sockfd, (void*) sendepuffer, (sendepufferzeiger - &sendepuffer[0]), 0, (struct sockaddr*) &display_adresse, sizeof(display_adresse));
	#endif
	sendepufferzeiger = &sendepuffer[0];
}

void Netzwerk::joystick_daten_auswerten(){
	
}

void Netzwerk::daten_empfangen(){
	#ifdef __linux__
		addrlen = sizeof(client_adresse);
		while((anzahl_empfangener_bytes = recvfrom(sockfd, (void*) empfangspuffer, puffergroesse, 0, (struct sockaddr *) &client_adresse, (socklen_t *) &addrlen)) > 0){
			/*if(errno == EWOULDBLOCK || errno == EAGAIN) {
				printf("[warning] recv(), [errno %s]\n", errno == EAGAIN ? "EAGAIN" : "EWOULDBLOCK");
				sleep(5);
			}else{
				printf("[error] recv(), [errno %d]\n", errno);
				break;
			}*/
		
			empfangspuffer_leeren(befehlsbyte);
		
			switch(befehlsbyte){
				case 0:		// Blender Datenübertragung starten (10 Sekunden Dauer)
							blender_uebertragung_starten();
							break;
				case 1:		// Analyse Datenübertragung starten (10 Sekunden Dauer)						
							analyse_uebertragung_starten();
							break;
				case 2:		// Display Datenübertragung starten (10 Sekunden Dauer)
							display_uebertragung_starten();
							break;
				case 3:		// Joystick Daten
							joystick_daten_auswerten();
							break;
				default:
							break;
			}
		}
	
		/*if (n < 0) {
			printf ("%s: Kann keine Daten empfangen ...\n",
			argv[0] );
			continue;
		}
	
		time(&time1);
		strncpy(loctime, ctime(&time1), BUF);
		ptr = strchr(loctime, '\n');
		*ptr = '\0';
	
		printf ("%s: Daten erhalten von %s:UDP%u : %s \n", loctime, inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port), puffer);*/
		
		empfangspufferzeiger = &empfangspuffer[0];

		//http://openbook.rheinwerk-verlag.de/linux_unix_programmierung/Kap11-016.htm
		//https://www.ibm.com/support/pages/why-does-send-return-eagain-ewouldblock
		//https://beej.us/guide/bgnet/html/#blocking
	#endif
}

void Netzwerk::daten_senden(Treppensteiger &ts){
	if(blender_uebertragung){
		blender_uebertragungsdauer += ts.systeminfo.schleifenzeit;
		if(blender_uebertragungsdauer >= uebertragungs_timeout){
			blender_uebertragung = false;
		}
		else{
			blenderdaten_senden(ts);
		}
	}
	if(analyse_uebertragung){
		analyse_uebertragungsdauer += ts.systeminfo.schleifenzeit;
		if(analyse_uebertragungsdauer >= uebertragungs_timeout){
			analyse_uebertragung = false;
		}
		else{
			analysedaten_senden(ts);
		}
	}
	if(display_uebertragung){
		display_uebertragungsdauer += ts.systeminfo.schleifenzeit;
		if(display_uebertragungsdauer >= uebertragungs_timeout){
			display_uebertragung = false;
		}
		else{
			displaydaten_senden(ts);
		}
	}	
}

void Netzwerk::sendepuffer_fuellen(unsigned char variable){
	if((puffergroesse - (sendepufferzeiger - &sendepuffer[0])) >= sizeof(unsigned char)){
		*sendepufferzeiger = variable;
		sendepufferzeiger += sizeof(unsigned char);
	}
}

void Netzwerk::sendepuffer_fuellen(int variable){
	if((puffergroesse - (sendepufferzeiger - &sendepuffer[0])) >= sizeof(int)){
		*((int*)sendepufferzeiger) = variable;
		sendepufferzeiger += sizeof(int);
	}
}

void Netzwerk::sendepuffer_fuellen(unsigned long long variable){
	if((puffergroesse - (sendepufferzeiger - &sendepuffer[0])) >= sizeof(unsigned long long)){
		*((unsigned long long*)sendepufferzeiger) = variable;
		sendepufferzeiger += sizeof(unsigned long long);
	}
}

void Netzwerk::sendepuffer_fuellen(float variable){
	if((puffergroesse - (sendepufferzeiger - &sendepuffer[0])) >= sizeof(float)){
		*((float*)sendepufferzeiger) = variable;
		sendepufferzeiger += sizeof(float);
	}
}

void Netzwerk::sendepuffer_fuellen(unsigned char nachricht[], unsigned char laenge){
	if((puffergroesse - (sendepufferzeiger - &sendepuffer[0])) >= laenge){
		memcpy(sendepufferzeiger, nachricht, laenge);
		sendepufferzeiger += laenge;
	}
}

void Netzwerk::empfangspuffer_leeren(unsigned char &variable){
	if((puffergroesse - (empfangspufferzeiger - &empfangspuffer[0])) >= sizeof(unsigned char)){
		variable = *empfangspufferzeiger;
		empfangspufferzeiger += sizeof(unsigned char);
	}
}

void Netzwerk::empfangspuffer_leeren(int &variable){
	if((puffergroesse - (empfangspufferzeiger - &empfangspuffer[0])) >= sizeof(int)){
		variable = *((int*)empfangspufferzeiger);
		empfangspufferzeiger += sizeof(int);
	}
}

void Netzwerk::empfangspuffer_leeren(unsigned long long &variable){
	if((puffergroesse - (empfangspufferzeiger - &empfangspuffer[0])) >= sizeof(unsigned long long)){
		variable = *((unsigned long long*)empfangspufferzeiger);
		empfangspufferzeiger += sizeof(unsigned long long);
	}
}

void Netzwerk::empfangspuffer_leeren(float &variable){
	if((puffergroesse - (empfangspufferzeiger - &empfangspuffer[0])) >= sizeof(float)){
		variable = *((float*)empfangspufferzeiger);
		empfangspufferzeiger += sizeof(float);
	}
}

void Netzwerk::empfangspuffer_leeren(unsigned char nachricht[], unsigned char laenge){
	if((puffergroesse - (empfangspufferzeiger - &empfangspuffer[0])) >= laenge){
		memcpy(nachricht, empfangspufferzeiger, laenge);
		empfangspufferzeiger += laenge;
	}
}

/*
void Netzwerk::sendepuffer_fuellen(const float &variable){
	if((puffergroesse - (sendepufferzeiger - &sendepuffer[0])) >= sizeof(float)){
		memcpy(sendepufferzeiger, (unsigned char*) &variable, sizeof(float));
		sendepufferzeiger += sizeof(float);
	}
}

void Netzwerk::sendepuffer_fuellen(const int &variable){
	if((puffergroesse - (sendepufferzeiger - &sendepuffer[0])) >= sizeof(int)){
		memcpy(sendepufferzeiger, (unsigned char*) &variable, sizeof(int));
		sendepufferzeiger += sizeof(float);
	}
}

void Netzwerk::empfangspuffer_leeren(float &variable){
	if((puffergroesse - (empfangspufferzeiger - &empfangspuffer[0])) >= sizeof(float)){
		memcpy((unsigned char*) &variable, empfangspufferzeiger, sizeof(float));
		empfangspufferzeiger += sizeof(float);
	}
}

void Netzwerk::empfangspuffer_leeren(int &variable){
	if((puffergroesse - (empfangspufferzeiger - &empfangspuffer[0])) >= sizeof(int)){
		memcpy((unsigned char*) &variable, empfangspufferzeiger, sizeof(int));
		empfangspufferzeiger += sizeof(int);
	}
}
*/