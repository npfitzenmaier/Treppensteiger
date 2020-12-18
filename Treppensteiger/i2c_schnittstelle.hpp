#ifndef i2c_schnittstelle_H
#define i2c_schnittstelle_H  

#include <fcntl.h>
#ifdef __linux__
#include <unistd.h>
#endif

//100 kHz
//apt-get install libi2c-dev
//https://de.i2c-bus.org/

class I2C_Schnittstelle{
	public:
		int fd;
		const char* i2cFilename = "/dev/i2c-1";
		//constexpr char* i2cFilename = (char*)"/dev/i2c-1"; // I2C device 1 on the Raspberry Pi
		~I2C_Schnittstelle(){
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