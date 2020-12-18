#define _USE_MATH_DEFINES
#include <cmath>

#include "arm.hpp"

const float Arm::abstand_mittelachse_angriffspunkt_fahrgestell_stellglied	= 446.5;		// [mm]
const float Arm::abstand_mittelachse_angriffspunkt_arm_stellglied			= 223.4;		// [mm]
const float Arm::winkel_offset												= 39.9;			// [Grad]
const float Arm::gewicht	= 14.0;		// [kg]

Vektor Arm::rad_pos(int rad_nummer, Vektor ts_pos){
	return ts_pos + lenkung.rad_pos(rad_nummer);
}

void Arm::stellen(float theta){	
	Matrix mat_drehung = Matrix::rotation_mat(mittel_achse, theta);
	lenkung.rotieren(mat_drehung);
	
	drehwinkel += theta;
}

void Arm::rotieren(Matrix &mat_drehung){
	lenkung.rotieren(mat_drehung);
}

void Arm::berechne_rotation(){	
	// Kosinussatz
	// c^2 = a^2 + b^2 -2*a*b*cos(gamma)
	// https://de.wikipedia.org/wiki/Kosinussatz#/media/Datei:Triangle_-_angles,_vertices,_sides.svg
	// cos(gamma) = (a^2 + b^2 - c^2) / (2*a*b)
	// gamma = arccos((a^2 + b^2 - c^2) / (2*a*b))
	// gamma = [0, pi]	
	
	float a = abstand_mittelachse_angriffspunkt_fahrgestell_stellglied;
	float b = abstand_mittelachse_angriffspunkt_arm_stellglied;
	float c = stellglied.laenge;
	
	float gamma = std::acos((a*a + b*b - c*c) / (2.0*a*b));
	drehwinkel = (winkel_offset / 180.0) * M_PI - gamma;
	
	//stellglied.motorregler_winkel.regelgroesse = drehwinkel;
}