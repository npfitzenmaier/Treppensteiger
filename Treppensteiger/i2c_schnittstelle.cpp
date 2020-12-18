#include <cstdio>

#include "i2c_schnittstelle.hpp"

int I2C_Schnittstelle::init(){
	#ifdef __linux__
		fd = -1;
		fd = open(i2cFilename, O_RDWR);
		if(fd == -1){
			printf( "Couldn't open i2c filedescriptor\n" );
			return -1;
		}
	#endif
	return 0;
}