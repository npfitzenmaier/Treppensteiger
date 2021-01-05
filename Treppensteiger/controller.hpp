#ifndef controller_H
#define controller_H

#include <fcntl.h>
#ifdef __linux__
	#include <linux/joystick.h>	// MAC DS4:	DC:AF:68:66:E9:01
	#include <linux/input.h>
#endif
#include <stdio.h>
//#include <sys/ioctl.h>

#include "rgb_led.hpp"

#define JOY_DEV "/dev/input/js0"
// cd /dev/input/; dmesg | grep Wire
#define BAT_LEVEL	"/sys/class/power_supply/sony_controller_battery_dc:af:68:66:e9:01/capacity"
// sudo apt-get install joystick
// fftest /dev/input/eventX
//https://01.org/linuxgraphics/gfx-docs/drm/input/ff.html
//https://www.kernel.org/doc/Documentation/input/event-codes.txt
#define EVENT_PFAD	"/dev/input/event2"
#define ANZAHL_SIMULTANER_VIBRATIONS_EFFEKTE	16

#define ds4_kreuz		0
#define ds4_kreis		1
#define ds4_dreieck		2
#define ds4_quadrat		3
#define ds4_knopf_l1	4
#define ds4_knopf_r1	5
#define ds4_knopf_l2	6
#define ds4_knopf_r2	7
#define ds4_share		8
#define ds4_options		9
#define ds4_ps_knopf	10
#define ds4_knopf_achse_links 	11
#define ds4_knopf_achse_rechts 	12

#define ds4_achse_rechts_vertikal		4
#define ds4_achse_rechts_horizontal		3
#define ds4_achse_links_vertikal		1
#define ds4_achse_links_horizontal		0
#define ds4_hebel_r2					5
#define ds4_hebel_l2					2
#define ds4_achse_kreuz_vertikal		7
#define ds4_achse_kreuz_horizontal		6

class Controller{
	public:
		//Controller();
		~Controller();
		int init();
		void eingaben_abrufen();
		int akku_ladestand_abrufen();
		void led_farbe_aktualisieren(bool moduswechsel);
		void setze_rgb_led_farbe(unsigned char rot, unsigned char gruen, unsigned char blau);
		void setze_led_farbe(const char* pfad, unsigned char intensitaet);
		void led_farbe_zuruecksetzen();
		void moduswechsel();
		void eingabesperre();
		void vibrieren(bool an_aus);
		void kurz_vibrieren();
		
		int joy_fd;
		int* achse_rohwert = NULL;
		int* achse_rohwert_vorher = NULL;
		int achsenanzahl = 0;
		int knopfanzahl = 0;
		int event_fd;
		float* achse = NULL;
		char* knopf = NULL;
		char* knopf_vorher = NULL;
		char controller_name[80];
		
		int modus = 1; // 0...4
		int menueauswahl_kreuz_horizontal = 0; // 0...inf
		int menueauswahl_kreuz_vertikal = 0; // 0...inf
		static const int menueanzahl_kreuz_horizontal = 3;
		static const int menueanzahl_kreuz_vertikal = 5;
		
		static const int puffergroesse = 5;
		char puffer[puffergroesse];
		
		bool share_schalter;
		bool ps_knopf_schalter;
		
		unsigned char led_r;
		unsigned char led_g;
		unsigned char led_b;
		
		RGB_LED* rpi_rgb_led;
	private:
	#ifdef __linux__
		struct js_event js;
		struct ff_effect effect;

	#elif _WIN32
		int* deviceHandleArray;
		int deviceId;
	#endif
};
#endif