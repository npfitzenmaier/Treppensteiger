#include "controller.hpp"

#ifdef __linux__
#include <unistd.h>
#endif


#include <iostream>
#include <fstream>
#include <string>

using namespace std;

Controller::~Controller(){
	#ifdef __linux__
		close( joy_fd );
		close( event_fd );
	#endif
	delete[] achse_rohwert;
	delete[] achse_rohwert_vorher;
	delete[] achse;
	delete[] knopf;
	delete[] knopf_vorher;
}

int Controller::init(){
	#ifdef __linux__
		int timeout = 0;
		while((joy_fd = open(JOY_DEV , O_RDONLY)) == -1){
			timeout++;	
			sleep(1);
		
			if(timeout >= 20){	// 20 Sekunden Timeout
				printf( "Couldn't open controller\n" );
				return -1;
			}
		}
	
		ioctl(joy_fd, JSIOCGAXES, &achsenanzahl);
		ioctl(joy_fd, JSIOCGBUTTONS, &knopfanzahl);
		ioctl(joy_fd, JSIOCGNAME(80), &controller_name);	
	
		achse_rohwert			= new int[achsenanzahl];	// [-32767, 32767]
		achse_rohwert_vorher	= new int[achsenanzahl];	// [-32767, 32767]
		achse					= new float[achsenanzahl];	// Achsen [-1.0, 1.0], wobei positiv oben/rechts; Kipphebel [0.0, 1.0], wobei voll durchdruecken = 1.0
		knopf					= new char[knopfanzahl];	// {1, 0}
		knopf_vorher			= new char[knopfanzahl];	// {1, 0}	

		//achse			= (int *) calloc( achsenanzahl, sizeof( int ) );
		//achse_vorher	= (int *) calloc( achsenanzahl, sizeof( int ) );
		//knopf			= (char *) calloc( knopfanzahl, sizeof( char ) );
		//knopf_vorher	= (char *) calloc( knopfanzahl, sizeof( char ) );
	
		for(int i=0; i<achsenanzahl; i++){
			achse_rohwert[i] = 0;
			achse_rohwert_vorher[i] = 0;
			achse[i] = 0.0;
		}
		for(int i=0; i<knopfanzahl; i++){
			knopf[i] = 0;
			knopf_vorher[i] = 0;
		}

		fcntl( joy_fd, F_SETFL, O_NONBLOCK );	/* use non-blocking mode */
	
		share_schalter = true;
		ps_knopf_schalter = false;
	
		// Vibration Initialisierung
		timeout = 0;
		while((event_fd = open(EVENT_PFAD, O_RDWR)) == -1){
			timeout++;
			sleep(1);
		
			if(timeout >= 4){	// 4 Sekunden Timeout
				printf("Couldn't open controller vibration event filedescriptor\n");
				return -1;
			}
		}	
	
		effect.type = FF_RUMBLE;
		effect.id = -1;
		effect.u.rumble.strong_magnitude = 0xFFFF;
		effect.u.rumble.weak_magnitude = 0xFFFF;
		effect.replay.length = 250;
		effect.replay.delay = 0;
	
		ioctl(event_fd, EVIOCSFF, &effect);
	
		setze_rgb_led_farbe(187,0,0);
	#endif
	return 0;
}

void Controller::eingaben_abrufen(){
	#ifdef __linux__
		while(read(joy_fd, &js, sizeof(struct js_event)) == sizeof(struct js_event)){	//read the joystick state	
			//see what to do with the event
			switch (js.type & ~JS_EVENT_INIT){
				case JS_EVENT_AXIS:
					achse_rohwert[js.number] = js.value;
					break;
				case JS_EVENT_BUTTON:
					knopf[js.number] = js.value;
					break;
			}
		}
		/*for(int x=0 ; x<knopfanzahl ; ++x )
					printf("B%d: %d ", x, knopf[x] );
		for(int x=0 ; x<achsenanzahl ; ++x )
			printf("Achse%d: %6d ", x, achse_rohwert[x] );
		printf("\n");*/
	#endif
}

void Controller::moduswechsel(){
	#ifdef __linux__
		if(knopf[ds4_kreuz] == 1 && knopf_vorher[ds4_kreuz] == 0){			
			if(modus == 1){
				modus = 0;
			}
			else{
				modus = 1;
			}
		}
		if(knopf[ds4_quadrat] == 1 && knopf_vorher[ds4_quadrat] == 0){
			modus = 2;
		}
		if(knopf[ds4_dreieck] == 1 && knopf_vorher[ds4_dreieck] == 0){
			modus = 3;
		}
		if(knopf[ds4_kreis] == 1 && knopf_vorher[ds4_kreis] == 0){		
			if(modus == 4)	modus = 5;
			else 			modus = 4;
		}	
		if(knopf[ds4_share] == 1 && knopf_vorher[ds4_share] == 0){
			share_schalter = !share_schalter;
		}
		if(knopf[ds4_ps_knopf] == 1 && knopf_vorher[ds4_ps_knopf] == 0){
			ps_knopf_schalter = !ps_knopf_schalter;
		}
		if(knopf[ds4_options] == 1){
			eingabesperre();
		}
		if(achse_rohwert[ds4_achse_kreuz_horizontal] == 32767 && achse_rohwert_vorher[ds4_achse_kreuz_horizontal] == 0 && (menueauswahl_kreuz_horizontal + 1) < menueanzahl_kreuz_horizontal){
			menueauswahl_kreuz_horizontal++;
		}
		if(achse_rohwert[ds4_achse_kreuz_horizontal] == -32767 && achse_rohwert_vorher[ds4_achse_kreuz_horizontal] == 0 && menueauswahl_kreuz_horizontal > 0){
			menueauswahl_kreuz_horizontal--;
		}
		if(achse_rohwert[ds4_achse_kreuz_vertikal] == -32767 && achse_rohwert_vorher[ds4_achse_kreuz_vertikal] == 0 && (menueauswahl_kreuz_vertikal + 1) < menueanzahl_kreuz_vertikal){
			menueauswahl_kreuz_vertikal++;
		}
		if(achse_rohwert[ds4_achse_kreuz_vertikal] == 32767 && achse_rohwert_vorher[ds4_achse_kreuz_vertikal] == 0 && menueauswahl_kreuz_vertikal > 0){
			menueauswahl_kreuz_vertikal--;
		}
	
		knopf_vorher[ds4_kreuz]			= knopf[ds4_kreuz];
		knopf_vorher[ds4_kreis]			= knopf[ds4_kreis];
		knopf_vorher[ds4_dreieck]		= knopf[ds4_dreieck];
		knopf_vorher[ds4_quadrat]		= knopf[ds4_quadrat];
		knopf_vorher[ds4_share]			= knopf[ds4_share];
		knopf_vorher[ds4_ps_knopf]		= knopf[ds4_ps_knopf];
	
		achse_rohwert_vorher[ds4_achse_kreuz_horizontal] = achse_rohwert[ds4_achse_kreuz_horizontal];
		achse_rohwert_vorher[ds4_achse_kreuz_vertikal] = achse_rohwert[ds4_achse_kreuz_vertikal];
	
		achse[ds4_achse_rechts_vertikal]	= (float) -achse_rohwert[ds4_achse_rechts_vertikal]/32767.0;
		achse[ds4_achse_rechts_horizontal]	= (float)  achse_rohwert[ds4_achse_rechts_horizontal]/32767.0;
		achse[ds4_achse_links_vertikal]		= (float) -achse_rohwert[ds4_achse_links_vertikal]/32767.0;
		achse[ds4_achse_links_horizontal]	= (float)  achse_rohwert[ds4_achse_links_horizontal]/32767.0;
		achse[ds4_hebel_r2] = (float) (achse_rohwert[ds4_hebel_r2]+32767)/65534.0;
		achse[ds4_hebel_l2] = (float) (achse_rohwert[ds4_hebel_l2]+32767)/65534.0;
	#endif
}

void Controller::led_farbe_aktualisieren(bool moduswechsel){
	if(moduswechsel){
		switch(modus){
			case 0:
				led_farbe_zuruecksetzen();
				break;
			case 1:
				setze_rgb_led_farbe(187,0,0);
				break;
			case 2:				
				setze_rgb_led_farbe(0,187,187);
				break;
			case 3:
				setze_rgb_led_farbe(187,0,187);
				break;
			case 4:
				setze_rgb_led_farbe(187,187,0);
				break;
			case 5:
				setze_rgb_led_farbe(187,187,0);
				break;
			default:
				led_farbe_zuruecksetzen();
		}
	}
	
	static int blink_zaehler = 0;
	static bool blink_led_an = false;
	if(modus == 5){
		if(blink_zaehler%10 == 0){
			if(blink_led_an)	setze_rgb_led_farbe(187,187,0);
			else				setze_rgb_led_farbe(0,0,0);
			
			blink_led_an = !blink_led_an;
		}
		blink_zaehler++;
	}
}

int Controller::akku_ladestand_abrufen(){
	int akku_ladestand;
	string line;
	ifstream myfile(BAT_LEVEL);
	if (myfile.is_open()){
		getline(myfile,line);
		akku_ladestand = stoi(line);
	}	
	else{
		akku_ladestand = -1;;
	}
	myfile.open (BAT_LEVEL);

	myfile.close();
	return akku_ladestand;
}

void Controller::setze_rgb_led_farbe(unsigned char rot, unsigned char gruen, unsigned char blau){
	led_r = rot;
	led_g = gruen;
	led_b = blau;
	setze_led_farbe("/sys/class/leds/0005:054C:09CC.0001:red/brightness", rot);
	setze_led_farbe("/sys/class/leds/0005:054C:09CC.0001:green/brightness", gruen);
	setze_led_farbe("/sys/class/leds/0005:054C:09CC.0001:blue/brightness", blau);
	
	rpi_rgb_led->setze_rgb_led_farbe(rot, gruen, blau);
}

void Controller::setze_led_farbe(const char* pfad, unsigned char intensitaet){
	fstream fs;
	fs.open(pfad, ios::out | ios::binary);
	for(int i =0; i<puffergroesse; i++){
		puffer[i] = 0x00;
	}
	if(intensitaet<10){
		puffer[0] = intensitaet + '0';
		puffer[1] = 0x0a; // line feed
	}
	else{			
		if(intensitaet<100){
			puffer[0] = intensitaet/10 + '0';
			puffer[1] = intensitaet%10 + '0';
			puffer[2] = 0x0a; // line feed
		}
		else{
			puffer[0] = intensitaet/100 + '0';
			puffer[1] = (intensitaet%100)/10 + '0';
			puffer[2] = intensitaet%10 + '0';
			puffer[3] = 0x0a; // line feed
		}
	}
	if(fs.is_open()){
		fs.write(puffer, puffergroesse);
		fs.close();
	}
	else{
		return;
	}	
}

void Controller::led_farbe_zuruecksetzen(){
	setze_rgb_led_farbe(0,0,64);
}

void Controller::eingabesperre(){	
	unsigned char r = led_r;
	unsigned char g = led_g;
	unsigned char b = led_b;
	
	bool freigabe = false;
	bool schloss1 = false;
	bool schloss2 = false;
	bool schloss3 = false;
	int ruecksetzzeit = 0;
	bool led_an_aus = false;
	int led_blink_teiler = 0;
	
	kurz_vibrieren();
	
	#ifdef __linux__
		while(!freigabe){
			eingaben_abrufen();
		
			usleep(10000);	// 10ms
		
			if(knopf[ds4_knopf_l1] == 1 && knopf[ds4_knopf_r1] == 0 && knopf[ds4_ps_knopf] == 0) schloss1 = true;
			if(knopf[ds4_knopf_l1] == 1 && knopf[ds4_knopf_r1] == 1 && knopf[ds4_ps_knopf] == 0 && schloss1) schloss2 = true;
			if(knopf[ds4_knopf_l1] == 1 && knopf[ds4_knopf_r1] == 1 && knopf[ds4_ps_knopf] == 1 && schloss2) schloss3 = true;
			if(schloss1 && schloss2 && schloss3) freigabe = true;
			if(schloss1 || schloss2 || schloss3) ruecksetzzeit++;
			if(ruecksetzzeit >= 200){	// 200 * 10ms = 2 Sekunden
				schloss1 = false;
				schloss2 = false;
				schloss3 = false;
				ruecksetzzeit = 0;
			}
		
			if(led_blink_teiler%25 == 0){
				led_an_aus = !led_an_aus;
				if(led_an_aus)	setze_rgb_led_farbe(0,187,0);
				else			setze_rgb_led_farbe(0,0,0);
			}
			led_blink_teiler++;		
		}
		setze_rgb_led_farbe(r, g, b);
		kurz_vibrieren();
	#endif
}

void Controller::vibrieren(bool an_aus){
	#ifdef __linux__
		if(an_aus){
			struct input_event play;
			play.type = EV_FF;
			play.code = effect.id;
			play.value = 1;
			write(event_fd, (const void*) &play, sizeof(play));
		}
		else{
			struct input_event stop;
			stop.type = EV_FF;
			stop.code =  effect.id;
			stop.value = 0;	
			write(event_fd, (const void*) &stop, sizeof(stop));
		}
	#endif
}

void Controller::kurz_vibrieren(){
	#ifdef __linux__
		vibrieren(true);
		usleep(500000);
		vibrieren(false);
	#endif
}