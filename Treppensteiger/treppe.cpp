#include "treppe.hpp"

void Treppe::erstelle_test_treppe(){
	int anzahl_stufen = 5;
	float stufenhoehe = 0.173;
	float stufentiefe = 0.3;
	float x, y, z;
	
	Vektor position, richtung;
	
	struct Stufe stufe;
	
	stufen.resize(anzahl_stufen);
	
	for(int i=0; i<anzahl_stufen; i++){
		x = 1.0 + stufentiefe*i;
		y = 0.0;
		z = 0.0 + stufenhoehe*(i+1);		
		
		position = Vektor(x, y, z);
		richtung = Vektor(-1.0, 0.0, 0.0);
		
		stufe.hoehe = stufenhoehe;
		stufe.breite = 1;
		stufe.position = position;
		stufe.richtung = richtung;
		
		stufen[i] = stufe;
	}
}