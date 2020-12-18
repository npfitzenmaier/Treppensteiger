#include <cstdio>

#include "treppensteiger.hpp"
#include "vektor.hpp"

int main(){
	
	Vektor pos		( 0.0, 0.0, 0.135 );
	Vektor lage		( 0.0, 0.0, 1.0   );
	Vektor richtung	( 1.0, 0.0, 0.0   );	
	
	Treppensteiger treppensteiger(pos, lage, richtung);	
	
	if(treppensteiger.init() == -1){		
		printf("Fehler in treppensteiger.init()\n");
		return -1;
	}
	
	treppensteiger.starten();
	
	return 0;
}

//sudo g++ -o steuerung *.cpp -lpigpio
//sudo g++ -o steuerung -I ./ICM-20948/src -I ./ICM-20948/src/util -I ./i2c -I ./rpi_ws2812 *.cpp ICM-20948/src/*.cpp ICM-20948/src/util/ICM_20948_C.c i2c/smbus.c rpi_ws2812/*.o -lpigpio
//TODO Makefile
// ps -e | grep main