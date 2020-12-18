#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>

#include "matrix.hpp"

Vektor Matrix::mat_vek_mult(const Matrix &A, const Vektor &v){
	Vektor ergebnis;
	ergebnis.x = A.mat[0][0] * v.x + A.mat[0][1] * v.y + A.mat[0][2] * v.z;
	ergebnis.y = A.mat[1][0] * v.x + A.mat[1][1] * v.y + A.mat[1][2] * v.z;
	ergebnis.z = A.mat[2][0] * v.x + A.mat[2][1] * v.y + A.mat[2][2] * v.z;
	return ergebnis;
}		


Matrix Matrix::rotation_mat(const Vektor &achse, const float &theta){	// Euler-Rodrigues formula
	/*
    Return the rotation matrix associated with counterclockwise rotation about
    the given axis by theta radians.
	*/
	
	Vektor v = achse;
	v.normieren();
    
	float a, b, c, d;	
	
	a = std::cos(theta / 2.0);
	v.skalieren(-std::sin(theta / 2.0));
	
	b = v.x;
	c = v.y;
	d = v.z;
	
	float mat[3][3] = {  { a*a + b*b - c*c - d*d , 2 * (b*c + a*d)       , 2 * (b*d - a*c)       },
					     { 2 * (b*c - a*d)       , a*a + c*c - b*b - d*d , 2 * (c*d + a*b)       },
					     { 2 * (b*d + a*c)       , 2 * (c*d - a*b)       , a*a + d*d - b*b - c*c }  };
						
	Matrix rot_mat(&mat[0][0]);
	
	return rot_mat;
}

/*
	a = math.cos(theta / 2.0)
    b, c, d = -axis * math.sin(theta / 2.0)
    aa, bb, cc, dd = a * a, b * b, c * c, d * d
    bc, ad, ac, ab, bd, cd = b * c, a * d, a * c, a * b, b * d, c * d
    return np.array([[aa + bb - cc - dd, 2 * (bc + ad), 2 * (bd - ac)],
                     [2 * (bc - ad), aa + cc - bb - dd, 2 * (cd + ab)],
                     [2 * (bd + ac), 2 * (cd - ab), aa + dd - bb - cc]])
*/

Matrix Matrix::nullmatrix(){
	Matrix matrix;
	
	matrix.mat[0][0] = 0.0;
	matrix.mat[0][1] = 0.0;
	matrix.mat[0][2] = 0.0;
	matrix.mat[1][0] = 0.0;
	matrix.mat[1][1] = 0.0;
	matrix.mat[1][2] = 0.0;
	matrix.mat[2][0] = 0.0;
	matrix.mat[2][1] = 0.0;
	matrix.mat[2][2] = 0.0;
	
	return matrix;
}

bool Matrix::ist_nullmatrix(Matrix matrix){
	
	if(matrix.mat[0][0] != 0.0) return false;
	if(matrix.mat[0][1] != 0.0) return false;
	if(matrix.mat[0][2] != 0.0) return false;
	if(matrix.mat[1][0] != 0.0) return false;
	if(matrix.mat[1][1] != 0.0) return false;
	if(matrix.mat[1][2] != 0.0) return false;
	if(matrix.mat[2][0] != 0.0) return false;
	if(matrix.mat[2][1] != 0.0) return false;
	if(matrix.mat[2][2] != 0.0) return false;
	
	return true;
}

void Matrix::ausgeben(){
	printf("Matrix:\n");
	printf("%8.4f %8.4f %8.4f\n", this->mat[0][0], this->mat[0][1], this->mat[0][2]);
	printf("%8.4f %8.4f %8.4f\n", this->mat[1][0], this->mat[1][1], this->mat[1][2]);
	printf("%8.4f %8.4f %8.4f\n", this->mat[2][0], this->mat[2][1], this->mat[2][2]);
}
/*
Matrix Treppensteiger::berechne_rotationsmatrix(double phi, double theta, double psi){
	Matrix ergebnis;
	
	ergebnis.mat[0][0] = cos(theta)*cos(psi);
	ergebnis.mat[0][1] = sin(phi)*sin(theta)*cos(psi) - cos(phi)*sin(psi);
	ergebnis.mat[0][2] = cos(phi)*sin(theta)*cos(psi) + sin(phi)*sin(psi);
	ergebnis.mat[1][0] = cos(theta)*sin(psi);
	ergebnis.mat[1][1] = sin(phi)*sin(theta)*sin(psi) + cos(phi)*cos(psi);
	ergebnis.mat[1][2] = cos(phi)*sin(theta)*sin(phi) - sin(phi)*cos(psi);
	ergebnis.mat[2][0] = -sin(theta);
	ergebnis.mat[2][1] = sin(phi)*cos(theta);
	ergebnis.mat[2][2] = cos(phi)*cos(theta);
	
	return ergebnis;
}*/

/*
pi			M_PI		3.14159265358979323846
pi/2		M_PI_2		1.57079632679489661923
pi/4		M_PI_4		0.785398163397448309616
1/pi		M_1_PI		0.318309886183790671538
2/pi		M_2_PI		0.636619772367581343076
2/sqrt(pi)	M_2_SQRTPI	1.12837916709551257390
sqrt(2)		M_SQRT2		1.41421356237309504880
1/sqrt(2)	M_SQRT1_2	0.707106781186547524401
e			M_E			2.71828182845904523536
log_2(e)	M_LOG2E		1.44269504088896340736
log_10(e)	M_LOG10E	0.434294481903251827651
log_e(2)	M_LN2		0.693147180559945309417
log_e(10)	M_LN10		2.30258509299404568402
*/