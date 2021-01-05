#include <cstdio>
#define _USE_MATH_DEFINES
#include <cmath> //?????????????????????

#include "treppensteiger.hpp"
#include "telemetrie.hpp"

void Telemetrie::starten(){
	aktiviert = true;
	printf(HIDE_CURSOR);
	#ifdef __linux__
		printf(CLEAR_SCREEN);
	#elif _WIN32
		console = GetConsoleWindow();
		GetWindowRect(console, &r1); //stores the console's current dimensions
		r2 = r1;
		MoveWindow(console, r1.left, r1.top, 1200, 1000, TRUE); // 800 width, 100 height
		printf(CLEAR_SCREEN);
	#endif
	fflush(stdout);
}

void Telemetrie::stoppen(){
	aktiviert = false;
	printf(UNHIDE_CURSOR);
	printf("\n");
	fflush(stdout);
}

void Telemetrie::aktualisieren(Treppensteiger &ts){
	vergangene_zeit_seit_letzter_ausgabe += ts.systeminfo.schleifenzeit;
	if(aktiviert && vergangene_zeit_seit_letzter_ausgabe >= 0.5){		
		#ifdef __linux__
			if(menue_modus != ts.controller.menueauswahl_kreuz_horizontal){
				menue_modus = ts.controller.menueauswahl_kreuz_horizontal;
				printf(CLEAR_SCREEN);
			}
		#elif _WIN32
			GetWindowRect(console, &r1); //stores the console's current dimensions
			if(r2.left != r1.left || r2.top != r1.top || r2.right != r1.right || r2.bottom != r1.bottom){
				r2 = r1;
				printf(CLEAR_SCREEN);
			}
		#endif
		
		printf(CURSOR_TO_BEGINNING);
		printf(T_BLACK_B_WHITE " Treppensteiger v1.0 " T_WHITE_B_BLACK "\n");
		printf("Status: ");		
		if(ts.winkelsensor_fehler){			
			printf( T_RED_B_BLACK "Winkelsensorfehler" T_WHITE_B_BLACK "\n");
		}
		else{
			printf( T_GREEN_B_BLACK "Bereit            " T_WHITE_B_BLACK "\n");
		}
		
		switch(ts.modus){
			case 0:		
				printf("Modus: " T_RED_B_BLACK		"Ausschalten" T_WHITE_B_BLACK "\n");	// Red
				break;
			case 1:
				printf("Modus: " T_RED_B_BLACK		"Stopp" T_WHITE_B_BLACK "      \n");	// Red
				break;
			case 2:		
				printf("Modus: " T_Cyan_B_BLACK		"Horizontal" T_WHITE_B_BLACK " \n");	// Cyan
				break;
			case 3:		
				printf("Modus: " T_MAGENTA_B_BLACK	"Vertikal" T_WHITE_B_BLACK "   \n");	// Magenta
				break;
			case 4:		
				printf("Modus: " T_YELLOW_B_BLACK	"Simulation" T_WHITE_B_BLACK " \n");	// Yellow
				break;
			case 5:		
				printf("Modus: " T_YELLOW_B_BLACK	"Automatisch" T_WHITE_B_BLACK "\n");	// Yellow
				break;
			default:
				printf("Modus: " T_RED_B_BLACK		"Fehler" T_WHITE_B_BLACK "     \n");	// Red
		}
		
		printf("\n");
		systeminfo_ausgeben(ts.systeminfo);
		printf("\n");		
		
		switch(menue_modus){
			case 0:
				dreistern_laengs_daten_ausgeben(ts.arm_vorne, "Dreistern Vorne:");
				dreistern_laengs_daten_ausgeben(ts.arm_hinten, "Dreistern Hinten:");		
				dreistern_seitlich_daten_ausgeben(ts.dreistern_links, "Dreistern Links:");
				dreistern_seitlich_daten_ausgeben(ts.dreistern_rechts, "Dreistern Rechts:");		
				printf("\n");
				
				stellglied_daten_ausgeben(ts.arm_vorne, "Stellglied Vorne:");
				stellglied_daten_ausgeben(ts.arm_hinten, "Stellglied Hinten:");
				printf("\n");
				
				stuhl_daten_ausgeben(ts.stuhl);
				fahrgestell_daten_ausgeben(ts);
				printf("\n");
				
				imu_daten_ausgeben(ts.stuhl.imu, "IMU Stuhl:");
				imu_daten_ausgeben(ts.imu_fahrgestell, "IMU Fahrgestell:");
				printf("\n");				
				
				sequenzregler_daten_ausgeben(ts.sequenzregler);
				printf("\n");
				break;
			case 1:
				controller_daten_ausgeben(ts.controller);
				printf("\n");
				kraftsensor_daten_ausgeben(ts.arm_vorne.stellglied.kraftsensor, "Kraftsensor Vorne:");
				kraftsensor_daten_ausgeben(ts.arm_hinten.stellglied.kraftsensor, "Kraftsensor Hinten:");
				printf("\n");
				netzwerk_daten_ausgeben(ts.netzwerk);		
				printf("\n");
				datenlogger_zustand_ausgeben(ts.datenlogger);
				printf("\n");
				break;
			case 2:
				motorregler_parameter_ausgeben(ts);
				printf("\n");
				uebertragungsfehler_ausgeben(ts);
				printf("\n");
		}
		
		fflush(stdout);
		vergangene_zeit_seit_letzter_ausgabe = 0.0;
	}
}

void Telemetrie::systeminfo_ausgeben(Systeminfo &systeminfo){
	printf(T_BLACK_B_WHITE "System:" T_WHITE_B_BLACK "\n");
	printf("(Laufzeit)        Gesamt: " T_YELLOW_B_BLACK "%6llu.%03llu s" T_WHITE_B_BLACK "\n", systeminfo.vergangene_us_seit_start/1000000, (systeminfo.vergangene_us_seit_start/1000)%1000);
	printf("(Schleifenzeit)   Mittel: " T_YELLOW_B_BLACK "%10.3f ms" T_WHITE_B_BLACK "   |  Max: "  T_YELLOW_B_BLACK "%10.3f ms" T_WHITE_B_BLACK "   |  Min: " T_YELLOW_B_BLACK "%10.3f ms" T_WHITE_B_BLACK "\n",
		(float)(systeminfo.mittlere_schleifenzeit*1000.0), (float)(systeminfo.maximale_schleifenzeit*1000.0), (float)(systeminfo.minimale_schleifenzeit*1000.0));
	
	const size_t kernanzahl = systeminfo.prozessorkernauslastung.size();
	
	if(kernanzahl == 0){
		printf("(Prozessor)       CPU:  " T_YELLOW_B_BLACK "%5.1f%%" T_WHITE_B_BLACK "\n", systeminfo.prozessorgesamtauslastung);
		printf("(Kerne)\n");
	}
	else{
		printf("(Prozessor)       CPU:  " T_YELLOW_B_BLACK "%5.1f%%" T_WHITE_B_BLACK "\n", systeminfo.prozessorgesamtauslastung);
		for(size_t i=0; i<kernanzahl; ++i){
			if(i == 0){
				printf("(Kerne)           CPU%u: " T_YELLOW_B_BLACK "%5.1f%%" T_WHITE_B_BLACK, i, systeminfo.prozessorkernauslastung[i]);
			}
			else{
				printf("  |  CPU%u: " T_YELLOW_B_BLACK "%5.1f%%" T_WHITE_B_BLACK, i, systeminfo.prozessorkernauslastung[i]);
			}
		}
		printf("\n");
	}	
}

void Telemetrie::imu_daten_ausgeben(IMU &imu, const char* ueberschrift){
	printf(T_BLACK_B_WHITE "%s" T_WHITE_B_BLACK "\n", ueberschrift);
	printf("(Magnetfeld)      x: " T_Grey_B_BLACK "%8.1f " MICRO "T" T_WHITE_B_BLACK "   |  y: " T_Grey_B_BLACK "%8.1f " MICRO "T" T_WHITE_B_BLACK "   |  z: " T_Grey_B_BLACK "%8.1f " MICRO "T" T_WHITE_B_BLACK 
		"   |  Norm: " T_Grey_B_BLACK "%8.1f " MICRO "T" T_WHITE_B_BLACK "        \n"
		, imu.mag.x, imu.mag.y, imu.mag.z, imu.mag.betrag());
		
	printf("(Magnetfeld)      x: " T_GREEN_B_BLACK "%8.1f " MICRO "T" T_WHITE_B_BLACK "   |  y: " T_GREEN_B_BLACK "%8.1f " MICRO "T" T_WHITE_B_BLACK "   |  z: " T_GREEN_B_BLACK "%8.1f " MICRO "T" T_WHITE_B_BLACK 
		"   |  Norm: " T_YELLOW_B_BLACK "%8.1f " MICRO "T" T_WHITE_B_BLACK "        \n"
		, imu.sys_mag.x, imu.sys_mag.y, imu.sys_mag.z, imu.sys_mag.betrag());
		
	printf("(Beschleunigung)  x: " T_Grey_B_BLACK "%8.1f mg" T_WHITE_B_BLACK "   |  y: " T_Grey_B_BLACK "%8.1f mg" T_WHITE_B_BLACK "   |  z: " T_Grey_B_BLACK "%8.1f mg" T_WHITE_B_BLACK 
		"   |  Norm: " T_Grey_B_BLACK "%8.1f mg" T_WHITE_B_BLACK "        \n"
		, imu.acc.x, imu.acc.y, imu.acc.z, imu.acc.betrag());
		
	printf("(Beschleunigung)  x: " T_GREEN_B_BLACK "%8.1f mg" T_WHITE_B_BLACK "   |  y: " T_GREEN_B_BLACK "%8.1f mg" T_WHITE_B_BLACK "   |  z: " T_GREEN_B_BLACK "%8.1f mg" T_WHITE_B_BLACK 
		"   |  Norm: " T_YELLOW_B_BLACK "%8.1f mg" T_WHITE_B_BLACK "        \n"
		, imu.sys_acc.x, imu.sys_acc.y, imu.sys_acc.z, imu.sys_acc.betrag());
		
	printf("(Gyroskop)        x: " T_Grey_B_BLACK "%8.1f " DEGREE "/s" T_WHITE_B_BLACK "  |  y: " T_Grey_B_BLACK "%8.1f " DEGREE "/s" T_WHITE_B_BLACK "  |  z: " T_Grey_B_BLACK "%8.1f " DEGREE "/s" T_WHITE_B_BLACK
		"        \n", imu.gyr.x, imu.gyr.y, imu.gyr.z);
		
	printf("(Gyroskop)        x: " T_GREEN_B_BLACK "%8.1f " DEGREE "/s" T_WHITE_B_BLACK "  |  y: " T_GREEN_B_BLACK "%8.1f " DEGREE "/s" T_WHITE_B_BLACK "  |  z: " T_GREEN_B_BLACK "%8.1f " DEGREE "/s" T_WHITE_B_BLACK
		"        \n", imu.sys_gyr.x, imu.sys_gyr.y, imu.sys_gyr.z);
		
	printf("(Temperatur)      T: " T_GREEN_B_BLACK "%8.1f " DEGREE "C" T_WHITE_B_BLACK "        \n", imu.temperatur);
	#ifdef __linux__
		printf("(Status)          s:     " T_GREEN_B_BLACK "0x%02x" T_WHITE_B_BLACK "      |  c:  " T_GREEN_B_BLACK "%s" T_WHITE_B_BLACK "          \n"
			, imu.status, imu.icm_20948_imu.statusString());
	#endif
}

//*******************************************************************************************************************************************************************************************

void Telemetrie::controller_daten_ausgeben(Controller &controller){
	printf(T_BLACK_B_WHITE "DS4 Controller:" T_WHITE_B_BLACK "\n");
	printf("(Linker-Stick)    x:   " T_GREEN_B_BLACK "%6d" T_WHITE_B_BLACK "  |  y:     " T_GREEN_B_BLACK "%6d" T_WHITE_B_BLACK "\n"
		, controller.achse_rohwert[ds4_achse_links_horizontal], controller.achse_rohwert[ds4_achse_links_vertikal]);
		
	printf("(Rechter-Stick)   x:   " T_GREEN_B_BLACK "%6d" T_WHITE_B_BLACK "  |  y:     " T_GREEN_B_BLACK "%6d" T_WHITE_B_BLACK "\n"
		, controller.achse_rohwert[ds4_achse_rechts_horizontal], controller.achse_rohwert[ds4_achse_rechts_vertikal]);
		
	printf("(Kreuz)           x:   " T_GREEN_B_BLACK "%6d" T_WHITE_B_BLACK "  |  y:     " T_GREEN_B_BLACK "%6d" T_WHITE_B_BLACK "\n"
		, controller.achse_rohwert[ds4_achse_kreuz_horizontal], controller.achse_rohwert[ds4_achse_kreuz_vertikal]);
		
	printf("(Hebel)           L:   " T_GREEN_B_BLACK "%6d" T_WHITE_B_BLACK "  |  R:     " T_GREEN_B_BLACK "%6d" T_WHITE_B_BLACK "\n"
		, controller.achse_rohwert[ds4_hebel_l2], controller.achse_rohwert[ds4_hebel_r2]);
		
	printf("(Tasten)          B:  " T_GREEN_B_BLACK "%d %d %d %d %d %d %d %d %d %d %d %d %d" T_WHITE_B_BLACK "\n", controller.knopf[ds4_kreuz], controller.knopf[ds4_kreis]
		, controller.knopf[ds4_dreieck], controller.knopf[ds4_quadrat], controller.knopf[ds4_knopf_l1], controller.knopf[ds4_knopf_r1], controller.knopf[ds4_knopf_l2]
		, controller.knopf[ds4_knopf_r2], controller.knopf[ds4_share], controller.knopf[ds4_options], controller.knopf[ds4_ps_knopf], controller.knopf[ds4_knopf_achse_links]
		, controller.knopf[ds4_knopf_achse_rechts]);
		
	printf("(Ladestand)       Bat: " T_GREEN_B_BLACK "%6d%%" T_WHITE_B_BLACK "\n", controller.akku_ladestand_abrufen());
}

//*******************************************************************************************************************************************************************************************

void Telemetrie::dreistern_laengs_daten_ausgeben(Arm &arm, const char* ueberschrift){
	printf(T_BLACK_B_WHITE "%s" T_WHITE_B_BLACK "\n", ueberschrift);
	printf("(Rotation)        Rohwert: " T_GREEN_B_BLACK "%6d" T_WHITE_B_BLACK "  |  Ist: " T_GREEN_B_BLACK "%8.2f" DEGREE T_WHITE_B_BLACK "   |  Soll: " T_YELLOW_B_BLACK "%8.2f" DEGREE T_WHITE_B_BLACK
		"   |  Antrieb: " T_YELLOW_B_BLACK "%6.1f%%" T_WHITE_B_BLACK "\n", arm.lenkung.dreistern.position_rohwert, rad_zu_grad(arm.lenkung.dreistern.drehwinkel)
		, rad_zu_grad(arm.lenkung.dreistern.fuehrungsgroesse), arm.lenkung.dreistern.motortreiber_steig.antriebsleistung*100.0);
			
		printf("(Lenkung)         Rohwert: " T_GREEN_B_BLACK "%6d" T_WHITE_B_BLACK "  |  Ist: " T_GREEN_B_BLACK "%8.2f" DEGREE T_WHITE_B_BLACK "   |  Soll: " T_YELLOW_B_BLACK "%8.2f" DEGREE T_WHITE_B_BLACK
		"   |  Antrieb: " T_YELLOW_B_BLACK "%6.1f%%" T_WHITE_B_BLACK "\n", arm.lenkung.winkelsensor.rohwert, rad_zu_grad(arm.lenkung.drehwinkel)
		, rad_zu_grad(arm.lenkung.fuehrungsgroesse), arm.lenkung.motortreiber.antriebsleistung*100.0);
}

//*******************************************************************************************************************************************************************************************

void Telemetrie::dreistern_seitlich_daten_ausgeben(Dreistern_Seitlich &dreistern, const char* ueberschrift){
	printf(T_BLACK_B_WHITE "%s" T_WHITE_B_BLACK "\n", ueberschrift);
	printf("(Rotation)        Rohwert: " T_GREEN_B_BLACK "%6d" T_WHITE_B_BLACK "  |  Ist: " T_GREEN_B_BLACK "%8.2f" DEGREE T_WHITE_B_BLACK "   |  Soll: " T_YELLOW_B_BLACK "%8.2f" DEGREE T_WHITE_B_BLACK
		"   |  Antrieb: " T_YELLOW_B_BLACK "%6.1f%%" T_WHITE_B_BLACK "\n", dreistern.winkelsensor_steig.rohwert, rad_zu_grad(dreistern.drehwinkel)
		, rad_zu_grad(dreistern.fuehrungsgroesse_steig), dreistern.motortreiber_steig.antriebsleistung*100.0);
		
	printf("(Translation)     Rohwert: " T_GREEN_B_BLACK "%6d" T_WHITE_B_BLACK "  |  Ist: " T_GREEN_B_BLACK "%8.2fmm" T_WHITE_B_BLACK "  |  Soll: " T_YELLOW_B_BLACK "%8.2fmm" T_WHITE_B_BLACK
		"  |  Antrieb: " T_YELLOW_B_BLACK "%6.1f%%" T_WHITE_B_BLACK "\n", dreistern.winkelsensor_antrieb.rohwert, dreistern.antrieb_strecke*1000.0
		, dreistern.fuehrungsgroesse_antrieb*1000.0, dreistern.motortreiber_antrieb.antriebsleistung*100.0);
}

//*******************************************************************************************************************************************************************************************

void Telemetrie::stellglied_daten_ausgeben(Arm &arm, const char* ueberschrift){
	printf(T_BLACK_B_WHITE "%s" T_WHITE_B_BLACK "\n", ueberschrift);
	
	printf("(Rotation)        Rohwert: " T_GREEN_B_BLACK "%6d" T_WHITE_B_BLACK "  |  Ist: " T_GREEN_B_BLACK "%8.2f" DEGREE T_WHITE_B_BLACK "   |  Soll: " T_YELLOW_B_BLACK "%8.2f" DEGREE T_WHITE_B_BLACK
		"   |  Antrieb: " T_YELLOW_B_BLACK "%6.1f%%" T_WHITE_B_BLACK "\n", arm.stellglied.position_rohwert, rad_zu_grad(arm.drehwinkel), rad_zu_grad(arm.stellglied.fuehrungsgroesse)
		, arm.stellglied.motortreiber.antriebsleistung*100.0);
	printf("                  Laenge:  " T_GREEN_B_BLACK "%8.2fmm" T_WHITE_B_BLACK "\n", arm.stellglied.laenge);
}

//*******************************************************************************************************************************************************************************************

void Telemetrie::stuhl_daten_ausgeben(Stuhl &stuhl){
	printf(T_BLACK_B_WHITE "Stuhl:" T_WHITE_B_BLACK "\n");
	printf("(Rotation)        Stellwinkel: " T_GREEN_B_BLACK "%6.1f" DEGREE T_WHITE_B_BLACK "  |  Antrieb Links: " T_YELLOW_B_BLACK "%6.1f%%" T_WHITE_B_BLACK "  |  Antrieb Rechts: " T_YELLOW_B_BLACK 
		"%6.1f%%" T_WHITE_B_BLACK "\n", rad_zu_grad(stuhl.drehwinkel), stuhl.motortreiber_links.antriebsleistung*100.0, stuhl.motortreiber_rechts.antriebsleistung*100.0);
}

//*******************************************************************************************************************************************************************************************

void Telemetrie::fahrgestell_daten_ausgeben(Treppensteiger &ts){
	printf(T_BLACK_B_WHITE "Fahrgestell:" T_WHITE_B_BLACK "\n");
	printf("(Rotation)        Stellwinkel: " T_GREEN_B_BLACK "%6.1f" DEGREE T_WHITE_B_BLACK "  |  Momentanpol: " T_GREEN_B_BLACK "%8.2fm" T_WHITE_B_BLACK "  |  Kompasswinkel: " T_GREEN_B_BLACK 
		"%7.1f" DEGREE T_WHITE_B_BLACK "\n", rad_zu_grad(ts.rot.y), ts.momentanpol, rad_zu_grad(ts.kompass_winkel));
}

//*******************************************************************************************************************************************************************************************

void Telemetrie::netzwerk_daten_ausgeben(Netzwerk &netzwerk){
	printf(T_BLACK_B_WHITE "Netzwerk:" T_WHITE_B_BLACK "\n");
	printf("(Blender)         Uebertragung:  ");
	if(netzwerk.blender_uebertragung){
		printf(T_GREEN_B_BLACK " an" T_WHITE_B_BLACK);
	}
	else{
		printf(T_RED_B_BLACK "aus" T_WHITE_B_BLACK);
	}
	printf("\n");
	printf("(Analyse)         Uebertragung:  ");
	if(netzwerk.analyse_uebertragung){
		printf(T_GREEN_B_BLACK " an" T_WHITE_B_BLACK);
	}
	else{
		printf(T_RED_B_BLACK "aus" T_WHITE_B_BLACK);
	}
	printf("\n");
	printf("(Display)         Uebertragung:  ");
	if(netzwerk.display_uebertragung){
		printf(T_GREEN_B_BLACK " an" T_WHITE_B_BLACK);
	}
	else{
		printf(T_RED_B_BLACK "aus" T_WHITE_B_BLACK);
	}
	printf("\n");
}

//*******************************************************************************************************************************************************************************************

void Telemetrie::sequenzregler_daten_ausgeben(Sequenzregler &sequenzregler){
	printf(T_BLACK_B_WHITE "Sequenzregler:" T_WHITE_B_BLACK "\n");
	printf("(Simulation)      Ausgewaehlter Datensatz: " T_GREEN_B_BLACK "%6d" T_WHITE_B_BLACK, sequenzregler.ausgewaehlter_simulationsdatensatz);
	printf("\n");	
	printf("(Regelung)        Ausgewaehlter Datensatz: " T_GREEN_B_BLACK "%6d" T_WHITE_B_BLACK, sequenzregler.ausgewaehlter_regeldatensatz);
	printf("\n");
}

//*******************************************************************************************************************************************************************************************

void Telemetrie::kraftsensor_daten_ausgeben(Kraftsensor &kraftsensor, const char* ueberschrift){
	printf(T_BLACK_B_WHITE "%s" T_WHITE_B_BLACK "\n", ueberschrift);
	printf("(Kraft)           Rohwert: " T_GREEN_B_BLACK "%8d" T_WHITE_B_BLACK "  |  Mittelwert: " T_GREEN_B_BLACK "%8d" T_WHITE_B_BLACK "  |  Abs. Kraft: " T_GREEN_B_BLACK "%8.1fN" T_WHITE_B_BLACK"\n"
		, kraftsensor.kraft_rohwert, kraftsensor.mittelwert_kraft_rohwert, kraftsensor.kraft);
		
	printf("                  -1000      -750      -250        0        250      -750      1000\n");
		
	int skalierter_wert = (int)std::round(kraftsensor.kraft * 0.03);
		
	for(int i=-30; i<=30; i++){
		if(i == -30) printf("                     " T_WHITE_B_WHITE " ");		
		if(i == 0) printf(T_WHITE_B_WHITE " ");
		if(i == 30) printf(T_WHITE_B_WHITE " " T_WHITE_B_BLACK "\n");
		
		if(i != -30 && i != 0 && i != 30){
			if(i < 0){
				if(skalierter_wert < i){
					if(i <= -20){
						printf(T_RED_B_RED " ");
					}
					else{
						if(i <= -10){
							printf(T_YELLOW_B_YELLOW " ");
						}
						else{
							printf(T_GREEN_B_GREEN " ");
						}
					}
				}
				else{
					printf(T_BLACK_B_BLACK " ");
				}
			}
			
			if(i > 0){
				if(skalierter_wert > i){
					if(i >= 20){
						printf(T_RED_B_RED " ");
					}
					else{
						if(i >= 10){
							printf(T_YELLOW_B_YELLOW " ");
						}
						else{
							printf(T_GREEN_B_GREEN " ");
						}
					}
				}
				else{
					printf(T_BLACK_B_BLACK " ");
				}
			}
			
		}
	}
}

void Telemetrie::kraftsensor_daten_ausgeben_leichte_Last(Kraftsensor &kraftsensor, const char* ueberschrift){
	printf(T_BLACK_B_WHITE "%s" T_WHITE_B_BLACK "\n", ueberschrift);
	printf("(Kraft)           Rohwert: " T_GREEN_B_BLACK "%8d" T_WHITE_B_BLACK "  |  Mittelwert: " T_GREEN_B_BLACK "%8d" T_WHITE_B_BLACK "  |  Abs. Kraft: " T_GREEN_B_BLACK "%8.1fN" T_WHITE_B_BLACK "\n"
		, kraftsensor.kraft_rohwert, kraftsensor.mittelwert_kraft_rohwert, kraftsensor.kraft);
		
	printf("                   -100      -75       -25         0        25       -75        100\n");
		
	int skalierter_wert = (int)std::round(kraftsensor.kraft * 0.3);
		
	for(int i=-30; i<=30; i++){
		if(i == -30) printf("                     " T_WHITE_B_WHITE " ");		
		if(i == 0) printf(T_WHITE_B_WHITE " ");
		if(i == 30) printf(T_WHITE_B_WHITE " " T_WHITE_B_BLACK "\n");
		
		if(i != -30 && i != 0 && i != 30){
			if(i < 0){
				if(skalierter_wert < i){
					if(i <= -20){
						printf(T_RED_B_RED " ");
					}
					else{
						if(i <= -10){
							printf(T_YELLOW_B_YELLOW " ");
						}
						else{
							printf(T_GREEN_B_GREEN " ");
						}
					}
				}
				else{
					printf(T_BLACK_B_BLACK " ");
				}
			}
			
			if(i > 0){
				if(skalierter_wert > i){
					if(i >= 20){
						printf(T_RED_B_RED " ");
					}
					else{
						if(i >= 10){
							printf(T_YELLOW_B_YELLOW " ");
						}
						else{
							printf(T_GREEN_B_GREEN " ");
						}
					}
				}
				else{
					printf(T_BLACK_B_BLACK " ");
				}
			}			
		}
	}
}

//*******************************************************************************************************************************************************************************************

float Telemetrie::rad_zu_grad(float rad){
	return ((rad / M_PI) * 180.0);
}

//*******************************************************************************************************************************************************************************************

void Telemetrie::motorregler_parameter_ausgeben(Treppensteiger &ts){
	printf(T_BLACK_B_WHITE "Motorreglerparameter:" T_WHITE_B_BLACK "\n");
	printf("(Stellglied)           P: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "  |  I: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "  |  D: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "\n", 
		ts.arm_vorne.stellglied.Kp, ts.arm_vorne.stellglied.Ki, ts.arm_vorne.stellglied.Kd);
	printf("(Lenkung)              P: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "  |  I: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "  |  D: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "\n", 
		ts.arm_vorne.lenkung.Kp, ts.arm_vorne.lenkung.Ki, ts.arm_vorne.lenkung.Kd);
	printf("(Antrieb)              P: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "  |  I: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "  |  D: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "\n", 
		ts.dreistern_links.Kp_antrieb, ts.dreistern_links.Ki_antrieb, ts.dreistern_links.Kd_antrieb);
	printf("(Steig)                P: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "  |  I: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "  |  D: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "\n", 
		ts.dreistern_links.Kp_steig, ts.dreistern_links.Ki_steig, ts.dreistern_links.Kd_steig);
	printf("(BLDC)                 P: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "  |  I: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "  |  D: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "\n", 
		ts.arm_vorne.lenkung.dreistern.Kp, ts.arm_vorne.lenkung.dreistern.Ki, ts.arm_vorne.lenkung.dreistern.Kd);
	printf("(Stuhl)                P: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "  |  I: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "  |  D: " T_YELLOW_B_BLACK "%13.10f" T_WHITE_B_BLACK "\n", 
		ts.stuhl.Kp, ts.stuhl.Ki, ts.stuhl.Kd);
}

//*******************************************************************************************************************************************************************************************

void Telemetrie::datenlogger_zustand_ausgeben(Datenlogger &datenlogger){
	printf(T_BLACK_B_WHITE "Datenlogger:" T_WHITE_B_BLACK "\n");
	printf("(Fehlerprotokoll)      Zustand:  ");
	if(true){
		printf(T_GREEN_B_BLACK " an" T_WHITE_B_BLACK);
	}
	else{
		printf(T_RED_B_BLACK "aus" T_WHITE_B_BLACK);
	}	
	printf("  |  Eintraege: " T_GREEN_B_BLACK "%8d" T_WHITE_B_BLACK "\n", datenlogger.anzahl_fehlerprotokoll_eintraege);
	
	printf("(Systeminfo)           Zustand:  ");
	if(datenlogger.aktiv){
		printf(T_GREEN_B_BLACK " an" T_WHITE_B_BLACK);
	}
	else{
		printf(T_RED_B_BLACK "aus" T_WHITE_B_BLACK);
	}	
	printf("  |  Eintraege: " T_GREEN_B_BLACK "%8d" T_WHITE_B_BLACK "\n", datenlogger.anzahl_systeminfo_eintraege);
	
	printf("(Winkelsensoren)       Zustand:  ");
	if(datenlogger.aktiv){
		printf(T_GREEN_B_BLACK " an" T_WHITE_B_BLACK);
	}
	else{
		printf(T_RED_B_BLACK "aus" T_WHITE_B_BLACK);
	}	
	printf("  |  Eintraege: " T_GREEN_B_BLACK "%8d" T_WHITE_B_BLACK "\n", datenlogger.anzahl_winkelsensor_eintraege);
	
	printf("(IMU)                  Zustand:  ");
	if(datenlogger.aktiv){
		printf(T_GREEN_B_BLACK " an" T_WHITE_B_BLACK);
	}
	else{
		printf(T_RED_B_BLACK "aus" T_WHITE_B_BLACK);
	}	
	printf("  |  Eintraege: " T_GREEN_B_BLACK "%8d" T_WHITE_B_BLACK "\n", datenlogger.anzahl_imu_eintraege);
}

//*******************************************************************************************************************************************************************************************

void Telemetrie::uebertragungsfehler_ausgeben(Treppensteiger &ts){
	printf(T_BLACK_B_WHITE "Uebertragungsfehler:" T_WHITE_B_BLACK "\n");
	printf("(WS Dreistern Links Antrieb)    Fehler: " T_YELLOW_B_BLACK "%8d" T_WHITE_B_BLACK "\n", ts.dreistern_links.winkelsensor_steig.uebertragungsfehler);
	printf("(WS Dreistern Links Rotation)   Fehler: " T_YELLOW_B_BLACK "%8d" T_WHITE_B_BLACK "\n", ts.dreistern_links.winkelsensor_antrieb.uebertragungsfehler);
	printf("(WS Dreistern Rechts Antrieb)   Fehler: " T_YELLOW_B_BLACK "%8d" T_WHITE_B_BLACK "\n", ts.dreistern_rechts.winkelsensor_steig.uebertragungsfehler);
	printf("(WS Dreistern Rechts Antrieb)   Fehler: " T_YELLOW_B_BLACK "%8d" T_WHITE_B_BLACK "\n", ts.dreistern_rechts.winkelsensor_antrieb.uebertragungsfehler);
	printf("(WS Dreistern Vorne Lenkung)    Fehler: " T_YELLOW_B_BLACK "%8d" T_WHITE_B_BLACK "\n", ts.arm_vorne.lenkung.winkelsensor.uebertragungsfehler);
	printf("(WS Dreistern Hinten Lenkung)   Fehler: " T_YELLOW_B_BLACK "%8d" T_WHITE_B_BLACK "\n", ts.arm_hinten.lenkung.winkelsensor.uebertragungsfehler);
}