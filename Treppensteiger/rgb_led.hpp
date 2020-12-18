#ifndef rgb_led_H
#define rgb_led_H

#ifdef __linux__
	#include <stdint.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <sys/mman.h>
	#include <signal.h>
	#include <stdarg.h>
	#include <getopt.h>


	#include "clk.h"
	#include "gpio.h"
	#include "dma.h"
	#include "pwm.h"
	#include "version.h"
	#include "ws2811.h"
#endif

#define TARGET_FREQ             WS2811_TARGET_FREQ
#define GPIO_PIN                18
#define DMA                     10
//#define STRIP_TYPE            WS2811_STRIP_RGB		// WS2812/SK6812RGB integrated chip+leds
#define STRIP_TYPE              WS2811_STRIP_GBR		// WS2812/SK6812RGB integrated chip+leds
//#define STRIP_TYPE            SK6812_STRIP_RGBW		// SK6812RGBW (NOT SK6812RGB)

#define LED_COUNT               1

class RGB_LED{
	public:
	#ifdef __linux__
		ws2811_t ledstring;
	#endif
		
		int init();
		void setze_rgb_led_farbe(unsigned char rot, unsigned char gruen, unsigned char blau);
		
		~RGB_LED(){
			#ifdef __linux__
				ws2811_fini(&ledstring);
			#endif
		}
	private:
};

#endif