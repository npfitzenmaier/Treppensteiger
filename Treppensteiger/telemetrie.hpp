#ifndef telemetrie_H
#define telemetrie_H

#define T_WHITE_B_BLACK		"\033[0;37;40m"
#define T_BLACK_B_WHITE		"\033[0;30;47m"
#define T_BLACK_B_BLACK		"\033[0;30;40m"

#define T_GREEN_B_BLACK		"\033[0;32;40m"
#define T_YELLOW_B_BLACK	"\033[0;33;40m"
#define T_MAGENTA_B_BLACK	"\033[0;35;40m"
#define T_RED_B_BLACK		"\033[0;31;40m"
#define T_Cyan_B_BLACK		"\033[0;36;40m"
#define T_Grey_B_BLACK		"\033[0;90;40m"

#define T_WHITE_B_WHITE		"\033[0;37;47m"
#define T_GREEN_B_GREEN		"\033[0;32;42m"
#define T_YELLOW_B_YELLOW	"\033[0;33;43m"
#define T_RED_B_RED			"\033[0;31;41m"

#define T_BWHITE_B_BWHITE	"\033[0;97;107m"
#define T_BGREEN_B_BGREEN	"\033[0;92;102m"
#define T_BYELLOW_B_BYELLOW	"\033[0;93;103m"
#define T_BRED_B_BRED		"\033[0;91;101m"

#define HIDE_CURSOR				"\e[?25l"
#define UNHIDE_CURSOR			"\e[?25h"
#define CLEAR_SCREEN			"\033[2J"
#define CURSOR_TO_BEGINNING		"\033[1;1H"

#ifdef __linux__
	#define DEGREE		"°"
	#define MICROTESLA	"µ"
#elif _WIN32
	// Codepage 850 Westeuropaeisch (oktal)
	#define DEGREE		"\370"
	#define MICROTESLA	"\346"
#endif
//printf("\033[1;1H");		// positions the cursor at row 1, column 1

/*
print("\033[0;32;40m Bright Green  \n")

\033[	Escape code
0		Style
32		Text colour
40m		Background colour

Name				|	 FG Code	|	 BG Code	|	   RGB Code
--------------------|---------------------------------------------------
Black				|		30		|		40		|	  0,   0,   0
Red					|		31		|		41		|	187,   0,   0
Green				|		32		|		42		|	  0, 187,   0
Yellow				|		33		|		43		|	187, 187,   0
Blue				|		34		|		44		|	  0,   0, 187
Magenta				|		35		|		45		|	187,   0, 187
Cyan				|		36		|		46		|	  0, 187, 187
White				|		37		|		47		|	187, 187, 187
Bright Black		|		90		|		100		|	 85,  85,  85
Bright Red			|		91		|		101		|	255,  85,  85
Bright Green		|		92		|		102		|	 85, 255,  85
Bright Yellow		|		93		|		103		|	255, 255,  85
Bright Blue			|		94		|		104		|	 85,  85, 255
Bright Magenta		|		95		|		105		|	255,  85, 255
Bright Cyan			|		96		|		106		|	 85, 255, 255
Bright White		|		97		|		107		|	255, 255, 255

Text Style		|	   Code
----------------|----------------
No effect		|		0
Bold			|		1
Underline		|		2
Negative1		|		3
Negative2		|		5

*/
#ifdef _WIN32
	#include <windows.h>
#endif

class Treppensteiger;
class Arm;
class Dreistern_Seitlich;
class Controller;
class IMU;
class Stuhl;
class Netzwerk;
class Sequenzregler;
class Kraftsensor;
class Datenlogger;
class Systeminfo;

class Telemetrie{
	public:
		bool aktiviert = false;
		double vergangene_zeit_seit_letzter_ausgabe = 0.0;
		
		void starten();
		void stoppen();
		void aktualisieren(Treppensteiger &ts);
	private:
		void systeminfo_ausgeben(Systeminfo &systeminfo);
		void imu_daten_ausgeben(IMU &imu, const char* ueberschrift);
		void controller_daten_ausgeben(Controller &controller);
		void dreistern_laengs_daten_ausgeben(Arm &arm, const char* ueberschrift);
		void dreistern_seitlich_daten_ausgeben(Dreistern_Seitlich &dreistern, const char* ueberschrift);
		void stellglied_daten_ausgeben(Arm &arm, const char* ueberschrift);
		void stuhl_daten_ausgeben(Stuhl &stuhl);
		void fahrgestell_daten_ausgeben(Treppensteiger &ts);
		void netzwerk_daten_ausgeben(Netzwerk &netzwerk);
		void sequenzregler_daten_ausgeben(Sequenzregler &sequenzregler);
		void motorregler_parameter_ausgeben(Treppensteiger &ts);
		void kraftsensor_daten_ausgeben(Kraftsensor &kraftsensor, const char* ueberschrift);
		void kraftsensor_daten_ausgeben_leichte_Last(Kraftsensor &kraftsensor, const char* ueberschrift);		
		void datenlogger_zustand_ausgeben(Datenlogger &datenlogger);
		void uebertragungsfehler_ausgeben(Treppensteiger &ts);
		float rad_zu_grad(float rad);
		
		int menue_modus;

		#ifdef _WIN32
			HWND console;
			RECT r1, r2;
		#endif
};

#endif