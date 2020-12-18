#ifndef dreistern_H
#define dreistern_H

#define _USE_MATH_DEFINES
#include <cmath>

#include "matrix.hpp"
#include "vektor.hpp"
#include "motortreiber.hpp"

class Dreistern{
	public:
		Vektor rpos;
		Vektor achse;
		float achse_rad_abstand;
		
		Vektor rpos_raeder[2];
		
		Motortreiber motortreiber_steig;
		
		static const float rad_durchmesser;
		static const float rad_umfang;
		
		float drehwinkel;
		
		bool steig_drehwinkel_invertieren = false;		
		
		Dreistern(){}
		Dreistern(Vektor &rpos, Vektor &achse, Vektor &init_rad_richtung, float achse_rad_abstand
			, Motortreiber &motortreiber_steig)
			: rpos(rpos) // Attribut(Initialisierer)
			, achse(achse)
			, achse_rad_abstand(achse_rad_abstand)
			, motortreiber_steig(motortreiber_steig){
			
			float theta = (120.0/360.0) * 2*M_PI;
			Matrix mat_drehung1 = Matrix::rotation_mat(achse, theta);
			Matrix mat_drehung2 = Matrix::rotation_mat(achse, -theta);
			
			Vektor v = init_rad_richtung;
			v.skalieren(achse_rad_abstand);
			
			rpos_raeder[0] = v;
			rpos_raeder[1] = Matrix::mat_vek_mult(mat_drehung1, rpos_raeder[0]);
			rpos_raeder[2] = Matrix::mat_vek_mult(mat_drehung2, rpos_raeder[0]);

			drehwinkel = 0.0;			
		}		
		Vektor rad_pos(int rad_nummer);
		void drehen(float theta); // positiv in Fahrtrichtung
		void rotieren(Matrix &mat_drehung);		
	private:
};

#endif