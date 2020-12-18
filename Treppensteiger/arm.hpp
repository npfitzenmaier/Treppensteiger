#ifndef arm_H
#define arm_H

#include "lenkung.hpp"
#include "matrix.hpp"
#include "vektor.hpp"
#include "stellglied.hpp"

class Arm{
	public:
		Vektor mittel_achse;
		
		float drehwinkel;
		
		static const float abstand_mittelachse_angriffspunkt_fahrgestell_stellglied;
		static const float abstand_mittelachse_angriffspunkt_arm_stellglied;
		static const float winkel_offset;
		static const float gewicht;
		
		Stellglied stellglied;
		
		Lenkung lenkung;
		
		Arm(){}
		Arm(Vektor &mittel_achse, Lenkung &lenkung, Stellglied &stellglied)
			: mittel_achse(mittel_achse)
			,lenkung(lenkung)
			, stellglied(stellglied){
			
			drehwinkel = 0.0;
		}
		
		Vektor rad_pos(int rad_nummer, Vektor ts_rpos);
		void rotieren(Matrix &mat_drehung);
		void stellen(float theta);
		void berechne_rotation();
		
	private:
};

#endif