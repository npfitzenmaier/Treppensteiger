#ifndef vektor_H
#define vektor_H

class Vektor{
	public:
		float x;
		float y;
		float z;
		
		Vektor() {};
		Vektor(float x, float y, float z): x(x), y(y), z(z) {}
		//~Vektor();
		
		Vektor operator+(const Vektor &v){
			Vektor ergebnis;
			ergebnis.x = this->x + v.x;
			ergebnis.y = this->y + v.y;
			ergebnis.z = this->z + v.z;
			return ergebnis;
		}
		Vektor operator-(const Vektor &v){
			Vektor ergebnis;
			ergebnis.x = this->x - v.x;
			ergebnis.y = this->y - v.y;
			ergebnis.z = this->z - v.z;
			return ergebnis;
		}
		Vektor operator*(const Vektor &v){
			Vektor ergebnis;
			ergebnis.x = this->x * v.x;
			ergebnis.y = this->y * v.y;
			ergebnis.z = this->z * v.z;
			return ergebnis;
		}
		Vektor operator/(const Vektor &v){
			Vektor ergebnis;
			ergebnis.x = this->x + v.x;
			ergebnis.y = this->y + v.y;
			ergebnis.z = this->z + v.z;
			return ergebnis;
		}
		
		void normieren();
		void skalieren(float faktor);
		float betrag();
		void ausgeben();
		
		static float skalarprodukt(const Vektor &v1, const Vektor &v2);		
		static Vektor kreuzprodukt(const Vektor &v1, const Vektor &v2);
		static float winkel(Vektor &v1, Vektor &v2);
		
		 // abstand_ursprung ist der Abstand von der Ebene zum Ursprung
		 // abstand_ursprung >= 0, wenn der Normalenvektor in den Halbraum mit dem Ursprung zeigt
		 // ansonsten abstand_urpsrung < 0
		static float abstand_punkt_ebene(const Vektor punkt, const Vektor normalenvektor, const float abstand_urpsrung);
};
#endif