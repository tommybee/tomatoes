/*************************************************************************

                         "I Have No Tomatoes"
                  Copyright (c) 2004, Mika Halttunen

 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute
 it freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must
    not claim that you wrote the original software. If you use this
    software in a product, an acknowledgment in the product documentation
    would be appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must
    not be misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution.


 Mika Halttunen <lsoft@mbnet.fi>

*************************************************************************/

#ifndef VECT_H
#define VECT_H

#include <math.h>


// Basic vector class
class VECT {
public:

	// Components
	float x, y, z;

	// Constructors
	VECT() {}
	VECT(float f) {
		x = y = z = f;
	}
	VECT(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	}
	VECT(const float f[3]) {
		x = f[0];
		y = f[1];
		z = f[2];
	}


	// Operators
	VECT& operator += (const VECT &v) {
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}
	VECT& operator -= (const VECT &v) {
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}
	VECT& operator *= (const VECT &v) {
		x *= v.x;
		y *= v.y;
		z *= v.z;

		return *this;
	}
	VECT& operator /= (const VECT &v) {
		x /= v.x;
		y /= v.y;
		z /= v.z;

		return *this;
	}
	VECT& operator *= (float s) {
		x *= s;
		y *= s;
		z *= s;

		return *this;
	}
	VECT& operator /= (float s) {
		x /= s;
		y /= s;
		z /= s;

		return *this;
	}
	friend VECT operator + (const VECT &v) {
		return v;
	}
	friend VECT operator - (const VECT &v) {
		return VECT(-v.x, -v.y, -v.z);
	}
	friend VECT operator + (const VECT &v1, const VECT &v2) {
		return VECT(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	}
	friend VECT operator - (const VECT &v1, const VECT &v2) {
		return VECT(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
	}
	friend VECT operator * (const VECT &v, float s) {
		return VECT(v.x * s, v.y * s, v.z * s);
	}
	friend VECT operator * (float s, const VECT &v) {
		return VECT(v.x * s, v.y * s, v.z * s);
	}
	friend VECT operator / (const VECT &v, float s) {
		return VECT(v.x / s, v.y / s, v.z / s);
	}
};

// Basic vector operations (inlined)

// Dot product
inline float dot(VECT v1, VECT v2) {

	return(v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

// Cross product
inline VECT cross(VECT v1, VECT v2) {

	VECT result;

	result.x = (v1.y * v2.z) - (v2.y * v1.z);
	result.y = (v1.z * v2.x) - (v2.z * v1.x);
	result.z = (v1.x * v2.y) - (v2.x * v1.y);

	return (result);
}

// Normalize a vector
inline void normalize(VECT &v) {

	float len = (float)sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	v.x /= len;
	v.y /= len;
	v.z /= len;
}


// Return the length of a vector
inline double vector_length(VECT v) {

	return (sqrt(v.x * v.x + v.y * v.y + v.z * v.z));
}

// Set the length of a vector
inline void set_vector_length(VECT &v, float l) {

	float len = (float)sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	v.x *= l/len;
	v.y *= l/len;
	v.z *= l/len;
}

// Return true if the vector is a zero vector
inline bool is_zero_vector(VECT v) {

	if(v.x == 0 && v.y == 0 && v.z == 0)
		return true;
	else
		return false;
}



#endif


