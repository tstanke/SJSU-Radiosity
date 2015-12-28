/*
 * Rectangle.h
 *
 *  Created on: Dec 23, 2008
 *      Author: tim
 */

#ifndef RECTANGLE_H_
#define RECTANGLE_H_

class Rectangle {
public:
	Rectangle(float llx, float lly, float urx, float ury, float color []);
	void rotate(float v [] , float theta);
	void translate(float d []);
	void recalcNormal();
	void draw();
	void calcNextColor(Rectangle **rectangles, int r, float *f, int idx, float divisor);
	void takeNextColor();
	bool liesBetween(Rectangle *a, Rectangle *b);

	// next radiosity values
	float bNext[3];
	// current radiosity values
	float b[3];
	// emmissivity
	float e[3];
	// reflectivity
	float r[3];
	// center of the rectangle
	float center[3];
	// positions of the vertices
	float pos [4][3];
	// normal vector
	float normal[3];
	// area of this rectangle
	float area;
};

#endif /* RECTANGLE_H_ */
