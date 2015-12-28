/*
 * Util.h
 *
 *  Created on: Dec 15, 2008
 *      Author: tim
 */

#ifndef UTIL_H_
#define UTIL_H_

class Util {
public:

	// "infinity"
	static const float inf = 1e10;
	// pi
	static const float PI = 3.14159;
	// returns dot product of two 3d vectors
	static float dot(float a[], float b[]);
	// normalizes a 3d vector
	static void normalize(float a[]);
	// squares a float
	static inline float sq(float f){ return f * f;}
	// rotates a point about a vector using quaternion rotation
	static void doRotation(float point [], float angleRadians, float vector []);
	// returns the cross product of 2 3d vectors
	static void cross(float result [], float a[], float b[]);
	// returns the magnitude of a cross between two vectors or something
	static float determinant(float a[], float b[]);
	// fills result with the difference between two points (ie. the vector from b to a)
	static void difference(float result [], float a[], float b[]);
	// returns the min of two floats
	static inline float min(float f1, float f2) { return (f1 < f2) ? f1 : f2; }
	// returns the distance between two points
	static float dist(float p1 [], float p2 []);
	// returns the magnitude of a 3d vector
	static float mag(float v[]);
	// applies the current OpenGL modelview transformation matrix
	// to a point.  pretty neat!
	static void transform(float p[]);
};

#endif /* UTIL_H_ */
