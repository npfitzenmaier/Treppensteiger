#include <cstdio>

#include "rgb_led.hpp"

int RGB_LED::init(){
	#ifdef __linux__
		ws2811_t ledstring_tmp =
		{
			.freq = TARGET_FREQ,
			.dmanum = DMA,
			.channel =
			{
				[0] =
				{
					.gpionum = GPIO_PIN,				
					.invert = 0,
					.count = LED_COUNT,
					.strip_type = STRIP_TYPE,
					.brightness = 255,				
				},
				[1] =
				{
					.gpionum = 0,				
					.invert = 0,
					.count = 0,
					.brightness = 0,
				},
			},
		};
	
		ledstring = ledstring_tmp;
	
		ws2811_return_t ret;
		if((ret = ws2811_init(&ledstring)) != WS2811_SUCCESS)
		{
			printf("ws2811_init failed: %s\n", ws2811_get_return_t_str(ret));
			return -1;
		}
	#endif
	return 0;
}

void RGB_LED::setze_rgb_led_farbe(unsigned char rot, unsigned char gruen, unsigned char blau){
	#ifdef __linux__
		ws2811_return_t ret;
		ledstring.channel[0].leds[0] = (uint32_t)blau << 16 | (uint32_t)gruen << 8 | (uint32_t)rot;
	
		if((ret = ws2811_render(&ledstring)) != WS2811_SUCCESS){
			printf("ws2811_render failed: %s\n", ws2811_get_return_t_str(ret));
		}
	#endif
}