/*
 * Util.cpp
 *
 *  Created on: Dec 15, 2008
 *      Author: tim
 */

#include "Util.h"
#include <math.h>
#include <iostream>
#include "glut.h"

float Util::dot(float a[], float b[])
{
//	std::cout<<"Dot product: ";
	float res = 0;
	for(int i = 0; i < 3; i++)
	{
		res += a[i] * b[i];
//		std::cout<<a[i]<<" * "<<b[i]<<" + ";
	}
//	std::cout<<"= "<<res<<"\n";
	return res;
}

void Util::normalize(float a[])
{
	float mag = 0;
	for(int i = 0; i < 3; i++)
		mag += a[i] * a[i];
	mag = sqrt(mag);
	for(int i = 0; i < 3; i++)
		a[i] /= mag;
}

void Util::cross(float result [], float a [], float b [])
{
	result[0] = a[1] * b[2] - b[1] * a[2];
	result[1] = -(a[0] * b[2] - b[0] * a[2]);
	result[2] = a[0] * b[1] - a[1] * b[0];
}

float Util::determinant(float a[], float b[])
{
	float result[3];
	cross(result, a, b);
	float res = 0;
	for(int i = 0; i < 3; i++)
		res += result[i];
	return res;
}


void Util::difference(float result [], float a[], float b[])
{
	for(int i = 0; i < 3; i++)
		result[i] = a[i] - b[i];
}


void Util::doRotation(float p [], float angleRadians, float vector[])
{

	float s = cos(angleRadians / 2);
	float v[3];
	for(int i = 0; i < 3; i++)
		v[i] = vector[i] * sin(angleRadians / 2);

	float a = s;
	float b = v[0];
	float c = v[1];
	float d = v[2];

	float t2 =   a*b;
	float t3 =   a*c;
	float t4 =   a*d;
	float t5 =  -b*b;
	float t6 =   b*c;
	float t7 =   b*d;
	float t8 =  -c*c;
	float t9 =   c*d;
	float t10 = -d*d;
	float v1new = 2*( (t8 + t10)*p[0] + (t6 -  t4)*p[1] + (t3 + t7)*p[2] ) + p[0];
	float v2new = 2*( (t4 +  t6)*p[0] + (t5 + t10)*p[1] + (t9 - t2)*p[2] ) + p[1];
	float v3new = 2*( (t7 -  t3)*p[0] + (t2 +  t9)*p[1] + (t5 + t8)*p[2] ) + p[2];

	p[0] = v1new;
	p[1] = v2new;
	p[2] = v3new;
}

float Util::dist(float p1[], float p2[])
{
	float res = 0;
	for(int i = 0; i < 3; i++)
		res += sq(p1[i] - p2[i]);
	return sqrt(res);
}

float Util::mag(float p[])
{
	float res = 0;
	for(int i = 0; i < 3; i++)
		res += sq(p[i]);
	return sqrt(res);
}

void Util::transform(float p[3])
{
	float mat [16];
	glGetFloatv(GL_MODELVIEW_MATRIX, mat);

	float p2[4];
	for(int i = 0; i < 3; i++)
		p2[i] = p[i];
	p2[3] = 1;
	float res[] = {0,0,0,0};

	for(int i = 0; i < 16; i++)
	{
			res[i % 4] += mat[i] * p2[i / 4];
	}

	for(int i = 0; i < 3; i++)
		p[i] = res[i] / res[3];
}
