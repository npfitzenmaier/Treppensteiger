#include "dreistern_laengs.hpp"

void Dreistern_Laengs::position_anfordern(){
	motortreiber_steig.bldc_position_anfordern(position_rohwert);
	berechne_rotation();
}

void Dreistern_Laengs::berechne_rotation(){
	float drehwinkel_gesamt;
	float ganze_umdrehungen;	
	float drehwinkel_ergebnis;

	drehwinkel_gesamt = (((float) position_rohwert) / 1094.0);
	
	if(drehwinkel_gesamt >= 0.0){
		ganze_umdrehungen = std::floor(drehwinkel_gesamt);
		drehwinkel_ergebnis = (drehwinkel_gesamt - ganze_umdrehungen) * (2*M_PI);
	}
	else{
		ganze_umdrehungen = std::ceil(drehwinkel_gesamt);
		drehwinkel_ergebnis = (drehwinkel_gesamt - ganze_umdrehungen) * (2*M_PI);
		drehwinkel_ergebnis += (2*M_PI);
	}
	
	if(steig_drehwinkel_invertieren){
		drehwinkel_ergebnis = 2*M_PI - drehwinkel_ergebnis;
	}
	
	if(steig_drehwinkelberechnung_erster_durchlauf){
		steig_drehwinkelberechnung_erster_durchlauf = false;
		winkel_steig_offset = drehwinkel_ergebnis;// - (0.75 / 180.0) * M_PI;	// Dreisterne laengs stehen hoeher als die Mittelachse
	}
	
	drehwinkel = drehwinkel_ergebnis - winkel_steig_offset;
	
	if(drehwinkel < 0.0){
		drehwinkel += 2*M_PI;
	}
	if(drehwinkel > (2*M_PI)){
		drehwinkel -= 2*M_PI;
	}
}

void Dreistern_Laengs::pid_regler_setze_stellgroesse(float vergangene_Zeit){
	regelgroesse = drehwinkel;
	regelabweichung = fuehrungsgroesse - regelgroesse;	
	
	if(regelabweichung >= M_PI){
		regelabweichung = regelabweichung - 2*M_PI;
	}	
	if(regelabweichung < -M_PI){
		regelabweichung = regelabweichung + 2*M_PI;
	}
	
	/*if(std::abs(regelabweichung) < (1.0/360.0)*2*M_PI){
		regelabweichung = 0.0;
	}*/
	
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
	
	motortreiber_steig.setze_antriebsleistung(stellgroesse);
}