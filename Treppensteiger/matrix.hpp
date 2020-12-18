#ifndef mathematik_H
#define mathematik_H

#include <cstring>

#include "vektor.hpp"

class Matrix{
	public:
		float mat[3][3];	// [Zeilen][Spalten]
		
		Matrix(){};
		Matrix(float* pmat){
			memcpy(mat, pmat, sizeof(mat));
		}
		
		Matrix operator*(const Matrix &m){
			Matrix ergebnis;
			ergebnis.mat[0][0] = this->mat[0][0] * m.mat[0][0] + this->mat[0][1] * m.mat[1][0] + this->mat[0][2] * m.mat[2][0];
			ergebnis.mat[0][1] = this->mat[0][0] * m.mat[0][1] + this->mat[0][1] * m.mat[1][1] + this->mat[0][2] * m.mat[2][1];
			ergebnis.mat[0][2] = this->mat[0][0] * m.mat[0][2] + this->mat[0][1] * m.mat[1][2] + this->mat[0][2] * m.mat[2][2];
			ergebnis.mat[1][0] = this->mat[1][0] * m.mat[0][0] + this->mat[1][1] * m.mat[1][0] + this->mat[1][2] * m.mat[2][0];
			ergebnis.mat[1][1] = this->mat[1][0] * m.mat[0][1] + this->mat[1][1] * m.mat[1][1] + this->mat[1][2] * m.mat[2][1];
			ergebnis.mat[1][2] = this->mat[1][0] * m.mat[0][2] + this->mat[1][1] * m.mat[1][2] + this->mat[1][2] * m.mat[2][2];
			ergebnis.mat[2][0] = this->mat[2][0] * m.mat[0][0] + this->mat[2][1] * m.mat[1][0] + this->mat[2][2] * m.mat[2][0];
			ergebnis.mat[2][1] = this->mat[2][0] * m.mat[0][1] + this->mat[2][1] * m.mat[1][1] + this->mat[2][2] * m.mat[2][1];
			ergebnis.mat[2][2] = this->mat[2][0] * m.mat[0][2] + this->mat[2][1] * m.mat[1][2] + this->mat[2][2] * m.mat[2][2];
			
			return ergebnis;
		}
		
		void ausgeben();
			
		static Vektor mat_vek_mult(const Matrix &A, const Vektor &v);	
		static Matrix rotation_mat(const Vektor &achse, const float &theta);
		static Matrix nullmatrix();
		static bool ist_nullmatrix(Matrix matrix);
};

#endif