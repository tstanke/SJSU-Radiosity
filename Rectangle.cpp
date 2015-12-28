/*
 * Rectangle.cpp
 *
 *  Created on: Dec 23, 2008
 *      Author: tim
 */

#include "Rectangle.h"
#include "glut.h"
#include <math.h>
#include "Util.h"

Rectangle::Rectangle(float llx, float lly, float urx, float ury, float c[]) {
	pos[0][0] = llx;
	pos[0][1] = lly;
	pos[1][0] = urx;
	pos[1][1] = lly;
	pos[2][0] = urx;
	pos[2][1] = ury;
	pos[3][0] = llx;
	pos[3][1] = ury;

	for (int i = 0; i < 3; i++)
	{
		r[i] = c[i];
		b[i] = 0;
	}

	recalcNormal();
}

void Rectangle::recalcNormal() {

	float v1[3];
	float v2[3];
	Util::difference(v1, pos[1], pos[0]);
	Util::difference(v2, pos[2], pos[1]);

	Util::cross(normal, v1, v2);
	area = Util::mag(normal);
	Util::normalize(normal);

	for(int i = 0; i < 3; i++)
		center[i] = 0;
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 3; j++)
			center[j] += pos[i][j];
	for(int j = 0; j < 3; j++)
		center[j] /= 4.;
}

void Rectangle::draw() {

	float color[3];
	for(int i = 0; i < 3; i++)
		color[i] = e[i] + b[i];
	glColor3fv(color);

	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++) {
		glVertex3fv(pos[i]);
	}
	glEnd();

//	glColor3f(0, 0, 1);
//	glBegin(GL_LINES);
//	float n[3];
//	for (int i = 0; i < 3; i++)
//		n[i] = center[i];
//	glVertex3fv(n);
//	for (int i = 0; i < 3; i++)
//		n[i] += normal[i] * area / 10;
//	glVertex3fv(n);
//	glEnd();

}

void Rectangle::calcNextColor(Rectangle **rectangles, int r, float *f, int idx, float divisor) {

	for(int i = 0; i < 3; i++)
		bNext[i] = 0;

	for (int i = 0; i < r; i++) {
		for(int j = 0; j < 3; j++)
		{
			float val = this->r[j] * rectangles[i]->b[j] * f[i * r + idx];
			bNext[j] += val;
		}
	}
	for(int i = 0; i < 3; i++)
		e[i] = 0;
}

void Rectangle::takeNextColor()
{
	for(int i = 0; i < 3; i++)
	{
		b[i] += bNext[i];
		if(b[i] > 1)
			b[i] = 1;
	}
}

bool Rectangle::liesBetween(Rectangle *a, Rectangle *b)
{
	int n = 4;
	float aToB;
	float direction[3];
	Util::difference(direction, b->center, a->center);
	aToB = Util::mag(direction);
	Util::normalize(direction);

	float d = Util::dot(pos[0], normal);
	float iaDotN = Util::dot(a->center, normal);
	float dirDotN = Util::dot(direction, normal);

	if (dirDotN == 0)
		return 0;

	float dist = (d - iaDotN) / dirDotN;

	if(dist < 0 || dist > aToB)
		return 0;

	float p[3];
	for (int i = 0; i < 3; i++)
		p[i] = a->center[i] + dist * direction[i];

	for (int i = 1; i <= n; i++) {
		float v1[3];
		float v2[3];
		Util::difference(v1, pos[i % n], pos[i - 1]);
		Util::difference(v2, p, pos[i % n]);
		float cross[3];
		Util::cross(cross, v1, v2);
		Util::normalize(cross);
		float cosAngle = Util::dot(cross, normal);
		if(cosAngle < 0)
			return 0;
	}

	return 1;

}
