#ifdef __linux__
	#include <sys/ioctl.h>
#endif
#include <cstdio>

#include "spi_schnittstelle.hpp"

int SPI_Schnittstelle::init(){
	#ifdef __linux__
		// Open SPI device
		fd = open(spi_device, O_RDWR);
		if(fd == -1){
			printf( "Couldn't open spi filedescriptor\n" );
			return -1;
		}
	
		// Setup SPI interface for communication with SSC sensor interface
	
		/*
		* Configure Set SPI mode
		*/
		if (ioctl(fd, SPI_IOC_WR_MODE, &spi_mode) == -1) // write mode configuration
		{
			//perror("ioctl mode write");
			return -1;
		}

		if (ioctl(fd, SPI_IOC_RD_MODE, &spi_mode) == -1) // read mode configuration
		{
			//perror("ioctl mode read");
			return -1;
		}

		/*
		* Configure bits per word
		*/
		if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bpw) == -1) // write bpw configuration
		{
			//perror("ioctl bpw write");
			return -1;
		}

		if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bpw) == -1)// read bpw configuration
		{
			//perror("ioctl bpw read");
			return -1;
		}


		/*
		* Configure speed
		*/
		if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed) == -1) // write speed configuration
		{
			//perror("ioctl speed write");
			return -1;
		}

		if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed) == -1) // read speed configuration
		{
			//perror("ioctl speed read");
			return -1;
		}


		/*printf("spi mode: 0x%x\n", spi_mode);
		printf("bits per word: %d\n", spi_bpw);
		printf("max speed: %d kHz\n", spi_speed / 1000);*/
	#endif
	return 0;
}