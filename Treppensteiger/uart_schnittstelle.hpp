#ifndef uart_schnittstelle_H
#define uart_schnittstelle_H

#ifdef __linux__
	#include <unistd.h>
	#include <fcntl.h>
	//#include <termios.h>
	//#include <asm/termios.h>
	#include <sys/ioctl.h>
	#include <asm/ioctls.h>
	#include <asm/termbits.h>
#elif _WIN32
#endif

//https://github.com/torvalds/linux/blob/master/drivers/tty/serial/serial_core.c
//https://stackoverflow.com/questions/19440268/how-to-set-a-non-standard-baudrate-on-a-serial-port-device-on-linux

class UART_Schnittstelle{
	public:	
		int fd;
		
		~UART_Schnittstelle(){
			if(fd != -1){
				#ifdef __linux__
					close(fd);
				#endif
			}
		}
		
		int init();
	private:
		#ifdef __linux__
			struct termios options;
		#endif
};

#endif