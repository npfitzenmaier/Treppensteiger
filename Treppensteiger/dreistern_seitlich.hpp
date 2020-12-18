#ifndef dreistern_seitlich_H
#define dreistern_seitlich_H

#include "dreistern.hpp"
#include "winkelsensor.hpp"

class Dreistern_Seitlich : public Dreistern{
	public:
		Motortreiber motortreiber_antrieb;
		Winkelsensor winkelsensor_steig;
		Winkelsensor winkelsensor_antrieb;
		
		float antrieb_strecke = 0.0;	// [mm]
		bool antrieb_strecke_invertieren = false;		
		
		static const float uebersetung;
		
		float p, i, d;
		
		float Kp_steig, Ki_steig, Kd_steig, T_steig;
		float fuehrungsgroesse_steig = 0.0;
		float regelabweichung_steig;
		float regelabweichung_zuvor_steig;
		float stellgroesse_steig;
		float minimale_stellgroesse_steig;
		float regelgroesse_steig;
		
		float Kp_antrieb, Ki_antrieb, Kd_antrieb, T_antrieb;		
		float fuehrungsgroesse_antrieb = 0.0;
		float regelabweichung_antrieb;
		float regelabweichung_zuvor_antrieb;
		float stellgroesse_antrieb;
		float minimale_stellgroesse_antrieb;
		float regelgroesse_antrieb;
		
		Dreistern_Seitlich(){}
		Dreistern_Seitlich(Vektor &rpos, Vektor &achse, Vektor &init_rad_richtung, float achse_rad_abstand
			, Motortreiber &motortreiber_steig, Motortreiber &motortreiber_antrieb
			, Winkelsensor &winkelsensor_steig, Winkelsensor &winkelsensor_antrieb)
			: Dreistern(rpos, achse, init_rad_richtung, achse_rad_abstand, motortreiber_steig)
			, motortreiber_antrieb(motortreiber_antrieb)
			, winkelsensor_steig(winkelsensor_steig)
			, winkelsensor_antrieb(winkelsensor_antrieb){
				
			float Kr_kritisch_steig = 17.51;
			float T_kritisch_steig = 0.167;
			float Kr_kritisch_antrieb = 20.0;
			float T_kritisch_antrieb = 0.2;
			
			/*
			// PID-Reglerparameter nach Ziegler-Nichols
			float Kr_steig = 0.6 * Kr_kritisch_steig;
			float Tn_steig = 0.5 * T_kritisch_steig;
			float Tv_steig = 0.125 * T_kritisch_steig;
			
			float Kr_antrieb = 0.6 * Kr_kritisch_antrieb;
			float Tn_antrieb = 0.5 * T_kritisch_antrieb;
			float Tv_antrieb = 0.125 * T_kritisch_antrieb;
			
			Kp_steig = Kr_steig;
			Ki_steig = Kr_steig/Tn_steig;
			Kd_steig = Kr_steig*Tv_steig;
			
			Kp_antrieb = Kr_antrieb;
			Ki_antrieb = Kr_antrieb/Tn_antrieb;
			Kd_antrieb = Kr_antrieb*Tv_antrieb;*/
			
			float Kr_steig = 0.5 * Kr_kritisch_steig;
			
			Kp_steig = Kr_steig;
			Ki_steig = 0.0;
			Kd_steig = 0.0;
			
			float Kr_antrieb = 0.5 * Kr_kritisch_antrieb;
			
			Kp_antrieb = Kr_antrieb;
			Ki_antrieb = 0.0;
			Kd_antrieb = 0.0;
			
			i = 0.0;
			
			regelabweichung_steig = 0.0;
			regelabweichung_antrieb = 0.0;
		}
			
		Vektor rad_pos(int rad_nummer, Vektor ts_rpos);
		void berechne_antrieb_strecke();
		void berechne_steig_drehwinkel();
		
		void pid_regler_setze_stellgroesse_steig(float vergangene_Zeit);		
		void pid_regler_setze_stellgroesse_antrieb(float vergangene_Zeit);
	private:
		int antrieb_rad_umdrehungen = 0;
		float winkel_antrieb_zuvor;
		float winkel_antrieb;
		float winkel_antrieb_offset;
		
		float winkel_steig;
		float winkel_steig_offset;
		
		bool antrieb_streckenberechnung_erster_durchlauf = true;	
		bool steig_drehwinkelberechnung_erster_durchlauf = true;
};

#endif