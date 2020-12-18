#ifndef treppe_H
#define treppe_H

#include <vector>

#include "vektor.hpp"

struct Stufe{
	float hoehe;
	float breite;
	Vektor position;
	Vektor richtung;
};

class Treppe{
	public:
		std::vector<Stufe> stufen;
		float bodenhoehe = 0.0;
		
		void erstelle_test_treppe();
	private:
};

#endif