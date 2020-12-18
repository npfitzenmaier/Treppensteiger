#ifndef spi_schnittstelle_H
#define spi_schnittstelle_H

#ifdef __linux__
	#include <fcntl.h>
	#include <unistd.h>	
	#include <linux/spi/spidev.h>
#endif

#include <cstdint>

class SPI_Schnittstelle{
	public:
		#ifdef __linux__
			uint8_t spi_mode = SPI_MODE_1;	// TODO: try SPI_MODE_1 | SPI_3WIRE --> MOSI serves as MOMI (https://www.raspberrypi.org/documentation/hardware/raspberrypi/spi/README.md see notes in section Linux Driver - Supported Mode bits)
		#endif
		uint8_t spi_bpw = 8;			// RPi supports 8 bit only (and 9 bit in LoSSI mode)
		uint32_t spi_speed = 100000;
		int fd = -1;
		// Raspberry Pi SPI interface device
		const char* spi_device = "/dev/spidev0.0";
		~SPI_Schnittstelle(){
			#ifdef __linux__
				if(fd != -1){
					close(fd);
				}
			#endif
		}
		int init();
	private:
};

#endif