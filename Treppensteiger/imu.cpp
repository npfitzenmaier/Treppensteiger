#include <cstdio>

#include "ICM-20948/src/ICM_20948_I2C.h"

#include "imu.hpp"

int IMU::init(){
	#ifdef __linux__
		//std::cout << "Setup ICM-20948!\n" << std::endl;
	
		// start connection to IMU and perform default startup
		// ad0val = true for I2C-address = 1101001 (default on SparkFun board), ad0val = false for I2C-address = 1101000
		icm_20948_imu.begin(i2c_schnittstelle->fd, imu_adresse, ICM_20948_ARD_UNUSED_PIN);
		//std::cout << "IMU Chair (Addr 1): " << icm_20948_imu.statusString() << "\n" << std::endl;	
	
		if(icm_20948_imu.status != ICM_20948_Stat_Ok){
			//return -1;
		}
	
		// configure IMUs
		icm_20948_imu.setSampleMode(ICM_20948_InternalSensorID_bm::ICM_20948_Internal_Acc, ICM_20948_LP_CONFIG_CYCLE_e::ICM_20948_Sample_Mode_Continuous);
		ICM_20948_fss_t fss;
		fss.a = ICM_20948_ACCEL_CONFIG_FS_SEL_e::gpm2; //+-2g full scale
		icm_20948_imu.setFullScale(ICM_20948_InternalSensorID_bm::ICM_20948_Internal_Acc, fss);
		ICM_20948_dlpcfg_t dlpcfg;
		dlpcfg.a = ICM_20948_ACCEL_CONFIG_DLPCFG_e::acc_d50bw4_n68bw8; // 3dB-BW = 50.4Hz
		//dlpcfg.a = ICM_20948_ACCEL_CONFIG_DLPCFG_e::acc_d5bw7_n8bw3; // 3db-BW = 5.7 HZ;
		icm_20948_imu.setDLPFcfg(ICM_20948_InternalSensorID_bm::ICM_20948_Internal_Acc, dlpcfg);
		icm_20948_imu.enableDLPF(ICM_20948_InternalSensorID_bm::ICM_20948_Internal_Acc, true);
		ICM_20948_smplrt_t smplrt; // sample rate divider
		smplrt.a = 10; // sample rate = 1125Hz/(1+smplrt) = 102,27Hz
		icm_20948_imu.setSampleRate(ICM_20948_InternalSensorID_bm::ICM_20948_Internal_Acc, smplrt);
	
	
	
		//icm_20948_imu.i2cMasterEnable(false);
	#endif
    return 0;	
}

void IMU::daten_anfordern(){
	#ifdef __linux__
		//if(icm_20948_imu.dataReady()){
			icm_20948_imu.getAGMT();
		//}
		/*if(icm_20948_imu.status != ICM_20948_Stat_Ok){
			icm_20948_imu.swReset();
		}*/
	
		status = icm_20948_imu.status;
	
		// milli g's
		acc.x = icm_20948_imu.accX();
		acc.y = icm_20948_imu.accY();
		acc.z = icm_20948_imu.accZ();
	
		// degrees per second
		gyr.x = icm_20948_imu.gyrX();
		gyr.y = icm_20948_imu.gyrY();
		gyr.z = icm_20948_imu.gyrZ();
	
		// micro teslas
		mag.x = icm_20948_imu.magX();
		mag.y = icm_20948_imu.magY();
		mag.z = icm_20948_imu.magZ();
	
		temperatur = icm_20948_imu.temp();
	#endif
}

void IMU::sys_koordinaten_berechnen(){
	mag.y = -mag.y;
	mag.z = -mag.z;
	
	sys_acc = Matrix::mat_vek_mult(mat_imu_zu_sys_koordinaten, acc);
	sys_gyr = Matrix::mat_vek_mult(mat_imu_zu_sys_koordinaten, gyr);
	sys_mag = Matrix::mat_vek_mult(mat_imu_zu_sys_koordinaten, mag);	
}