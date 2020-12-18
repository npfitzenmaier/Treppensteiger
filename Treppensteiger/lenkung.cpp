#include "lenkung.hpp"

const float Lenkung::maximaler_drehwinkel = (30.0/360.0) * (2*M_PI);

Vektor Lenkung::rad_pos(int rad_nummer){
	return rpos + dreistern.rad_pos(rad_nummer);
}

void Lenkung::lenken(float theta){
	Matrix mat_drehung = Matrix::rotation_mat(lenkachse, theta);
	lenkachse = Matrix::mat_vek_mult(mat_drehung, lenkachse);
	dreistern.rotieren(mat_drehung);
	
	drehwinkel += theta;
}

void Lenkung::rotieren(Matrix &mat_drehung){
	rpos = Matrix::mat_vek_mult(mat_drehung, rpos);
	lenkachse = Matrix::mat_vek_mult(mat_drehung, lenkachse);
	dreistern.rotieren(mat_drehung);
}

void Lenkung::berechne_stellung(){
	float theta;
	if(lenk_drehwinkel_invertieren){
		theta = 2*M_PI - winkelsensor.winkel_radiant;
	}
	else{
		theta = winkelsensor.winkel_radiant;
	}
	drehwinkel = theta - M_PI;
}

void Lenkung::pid_regler_setze_stellgroesse(float vergangene_Zeit){
	regelgroesse = drehwinkel;
	regelabweichung = fuehrungsgroesse - regelgroesse;
	
	regelabweichung_zuvor = regelabweichung;
	
	T = vergangene_Zeit;
	
	p = Kp * regelabweichung;
    i += Ki * (((regelabweichung + regelabweichung_zuvor)/2)*T);
    d = Kd * ((regelabweichung - regelabweichung_zuvor)/T);
	
	stellgroesse = p + i + d;
	
	if(std::abs(stellgroesse) > 1.0){
		stellgroesse = stellgroesse/std::abs(stellgroesse);
	}
	
	//if(std::abs(stellgroesse) < minimale_stellgroesse)	stellgroesse = 0.0;
	
	motortreiber.setze_antriebsleistung(stellgroesse);
}