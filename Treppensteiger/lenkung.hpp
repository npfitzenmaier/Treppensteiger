#ifndef lenkung_H
#define lenkung_H

#define _USE_MATH_DEFINES
#include <cmath>

#include "dreistern_laengs.hpp"
#include "winkelsensor.hpp"
#include "matrix.hpp"
#include "vektor.hpp"

class Lenkung{
	public:
		Vektor rpos;
		Vektor lenkachse;
		
		float drehwinkel;
		static const float maximaler_drehwinkel;
		//static constexpr float maximaler_drehwinkel = (30.0/360.0) * (2*M_PI);
		
		bool lenk_drehwinkel_invertieren = false;

		Dreistern_Laengs dreistern;
		Motortreiber motortreiber;
		Winkelsensor winkelsensor;
		
		float p, i, d;
		float Kp, Ki, Kd, T;		
		float fuehrungsgroesse = 0.0;
		float regelabweichung;
		float regelabweichung_zuvor;
		float stellgroesse;
		float minimale_stellgroesse;
		float regelgroesse;
		
		Lenkung(){}
		Lenkung(Vektor &rpos, Vektor &lenkachse, float drehwinkel, Dreistern_Laengs &dreistern
			, Motortreiber &motortreiber, Winkelsensor &winkelsensor)
			: rpos(rpos)
			, lenkachse(lenkachse)
			, drehwinkel(drehwinkel)
			, dreistern(dreistern)
			, motortreiber(motortreiber)
			, winkelsensor(winkelsensor){
			
			drehwinkel = 0.0;
			
			float Kr_kritisch = 25.00;
			float T_kritisch = 0.33;
			
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
		
		Vektor rad_pos(int rad_nummer);
		
		void lenken(float theta);
		void rotieren(Matrix &mat_drehung);
		
		void berechne_stellung();
		void pid_regler_setze_stellgroesse(float vergangene_Zeit);
		void p_regler_setze_stellgroesse(float vergangene_Zeit);
		
	private:
};

#endif