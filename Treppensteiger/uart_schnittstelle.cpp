#include "uart_schnittstelle.hpp"

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>

int UART_Schnittstelle::init(){
	#ifdef __linux__
		//-------------------------
		//----- SETUP USART 0 -----
		//-------------------------
		//At bootup, pins 8 and 10 are already set to UART0_TXD, UART0_RXD (ie the alt0 function) respectively
		fd = -1;
	
		//OPEN THE UART
		//The flags (defined in fcntl.h):
		//	Access modes (use 1 of these):
		//		O_RDONLY - Open for reading only.
		//		O_RDWR - Open for reading and writing.
		//		O_WRONLY - Open for writing only.
		//
		//	O_NDELAY / O_NONBLOCK (same function) - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
		//											if there is no input immediately available (instead of blocking). Likewise, write requests can also return
		//											immediately with a failure status if the output can't be written immediately.
		//
		//	O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.
		fd = open("/dev/ttyS0", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
		if(fd == -1){
			//ERROR - CAN'T OPEN SERIAL PORT
			printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
			return -1;
		}
	
		//gpioWrite(GPIO_UART_BLDC_OE, 1);
	
		//CONFIGURE THE UART
		//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
		//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
		//	CSIZE:- CS5, CS6, CS7, CS8
		//	CLOCAL - Ignore modem status lines
		//	CREAD - Enable receiver
		//	IGNPAR = Ignore characters with parity errors
		//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
		//	PARENB - Parity enable
		//	PARODD - Odd parity (else even)
	
		/*tcgetattr(fd, &options);
		options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;		//<Set baud rate, CS8 <- 8 Bit
		options.c_iflag = IGNPAR;
		options.c_oflag = 0;
		options.c_lflag = 0;
		tcflush(fd, TCIFLUSH);
		tcsetattr(fd, TCSANOW, &options);*/
	
		struct termios2 tio;
		ioctl(fd, TCGETS2, &tio);
	
		tio.c_cflag = CS8 | CLOCAL | CREAD;
		tio.c_iflag = IGNPAR;
		tio.c_oflag = 0;
		tio.c_lflag = 0;
	
		tio.c_cflag &= ~CBAUD;
		tio.c_cflag |= BOTHER;
		tio.c_ispeed = 100000;
		tio.c_ospeed = 100000;
		int r = ioctl(fd, TCSETS2, &tio);

		if(r == 0){
			//printf("Changed successfully.\n");
		}
		else{
			printf("Error - Unable to configure UART.  Ensure it is not in use by another application\n");
			return -1;
		}
	#endif
	return 0;
}