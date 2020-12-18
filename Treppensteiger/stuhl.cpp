#define _USE_MATH_DEFINES
#include <cmath>

#include "stuhl.hpp"

const float Stuhl::maximaler_drehwinkel = (15.0/360.0) * (2*M_PI);

void Stuhl::stellen(float theta){	
	Matrix mat_drehung = Matrix::rotation_mat(achse, theta);
	rotieren(mat_drehung);
	
	drehwinkel += theta;
}

void Stuhl::rotieren(Matrix &mat_drehung){
	rpos = Matrix::mat_vek_mult(mat_drehung, rpos);
	achse = Matrix::mat_vek_mult(mat_drehung, achse);
}

void Stuhl::berechne_stellung(float drehwinkel_fahrgestell){
	Vektor v1(0.0, 0.0, 0.0);
	Vektor z_achse(0.0, 0.0, 1.0);
	
	if(std::abs(imu.sys_acc.betrag() - 1000.0) < 300.0){
		v1 = imu.sys_acc;
		v1.y = 0.0;
		v1.normieren();
		
		if(v1.x >= 0.0){
			drehwinkel = -Vektor::winkel(v1, z_achse);	// nach vorne kippen
			drehwinkel -= drehwinkel_fahrgestell;
		}
		else{
			drehwinkel = Vektor::winkel(v1, z_achse);	// nach hinten kippen
			drehwinkel -= drehwinkel_fahrgestell;
		}		
	}
}

void Stuhl::pid_regler_setze_stellgroesse(float vergangene_Zeit){
	regelgroesse = drehwinkel;
	regelabweichung = fuehrungsgroesse - regelgroesse;
	
	T = vergangene_Zeit;
	
	regelabweichung_zuvor = regelabweichung;
	
	p = Kp * regelabweichung;
    i += Ki * (((regelabweichung + regelabweichung_zuvor)/2)*T);
    d = Kd * ((regelabweichung - regelabweichung_zuvor)/T);
	
	stellgroesse = p + i + d;
	
	if(std::abs(stellgroesse) > 1.0){
		stellgroesse = stellgroesse/std::abs(stellgroesse);
	}
	
	//if(std::abs(stellgroesse) < minimale_stellgroesse)	stellgroesse = 0.0;
	
	motortreiber_links.setze_antriebsleistung(stellgroesse);
	motortreiber_rechts.setze_antriebsleistung(stellgroesse);
}