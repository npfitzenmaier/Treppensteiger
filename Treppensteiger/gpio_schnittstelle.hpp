#ifndef gpio_schnittstelle_H
#define gpio_schnittstelle_H

#ifdef __linux__
	#include <pigpio.h>
#endif

/*
Yes, all of the B+'s GPIO pins have internal pull-up or pull-down resistors that can be controlled from your code.

Pull-up is 50K min – 65K max. Pull-down is 50K min – 60K max.
To round off your answer it might be worth mentioning that pins 3 and 5 have hard-wired 1k8 pull-ups to 3V3. 
These pull-ups are needed for the proper operation of the I2C bus. 
A consequence is that the internal pull-downs have no effect on these pins (the internal pull-downs are much weaker than the external pull-ups).
*/

// The power on state is defined in BCM2835 ARM Peripherals page 102.
// Basically all GPIO are in INPUT mode, 
// GPIO 0-8 have pull-ups to 3V3 enabled,
// GPIO 9-27 have pull-downs to 0V enabled.

//#define GPIO_UART_BLDC_OE 			21	// BCM 21, Pin 40, power on state: pull-down to 0V enabled

class GPIO_Schnittstelle{
	public:
		~GPIO_Schnittstelle(){
			#ifdef __linux__
				gpioTerminate();	// This function resets the used DMA channels, releases memory, and terminates any running threads
			#endif
		}
		int init();
	private:
};

#endif