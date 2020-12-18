#ifndef netzwerk_H
#define netzwerk_H

#ifdef __linux__
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <netdb.h>
	#include <stdio.h>
	#include <errno.h>
	#include <stdlib.h>
	#include <unistd.h>
#endif
#include <fcntl.h>
#include <cstring> // memcpy

#define _USE_MATH_DEFINES
#include <cmath>

class Treppensteiger;

class Netzwerk{
	public:		
		float test;
		
		static const unsigned int server_port = 1234;
		
		~Netzwerk(){
		#ifdef __linux__
			if(sockfd != -1){
				close(sockfd);
			}
		#endif
		}
		
		bool blender_uebertragung = false;
		bool analyse_uebertragung = false;
		bool display_uebertragung = false;
		
		int init();
		
		void daten_empfangen();
		void daten_senden(Treppensteiger &ts);
		
		void blender_uebertragung_starten();
		void analyse_uebertragung_starten();
		void display_uebertragung_starten();
		
		void blenderdaten_senden(Treppensteiger &ts);	
		void analysedaten_senden(Treppensteiger &ts);		
		void displaydaten_senden(Treppensteiger &ts);
		
		void joystick_daten_auswerten();
		
	private:
		int sockfd = -1;
		#ifdef __linux__
			struct sockaddr_in server_adresse;
			struct sockaddr_in client_adresse;
			struct sockaddr_in blender_adresse;
			struct sockaddr_in analyse_adresse;
			struct sockaddr_in display_adresse;
		#endif
		
		double blender_uebertragungsdauer;
		double analyse_uebertragungsdauer;
		double display_uebertragungsdauer;
		
		static const double uebertragungs_timeout;
		
		struct hostent *h;
		int addrlen;
		
		//void daten_senden(const char adresse[], unsigned int port, void* daten, unsigned int datenlaenge);
		
		static const int puffergroesse = 255;
		unsigned char sendepuffer[puffergroesse];
		unsigned char empfangspuffer[puffergroesse];
		unsigned char* sendepufferzeiger = NULL;
		unsigned char* empfangspufferzeiger = NULL;
		unsigned char befehlsbyte;
		int anzahl_empfangener_bytes;
		
		unsigned char analysedatenauswahl;
		
		void sendepuffer_fuellen(unsigned char variable);
		void sendepuffer_fuellen(int variable);
		void sendepuffer_fuellen(unsigned long long variable);		
		void sendepuffer_fuellen(float variable);
		void sendepuffer_fuellen(unsigned char nachricht[], unsigned char laenge);
		void empfangspuffer_leeren(unsigned char &variable);
		void empfangspuffer_leeren(int &variable);
		void empfangspuffer_leeren(unsigned long long &variable);		
		void empfangspuffer_leeren(float &variable);
		void empfangspuffer_leeren(unsigned char nachricht[], unsigned char laenge);
};

#endif