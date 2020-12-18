#ifndef stuhl_H
#define stuhl_H

#include "matrix.hpp"
#include "vektor.hpp"
#include "imu.hpp"
#include "motortreiber.hpp"

class Stuhl{
	public:
		Vektor rpos;
		Vektor achse;
		
		float drehwinkel;
		
		static const float maximaler_drehwinkel;
		
		IMU imu;
		
		Motortreiber motortreiber_links;
		Motortreiber motortreiber_rechts;
		
		float p, i, d;
		float Kp, Ki, Kd, T;
		float fuehrungsgroesse = 0.0;
		float regelabweichung;
		float regelabweichung_zuvor;
		float stellgroesse;
		float minimale_stellgroesse;
		float regelgroesse;
		
		Stuhl(){}
		Stuhl(Vektor &rpos, Vektor &achse, IMU &imu, Motortreiber &motortreiber_links, Motortreiber &motortreiber_rechts)
			: rpos(rpos)
			, achse(achse)
			, imu(imu)
			, motortreiber_links(motortreiber_links)
			, motortreiber_rechts(motortreiber_rechts){
			
			drehwinkel = 0.0;
			
			float Kr_kritisch = 6.37;
			float T_kritisch = 0.2;			
			
			/*
			// PID-Reglerparameter nach Ziegler-Nichols
			float Kr = 0.6 * Kr_kritisch;
			float Tn = 0.5 * T_kritisch;
			float Tv = 0.125 * T_kritisch;
			
			Kp = Kr;
			Ki = Kr/Tn;
			Kd = Kr*Tv;*/			
			
			float Kr = 0.5 * Kr_kritisch;
			
			Kp = Kr;
			Ki = 0.0;
			Kd = 0.0;
				
			i = 0.0;
			
			regelabweichung = 0.0;
		}
			
		void rotieren(Matrix &mat_drehung);
		void stellen(float theta);
		void berechne_stellung(float drehwinkel_fahrgestell);
		void pid_regler_setze_stellgroesse(float vergangene_Zeit);	
	private:
};

#endif