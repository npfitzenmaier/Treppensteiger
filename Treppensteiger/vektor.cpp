#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>

#include "vektor.hpp"

void Vektor::normieren(){
	float betrag = this->betrag();
	this->x /= betrag;
	this->y /= betrag;
	this->z /= betrag;
}

void Vektor::skalieren(float faktor){
	this->x *= faktor;
	this->y *= faktor;
	this->z *= faktor;
}

float Vektor::betrag(){
	return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
}
	
float Vektor::skalarprodukt(const Vektor &v1, const Vektor &v2){
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}	
		
Vektor Vektor::kreuzprodukt(const Vektor &v1, const Vektor &v2){
	Vektor ergebnis;
	ergebnis.x = v1.y * v2.z - v1.z * v2.y;
	ergebnis.y = v1.z * v2.x - v1.x * v2.z;
	ergebnis.z = v1.x * v2.y - v1.y * v2.x;
	return ergebnis;
}

float Vektor::abstand_punkt_ebene(const Vektor punkt, const Vektor normalenvektor, const float abstand_urpsrung){
	float distanz = Vektor::skalarprodukt(normalenvektor, punkt) + abstand_urpsrung;
	return distanz;
}

float Vektor::winkel(Vektor &v1, Vektor &v2){
	return std::acos(skalarprodukt(v1, v2)/(v1.betrag() * v2.betrag()));		// [0, M_PI/2]
}

void Vektor::ausgeben(){
	printf("Vektor: (%6.3f %6.3f %6.3f )\n", this->x, this->y, this->z);
}