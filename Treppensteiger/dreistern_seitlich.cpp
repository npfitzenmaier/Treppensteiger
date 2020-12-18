#include "dreistern_seitlich.hpp"

const float Dreistern_Seitlich::uebersetung = 27.0 / 28.0;	// (9/14) * (60/40) = 27/28

Vektor Dreistern_Seitlich::rad_pos(int rad_nummer, Vektor ts_pos){
	return ts_pos + Dreistern::rad_pos(rad_nummer);
}

void Dreistern_Seitlich::berechne_antrieb_strecke(){
	if(antrieb_strecke_invertieren){
		winkel_antrieb = 2*M_PI - winkelsensor_antrieb.winkel_radiant;
	}
	else{
		winkel_antrieb = winkelsensor_antrieb.winkel_radiant;
	}
	
	if(antrieb_streckenberechnung_erster_durchlauf){
		antrieb_streckenberechnung_erster_durchlauf = false;
		winkel_antrieb_zuvor = winkel_antrieb;
		winkel_antrieb_offset = winkel_antrieb;
	}	
	
	if(winkel_antrieb < M_PI_4 && winkel_antrieb_zuvor > (3*M_PI_4)){
		antrieb_rad_umdrehungen++;
	}
	if(winkel_antrieb > (3*M_PI_4) && winkel_antrieb_zuvor < M_PI_4){
		antrieb_rad_umdrehungen--;
	}
		
	antrieb_strecke = ((winkel_antrieb - winkel_antrieb_offset)/(2*M_PI) + ((float) antrieb_rad_umdrehungen)) * uebersetung * rad_umfang;
	antrieb_strecke /= 1000.0; // [mm] -> [m]
		
	winkel_antrieb_zuvor = winkel_antrieb;
}

void Dreistern_Seitlich::berechne_steig_drehwinkel(){
	if(steig_drehwinkel_invertieren){
		winkel_steig = 2*M_PI - winkelsensor_steig.winkel_radiant;
	}
	else{
		winkel_steig = winkelsensor_steig.winkel_radiant;
	}
	
	if(steig_drehwinkelberechnung_erster_durchlauf){
		steig_drehwinkelberechnung_erster_durchlauf = false;
		winkel_steig_offset = winkel_steig;
	}
	
	drehwinkel = winkel_steig - winkel_steig_offset;
	
	if(drehwinkel < 0.0){
		drehwinkel += 2*M_PI;
	}
	if(drehwinkel > (2*M_PI)){
		drehwinkel -= 2*M_PI;
	}
}

void Dreistern_Seitlich::pid_regler_setze_stellgroesse_steig(float vergangene_Zeit){
	regelgroesse_steig = drehwinkel;
	regelabweichung_steig = fuehrungsgroesse_steig - regelgroesse_steig;
	
	if(regelabweichung_steig >= M_PI){
		regelabweichung_steig = regelabweichung_steig - 2*M_PI;
	}	
	if(regelabweichung_steig < -M_PI){
		regelabweichung_steig = regelabweichung_steig + 2*M_PI;
	}
	
	regelabweichung_zuvor_steig = regelabweichung_steig;
	
	T_steig = vergangene_Zeit;
	
	p = Kp_steig * regelabweichung_steig;
    i += Ki_steig * (((regelabweichung_steig + regelabweichung_zuvor_steig)/2)*T_steig);
    d = Kd_steig * ((regelabweichung_steig - regelabweichung_zuvor_steig)/T_steig);
	
	stellgroesse_steig = p + i + d;
	
	if(std::abs(stellgroesse_steig) > 1.0){
		stellgroesse_steig = stellgroesse_steig/std::abs(stellgroesse_steig);
	}
	
	//if(std::abs(stellgroesse_steig) < minimale_stellgroesse_steig) stellgroesse_steig = 0.0;
	
	motortreiber_steig.setze_antriebsleistung(stellgroesse_steig);
}

void Dreistern_Seitlich::pid_regler_setze_stellgroesse_antrieb(float vergangene_Zeit){
	regelgroesse_antrieb = antrieb_strecke;
	regelabweichung_antrieb = fuehrungsgroesse_antrieb - regelgroesse_antrieb;
	
	regelabweichung_zuvor_antrieb = regelabweichung_antrieb;
	
	T_antrieb = vergangene_Zeit;
	
	p = Kp_antrieb * regelabweichung_antrieb;
    i += Ki_antrieb * (((regelabweichung_antrieb + regelabweichung_zuvor_antrieb)/2)*T_antrieb);
    d = Kd_antrieb * ((regelabweichung_antrieb - regelabweichung_zuvor_antrieb)/T_antrieb);
	
	stellgroesse_antrieb = p + i + d;
	
	if(std::abs(stellgroesse_antrieb) > 1.0){
		stellgroesse_antrieb = stellgroesse_antrieb/std::abs(stellgroesse_antrieb);
	}
	
	//if(std::abs(stellgroesse_antrieb) < minimale_stellgroesse_antrieb) stellgroesse_antrieb = 0.0;
	
	motortreiber_antrieb.setze_antriebsleistung(stellgroesse_antrieb);
}