#include "Matrix3x3.h"

inline std::ostream &
operator<<(std::ostream& out, const Matrix3x3& m)
{
	return out << m.column1() << std::endl
		<< m.column2() << std::endl
		<< m.column3();
}

// binary operators
inline Matrix3x3 operator+(const Matrix3x3&, const Matrix3x3&);
inline Matrix3x3 operator-(const Matrix3x3&);
inline Matrix3x3 operator-(const Matrix3x3&, const Matrix3x3&);
inline Matrix3x3 operator*(const Matrix3x3&, float);
inline Matrix3x3 operator*(float, const Matrix3x3&);
inline Matrix3x3 operator/(const Matrix3x3&, float);
inline Matrix3x3 operator*(const Matrix3x3&, const Matrix3x3&);

inline Vector3 operator*(const Matrix3x3&, const Vector3&);

inline Matrix3x3::Matrix3x3() { setIdentity(); }

inline Matrix3x3::Matrix3x3(const Vector3& u, const Vector3& v,
	const Vector3& s)
{
	m11 = u.x;      // Column 1
	m21 = u.y;
	m31 = u.z;
	m12 = v.x;      // Column 2
	m22 = v.y;
	m32 = v.z;
	m13 = s.x;      // Column 3
	m23 = s.y;
	m33 = s.z;
}

inline Matrix3x3::Matrix3x3(float a11, float a12, float a13,
	float a21, float a22, float a23,
	float a31, float a32, float a33)
	// Values specified in row order!!!
{
	m11 = a11;      // Row 1
	m12 = a12;
	m13 = a13;
	m21 = a21;      // Row 2
	m22 = a22;
	m23 = a23;
	m31 = a31;      // Row 3
	m32 = a32;
	m33 = a33;
}

inline void
Matrix3x3::setIdentity()
{
	m11 = m22 = m33 = 1.0;
	m12 = m13 = m21 = m23 = m31 = m32 = 0.0;
}

inline void
Matrix3x3::set(const Vector3& u, const Vector3& v,
const Vector3& s)
{
	m11 = u.x;      // Column 1
	m21 = u.y;
	m31 = u.z;
	m12 = v.x;      // Column 2
	m22 = v.y;
	m32 = v.z;
	m13 = s.x;      // Column 3
	m23 = s.y;
	m33 = s.z;
}

inline void
Matrix3x3::set(float a11, float a12, float a13,
float a21, float a22, float a23,
float a31, float a32, float a33)
// Values specified in row order!!!
{
	m11 = a11;      // Row 1
	m12 = a12;
	m13 = a13;
	m21 = a21;      // Row 2
	m22 = a22;
	m23 = a23;
	m31 = a31;      // Row 3
	m32 = a32;
	m33 = a33;
}

inline void
Matrix3x3::set(const Matrix3x3& M) // set to the matrix.
{
	m11 = M.m11;
	m12 = M.m12;
	m13 = M.m13;
	m21 = M.m21;
	m22 = M.m22;
	m23 = M.m23;
	m31 = M.m31;
	m32 = M.m32;
	m33 = M.m33;
}

inline void
Matrix3x3::setColumn1(float x, float y, float z)
{
	m11 = x; m21 = y; m31 = z;
}

inline void
Matrix3x3::setColumn2(float x, float y, float z)
{
	m12 = x; m22 = y; m32 = z;
}

inline void
Matrix3x3::setColumn3(float x, float y, float z)
{
	m13 = x; m23 = y; m33 = z;
}

inline void
Matrix3x3::setColumn1(const Vector3& u)
{
	m11 = u.x; m21 = u.y; m31 = u.z;
}

inline void
Matrix3x3::setColumn2(const Vector3& u)
{
	m12 = u.x; m22 = u.y; m32 = u.z;
}

inline void
Matrix3x3::setColumn3(const Vector3& u)
{
	m13 = u.x; m23 = u.y; m33 = u.z;
}

Vector3
Matrix3x3::column1() const
{
	return Vector3(m11, m21, m31);
}

Vector3
Matrix3x3::column2() const
{
	return Vector3(m12, m22, m32);
}

Vector3
Matrix3x3::column3() const
{
	return Vector3(m13, m23, m33);
}

inline void
Matrix3x3::transpose()  // Transposes it.
{
	register float temp;
	temp = m12;
	m12 = m21;
	m21 = temp;
	temp = m13;
	m13 = m31;
	m31 = temp;
	temp = m23;
	m23 = m32;
	m32 = temp;
}
//
//Matrix3x3&
//Matrix3x3::invert()          // Converts into inverse.
//{
//	float Tbt34C12 = m31*m42 - m32*m41;       // 2x2 subdeterminants
//	float Tbt34C13 = m31*m43 - m33*m41;
//	float Tbt34C14 = m31*m44 - m34*m41;
//	float Tbt34C23 = m32*m43 - m33*m42;
//	float Tbt34C24 = m32*m44 - m34*m42;
//	float Tbt34C34 = m33*m44 - m34*m43;
//	float Tbt24C12 = m21*m42 - m22*m41;       // 2x2 subdeterminants
//	float Tbt24C13 = m21*m43 - m23*m41;
//	float Tbt24C14 = m21*m44 - m24*m41;
//	float Tbt24C23 = m22*m43 - m23*m42;
//	float Tbt24C24 = m22*m44 - m24*m42;
//	float Tbt24C34 = m23*m44 - m24*m43;
//	float Tbt23C12 = m21*m32 - m22*m31;       // 2x2 subdeterminants
//	float Tbt23C13 = m21*m33 - m23*m31;
//	float Tbt23C14 = m21*m34 - m24*m31;
//	float Tbt23C23 = m22*m33 - m23*m32;
//	float Tbt23C24 = m22*m34 - m24*m32;
//	float Tbt23C34 = m23*m34 - m24*m33;
//
//	float sd11 = m22*Tbt34C34 - m23*Tbt34C24 + m24*Tbt34C23;    // 3x3 subdeterminants
//	float sd12 = m21*Tbt34C34 - m23*Tbt34C14 + m24*Tbt34C13;
//	float sd13 = m21*Tbt34C24 - m22*Tbt34C14 + m24*Tbt34C12;
//	float sd14 = m21*Tbt34C23 - m22*Tbt34C13 + m23*Tbt34C12;
//	float sd21 = m12*Tbt34C34 - m13*Tbt34C24 + m14*Tbt34C23;    // 3x3 subdeterminants
//	float sd22 = m11*Tbt34C34 - m13*Tbt34C14 + m14*Tbt34C13;
//	float sd23 = m11*Tbt34C24 - m12*Tbt34C14 + m14*Tbt34C12;
//	float sd24 = m11*Tbt34C23 - m12*Tbt34C13 + m13*Tbt34C12;
//	float sd31 = m12*Tbt24C34 - m13*Tbt24C24 + m14*Tbt24C23;    // 3x3 subdeterminants
//	float sd32 = m11*Tbt24C34 - m13*Tbt24C14 + m14*Tbt24C13;
//	float sd33 = m11*Tbt24C24 - m12*Tbt24C14 + m14*Tbt24C12;
//	float sd34 = m11*Tbt24C23 - m12*Tbt24C13 + m13*Tbt24C12;
//	float sd41 = m12*Tbt23C34 - m13*Tbt23C24 + m14*Tbt23C23;    // 3x3 subdeterminants
//	float sd42 = m11*Tbt23C34 - m13*Tbt23C14 + m14*Tbt23C13;
//	float sd43 = m11*Tbt23C24 - m12*Tbt23C14 + m14*Tbt23C12;
//	float sd44 = m11*Tbt23C23 - m12*Tbt23C13 + m13*Tbt23C12;
//
//	register float detInv = 1.0 / (m11*sd11 - m12*sd12 + m13*sd13 - m14*sd14);
//
//	m11 = sd11*detInv;
//	m12 = -sd21*detInv;
//	m13 = sd31*detInv;
//	m14 = -sd41*detInv;
//	m21 = -sd12*detInv;
//	m22 = sd22*detInv;
//	m23 = -sd32*detInv;
//	m24 = sd42*detInv;
//	m31 = sd13*detInv;
//	m32 = -sd23*detInv;
//	m33 = sd33*detInv;
//	m34 = -sd43*detInv;
//	m41 = -sd14*detInv;
//	m42 = sd24*detInv;
//	m43 = -sd34*detInv;
//	m44 = sd44*detInv;
//
//	return *this;
//}

inline Matrix3x3&
Matrix3x3::operator+=(const Matrix3x3& B)
{
	m11 += B.m11;
	m12 += B.m12;
	m13 += B.m13;
	m21 += B.m21;
	m22 += B.m22;
	m23 += B.m23;
	m31 += B.m31;
	m32 += B.m32;
	m33 += B.m33;
	return *this;
}

inline Matrix3x3&
Matrix3x3::operator-=(const Matrix3x3& B)
{
	m11 -= B.m11;
	m12 -= B.m12;
	m13 -= B.m13;
	m21 -= B.m21;
	m22 -= B.m22;
	m23 -= B.m23;
	m31 -= B.m31;
	m32 -= B.m32;
	m33 -= B.m33;
	return(*this);
}

inline Matrix3x3
operator+(const Matrix3x3& A, const Matrix3x3& B)
{
	return Matrix3x3(A.m11 + B.m11, A.m21 + B.m21, A.m31 + B.m31,
		A.m12 + B.m12, A.m22 + B.m22, A.m32 + B.m32,
		A.m13 + B.m13, A.m23 + B.m23, A.m33 + B.m33);
}

inline Matrix3x3
operator-(const Matrix3x3& A)
{
	return Matrix3x3(-A.m11, -A.m21, -A.m31,
		-A.m12, -A.m22, -A.m32,
		-A.m13, -A.m23, -A.m33);
}

inline Matrix3x3
operator-(const Matrix3x3& A, const Matrix3x3& B)
{
	return Matrix3x3(A.m11 - B.m11, A.m21 - B.m21, A.m31 - B.m31,
		A.m12 - B.m12, A.m22 - B.m22, A.m32 - B.m32,
		A.m13 - B.m13, A.m23 - B.m23, A.m33 - B.m33);
}

inline Matrix3x3&
Matrix3x3::operator*=(float b)
{
	m11 *= b;
	m12 *= b;
	m13 *= b;
	m21 *= b;
	m22 *= b;
	m23 *= b;
	m31 *= b;
	m32 *= b;
	m33 *= b;
	return *this;
}

inline Matrix3x3&
Matrix3x3::operator*=(const Matrix3x3& B)    // Matrix product
{
	float t1, t2, t3;       // temporary values
	t1 = m11*B.m11 + m12*B.m21 + m13*B.m31;
	t2 = m11*B.m12 + m12*B.m22 + m13*B.m32;
	t3 = m11*B.m13 + m12*B.m23 + m13*B.m33;
	m11 = t1;
	m12 = t2;
	m13 = t3;

	t1 = m21*B.m11 + m22*B.m21 + m23*B.m31;
	t2 = m21*B.m12 + m22*B.m22 + m23*B.m32;
	t3 = m21*B.m13 + m22*B.m23 + m23*B.m33;
	m21 = t1;
	m22 = t2;
	m23 = t3;

	t1 = m31*B.m11 + m32*B.m21 + m33*B.m31;
	t2 = m31*B.m12 + m32*B.m22 + m33*B.m32;
	t3 = m31*B.m13 + m32*B.m23 + m33*B.m33;
	m31 = t1;
	m32 = t2;
	m33 = t3;

	return *this;
}

inline Matrix3x3
operator*(const Matrix3x3& A, const Matrix3x3& B) // Matrix product
{
	Matrix3x3 R;
	float t1, t2, t3;       // temporary values
	t1 = A.m11*B.m11 + A.m12*B.m21 + A.m13*B.m31;
	t2 = A.m11*B.m12 + A.m12*B.m22 + A.m13*B.m32;
	t3 = A.m11*B.m13 + A.m12*B.m23 + A.m13*B.m33;
	R.m11 = t1;
	R.m12 = t2;
	R.m13 = t3;

	t1 = A.m21*B.m11 + A.m22*B.m21 + A.m23*B.m31;
	t2 = A.m21*B.m12 + A.m22*B.m22 + A.m23*B.m32;
	t3 = A.m21*B.m13 + A.m22*B.m23 + A.m23*B.m33;
	R.m21 = t1;
	R.m22 = t2;
	R.m23 = t3;

	t1 = A.m31*B.m11 + A.m32*B.m21 + A.m33*B.m31;
	t2 = A.m31*B.m12 + A.m32*B.m22 + A.m33*B.m32;
	t3 = A.m31*B.m13 + A.m32*B.m23 + A.m33*B.m33;
	R.m31 = t1;
	R.m32 = t2;
	R.m33 = t3;

	return R;
}

inline Matrix3x3
operator*(const Matrix3x3& A, float b)
{
	return(Matrix3x3(A.m11*b, A.m21*b, A.m31*b,
		A.m12*b, A.m22*b, A.m32*b,
		A.m13*b, A.m23*b, A.m33*b));
}

inline Matrix3x3
operator*(float b, const Matrix3x3& A)
{
	return(Matrix3x3(A.m11*b, A.m21*b, A.m31*b,
		A.m12*b, A.m22*b, A.m32*b,
		A.m13*b, A.m23*b, A.m33*b));
}

inline Matrix3x3
operator/(const Matrix3x3& A, float b)
{
	register float bInv = 1.0 / b;
	return (A*bInv);
}

inline Matrix3x3&
Matrix3x3::operator/=(float b)
{
	register float bInv = 1.0 / b;
	return (*this *= bInv);
}

inline Vector3
operator*(const Matrix3x3& A, const Vector3& u)
{
	return Vector3(A.m11*u.x + A.m12*u.y + A.m13*u.z,
		A.m21*u.x + A.m22*u.y + A.m23*u.z,
		A.m31*u.x + A.m32*u.y + A.m33*u.z);
	// note that this ignores the fourth row in the matrix!
}