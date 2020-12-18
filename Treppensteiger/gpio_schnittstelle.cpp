#include <cstdio>

#include "gpio_schnittstelle.hpp"
#include "winkelsensor.hpp"

int GPIO_Schnittstelle::init(){
	#ifdef __linux__
		if (gpioInitialise() < 0)	// Initialises the library.
		{
		   // pigpio initialisation failed.
		   printf("Error - Unable to initialize GPIOs. Ensure it is not in use by another application\n");
		   return -1;
		}
	
		gpioWrite(GPIO_CS_TLE5012_ANTRIEB_RECHTS, 1);
		gpioWrite(GPIO_CS_TLE5012_ANTRIEB_LINKS, 1);
		gpioWrite(GPIO_CS_TLE5012_STEIG_LINKS, 1);
		gpioWrite(GPIO_CS_TLE5012_STEIG_RECHTS, 1);
		gpioWrite(GPIO_CS_TLE5012_LENKUNG_HINTEN, 1);
		gpioWrite(GPIO_CS_TLE5012_LENKUNG_VORNE, 1);
	
		// pigpio initialised okay.
	#endif
	return 0;
}