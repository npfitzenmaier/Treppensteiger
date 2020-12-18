#define _USE_MATH_DEFINES
#include <cmath>

#include "sequenzregler.hpp"

bool Sequenzregler::regeldifferenzen_ausgeregelt(){
	struct zustandsgroessen fuehrungsgroesse = fuehrungsgroessen_datensatz.at(ausgewaehlter_regeldatensatz);
	bool ausgeregelt = true;
	ausgeregelt = regeldifferenz_winkel_ausgeregelt(fuehrungsgroesse.bldc_vorne_rotation, regelgroessen.bldc_vorne_rotation, 5.0) ? ausgeregelt : false;
	//ausgeregelt = regeldifferenz_winkel_ausgeregelt(fuehrungsgroesse.bldc_hinten_rotation, regelgroessen.bldc_hinten_rotation) ? ausgeregelt : false;
	//ausgeregelt = regeldifferenz_winkel_ausgeregelt(fuehrungsgroesse.steig_links_rotation, regelgroessen.steig_links_rotation) ? ausgeregelt : false;
	//ausgeregelt = regeldifferenz_winkel_ausgeregelt(fuehrungsgroesse.steig_rechts_rotation, regelgroessen.steig_rechts_rotation) ? ausgeregelt : false;
	//ausgeregelt = regeldifferenz_ausgeregelt(fuehrungsgroesse.antrieb_links_strecke, regelgroessen.antrieb_links_strecke, 0.01) ? ausgeregelt : false;
	//ausgeregelt = regeldifferenz_ausgeregelt(fuehrungsgroesse.antrieb_rechts_strecke, regelgroessen.antrieb_rechts_strecke, 0.01) ? ausgeregelt : false;
	////ausgeregelt = regeldifferenz_winkel_ausgeregelt(fuehrungsgroesse.stuhl_rotation, regelgroessen.stuhl_rotation) ? ausgeregelt : false;
	////ausgeregelt = regeldifferenz_winkel_ausgeregelt(fuehrungsgroesse.fahrgestell_rotation, regelgroessen.fahrgestell_rotation) ? ausgeregelt : false;	
	////ausgeregelt = regeldifferenz_winkel_ausgeregelt(fuehrungsgroesse.lenkung_vorne_rotation, regelgroessen.lenkung_vorne_rotation) ? ausgeregelt : false;
	////ausgeregelt = regeldifferenz_winkel_ausgeregelt(fuehrungsgroesse.lenkung_hinten_rotation, regelgroessen.lenkung_hinten_rotation) ? ausgeregelt : false;
	//ausgeregelt = regeldifferenz_ausgeregelt(fuehrungsgroesse.stellglied_vorne_kraft, regelgroessen.stellglied_vorne_kraft, 50.0) ? ausgeregelt : false;
	//ausgeregelt = regeldifferenz_ausgeregelt(fuehrungsgroesse.stellglied_hinten_kraft, regelgroessen.stellglied_hinten_kraft, 50.0) ? ausgeregelt : false;
	//ausgeregelt = regeldifferenz_winkel_ausgeregelt(fuehrungsgroesse.arm_vorne_rotation, regelgroessen.arm_vorne_rotation) ? ausgeregelt : false;
	//ausgeregelt = regeldifferenz_winkel_ausgeregelt(fuehrungsgroesse.arm_hinten_rotation, regelgroessen.arm_hinten_rotation) ? ausgeregelt : false;
	
	return ausgeregelt;
}

bool Sequenzregler::regeldifferenz_winkel_ausgeregelt(float fuehrungsgroesse, float regelgroesse, float maximale_regelabweichung){
	float regelabweichung = berechne_winkel_regelabweichung(fuehrungsgroesse, regelgroesse);
	
	if(std::abs(regelabweichung) < maximale_regelabweichung)	return true;
	else														return false;
}

bool Sequenzregler::regeldifferenz_ausgeregelt(float fuehrungsgroesse, float regelgroesse, float maximale_regelabweichung){
	float regelabweichung = (fuehrungsgroesse - regelgroesse);
	
	if(std::abs(regelabweichung) < maximale_regelabweichung)	return true;
	else														return false;
}

float Sequenzregler::berechne_winkel_regelabweichung(float fuehrungsgroesse, float regelgroesse){
	float regelabweichung = fuehrungsgroesse - regelgroesse;
	
	if(regelabweichung >= M_PI){
		regelabweichung = regelabweichung - 2*M_PI;
	}	
	if(regelabweichung < -M_PI){
		regelabweichung = regelabweichung + 2*M_PI;
	}
	
	return regelabweichung;
}