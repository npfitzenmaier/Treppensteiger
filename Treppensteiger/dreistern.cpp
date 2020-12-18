#include "dreistern.hpp"

const float Dreistern::rad_durchmesser = 125.0;					// [mm]
const float Dreistern::rad_umfang = rad_durchmesser * M_PI;		// [mm]

Vektor Dreistern::rad_pos(int rad_nummer){
	return rpos + rpos_raeder[rad_nummer];
}

void Dreistern::drehen(float theta){	// positiv in Fahrtrichtung
	Matrix mat_drehung = Matrix::rotation_mat(achse, theta);
	for(int i=0; i<3; i++){
		rpos_raeder[i] = Matrix::mat_vek_mult(mat_drehung, rpos_raeder[i]);
	}	

	drehwinkel += theta;
	if (drehwinkel < 0.0)
		drehwinkel += 2*M_PI;
	if (drehwinkel > (2*M_PI))
		drehwinkel -= 2*M_PI;
}

void Dreistern::rotieren(Matrix &mat_drehung){
	rpos = Matrix::mat_vek_mult(mat_drehung, rpos);
	achse = Matrix::mat_vek_mult(mat_drehung, achse);
	for(int i=0; i<3; i++){
		rpos_raeder[i] = Matrix::mat_vek_mult(mat_drehung, rpos_raeder[i]);
	}	
}