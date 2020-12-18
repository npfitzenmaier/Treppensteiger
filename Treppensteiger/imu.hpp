#ifndef imu_H
#define imu_H

#ifdef __linux__
	#include "ICM_20948_I2C.h"
#endif

#include "i2c_schnittstelle.hpp"
#include "matrix.hpp"
#include "vektor.hpp"

#define ICM_20948_STUHL			true
#define ICM_20948_FAHRGESTELL	false

class IMU{
	public:
		// IMU Koordinatensystem
		Vektor acc;	// micro teslas
		Vektor gyr;	// milli g's
		Vektor mag;	// degrees per second
		
		// System Koordinatensystem
		Matrix mat_imu_zu_sys_koordinaten;
		
		Vektor sys_acc;	// micro teslas
		Vektor sys_gyr;	// milli g's
		Vektor sys_mag;	// degrees per second
		
		#ifdef __linux__
			ICM_20948_I2C icm_20948_imu;
		#endif
		
		int status;
		
		float temperatur;
	
		bool imu_adresse;
		I2C_Schnittstelle* i2c_schnittstelle;
		
		IMU(){}
		IMU(I2C_Schnittstelle* i2c_schnittstelle, bool imu_adresse)
			: i2c_schnittstelle(i2c_schnittstelle)
			, imu_adresse(imu_adresse){}
		
		int init();
		void daten_anfordern();
		void sys_koordinaten_berechnen();
		
	private:
		//ICM_20948_I2C icm_20948_imu;
};

#endif