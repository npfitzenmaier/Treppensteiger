#ifndef dreistern_laengs_H
#define dreistern_laengs_H

#define _USE_MATH_DEFINES
#include <cmath>
#include "dreistern.hpp"

class Dreistern_Laengs : public Dreistern{
	public:
		short position_rohwert;		
		
		
		float p, i, d;
		float Kp, Ki, Kd, T;
		float fuehrungsgroesse = 0.0;
		float regelabweichung;
		float regelabweichung_zuvor;
		float stellgroesse;
		float minimale_stellgroesse;
		float regelgroesse;
		
		Dreistern_Laengs(){}
		Dreistern_Laengs(Vektor &rpos, Vektor &achse, Vektor &init_rad_richtung, float achse_rad_abstand
			, Motortreiber &motortreiber_steig)
			: Dreistern(rpos, achse, init_rad_richtung, achse_rad_abstand, motortreiber_steig){
				
			float Kr_kritisch = 5.09;
			float T_kritisch = 0.2778;
			
			/*// PID-Reglerparameter nach Ziegler-Nichols
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
			
		void position_anfordern();
		void pid_regler_setze_stellgroesse(float vergangene_Zeit);
	private:
		void berechne_rotation();
		
		float winkel_steig_offset;
		
		bool steig_drehwinkelberechnung_erster_durchlauf = true;
};

#endif