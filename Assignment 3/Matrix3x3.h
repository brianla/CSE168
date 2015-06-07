#ifndef CSE168_MATRIX3X3_H_INCLUDED
#define CSE168_MATRIX3X3_H_INCLUDED

#include <math.h>
#include <float.h>
#include <iostream>

#ifdef WIN32
#pragma warning(disable:4305) // disable useless warnings
#pragma warning(disable:4244)
#endif

#include "Vector3.h"

class Matrix3x3
{

public:
	float m11, m12, m13,
		m21, m22, m23,
		m31, m32, m33;

	// Implements a 3x3 matrix: m_ij - row-i and column-j entry

public:

	Matrix3x3();
	Matrix3x3(const Vector3&, const Vector3&,
		const Vector3&); // sets by columns!
	Matrix3x3(float, float, float,
		float, float, float,
		float, float, float); // sets by columns

	inline void setIdentity();     // set to the identity map
	inline void set(const Matrix3x3&); // set to the matrix.
	inline void set(const Vector3&, const Vector3&,
		const Vector3&);
	inline void set(float, float, float,
		float, float, float,
		float, float, float);
	inline void setColumn1(float, float, float);
	inline void setColumn2(float, float, float);
	inline void setColumn3(float, float, float);
	inline void setColumn1(const Vector3&);
	inline void setColumn2(const Vector3&);
	inline void setColumn3(const Vector3&);
	inline Vector3 column1() const;
	inline Vector3 column2() const;
	inline Vector3 column3() const;
	//inline Matrix3x3& invert();

	inline void transpose();                    // Transposes it.
	inline Matrix3x3& operator+=(const Matrix3x3&);
	inline Matrix3x3& operator-=(const Matrix3x3&);
	inline Matrix3x3& operator*=(float);
	inline Matrix3x3& operator/=(float);
	inline Matrix3x3& operator*=(const Matrix3x3&);    // Matrix product
};

#endif // CSE168_MATRIX3X3_H_INCLUDED