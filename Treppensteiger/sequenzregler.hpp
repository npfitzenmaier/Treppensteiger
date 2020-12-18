#ifndef sequenzregler_H
#define sequenzregler_H

#include <vector>

struct zustandsgroessen{
	float bldc_vorne_rotation;
	float bldc_hinten_rotation;
	float steig_links_rotation;
	float steig_rechts_rotation;
	float antrieb_links_strecke;
	float antrieb_rechts_strecke;
	float stuhl_rotation;
	float fahrgestell_rotation;
	float lenkung_vorne_rotation;
	float lenkung_hinten_rotation;
	float arm_vorne_rotation;
	float arm_hinten_rotation;
	float stellglied_vorne_kraft;
	float stellglied_hinten_kraft;
	float pos_x;
	float pos_y;
	float pos_z;	
	float rot_x;
	float rot_y;
	float rot_z;
};

class Sequenzregler{	
	public:
		int ausgewaehlter_simulationsdatensatz = 0;
		int ausgewaehlter_regeldatensatz = 0;
		struct zustandsgroessen regelgroessen;
		std::vector<zustandsgroessen> fuehrungsgroessen_datensatz;
		
		bool regeldifferenzen_ausgeregelt();
	private:
		bool regeldifferenz_winkel_ausgeregelt(float fuehrungsgroesse, float regelgroesse, float maximale_regelabweichung);
		bool regeldifferenz_ausgeregelt(float fuehrungsgroesse, float regelgroesse, float maximale_regelabweichung);
		float berechne_winkel_regelabweichung(float fuehrungsgroesse, float regelgroesse);
};

#endif