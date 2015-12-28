#include "glut.h"
#include <math.h>
#include "Rectangle.h"
#include "Util.h"
#include "iostream"

// aspect ratio of the window
float aspect;

// width and height of the screen
int w, h;

// parameter related to size for constructing
// the scene
int d = 100, d2 = d / 2;

// contains all "patches"
Rectangle *rectangles[1 << 20];

// contains the larger panels that the patches are
// derived from
Rectangle *macroscale[1 << 10];

// union of rectangles and macroscale
Rectangle *all[1 << 21];

// contains unique identifiers to represent
// many-to-one mapping of patches
// to larger panels
int container[1 << 20];

// counters for what is contained in the above arrays
int r = 0, m = 0, a = 0;

// coefficient matrix for the radiosity algorithm
float *f;

/**
 * Adds a unit panel at the origin with, as well as
 * an nxn grid of patches that match it.  OpenGL
 * transformations must be applied before this call
 * to position the panels.
 */
void addUnitPanel(int n, float color[]) {
	float interval = 2. / n;

	macroscale[m++] = new Rectangle(-1, -1, 1, 1, color);
	all[a++] = macroscale[m - 1];
	for (int j = 0; j < 4; j++)
		Util::transform(macroscale[m - 1]->pos[j]);
	macroscale[m - 1]->recalcNormal();

	for (int x = 0; x < n; x++) {
		for (int y = 0; y < n; y++) {
			container[r] = m - 1;
			rectangles[r++]
					= new Rectangle(x * interval - 1, y * interval - 1, (x + 1)
							* interval - 1, (y + 1) * interval - 1, color);
			all[a++] = rectangles[r - 1];
			for (int j = 0; j < 4; j++) {
				Util::transform(rectangles[r - 1]->pos[j]);
			}
			rectangles[r - 1]->recalcNormal();
		}
	}
}

// calculates how well patch i sees patch j
float calcFactor(int i, int j) {
	float jtoi[3];
	float itoj[3];
	Util::difference(jtoi, rectangles[i]->center, rectangles[j]->center);
	Util::difference(itoj, rectangles[j]->center, rectangles[i]->center);

	float dist = Util::mag(jtoi);

	Util::normalize(itoj);
	Util::normalize(jtoi);

	// test to see if the patches cannot see each other at all
	// due to facing away from each other.
	float d1 = Util::dot(itoj, rectangles[i]->normal);
	if (d1 <= 0)
		return 0;
	float d2 = Util::dot(jtoi, rectangles[j]->normal);
	if (d2 <= 0)
		return 0;

	// check occlusion for shadows
	for (int k = 0; k < m; k++)
		if (container[i] == k || container[j] == k)
			continue;
		else if (macroscale[k]->liesBetween(rectangles[i], rectangles[j]))
			return 0;

	// this result attempts to approximate the proportion of the visible
	// hemisphere around patch i that is occupied by patch j.
	// 2 * pi * dist^2 is the surface area of half a hemisphere.
	// rectangles[j]->area divided by that amount would be the coefficient
	// if patch j was directly facing patch i.  To account for j facing
	// some other direction a dot product similar to the one used
	// in diffuse lighting is factored in.
	float res = d2 * rectangles[j]->area  / (2 * Util::PI * Util::sq(dist));


	return res;
}

void constructScene() {
	int n = 10;
	float larger = 50;

	float white[] = { .1, .1, .1 };
	float red[] = { .1, 0, 0 };
	float green[] = { 0, .1, 0 };

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// back wall
	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	glTranslatef(0, 0, -larger);
	glScalef(larger, larger, larger);
	addUnitPanel(n, white);
	glPopMatrix();

	// near wall: there is a wall on the face
	// of the cube closest to the camera that
	// is not visible from outside but helps reflect
	// some white light onto the boxes.
	glPushMatrix();
	glRotatef(-90, 0, 1, 0);
	glTranslatef(0, 0, -larger);
	glScalef(larger, larger, larger);
	addUnitPanel(n, white);
	glPopMatrix();

	// floor
	glPushMatrix();

	glTranslatef(0, 0, -larger);
	glScalef(larger, larger, larger);

	addUnitPanel(n, white);
	glPopMatrix();

	// ceiling
	glPushMatrix();
	glRotatef(180, 0, 1, 0);
	glTranslatef(0, 0, -larger);
	glScalef(larger, larger, larger);
	addUnitPanel(n, white);
	glScalef(.2, .2, 1);
	glTranslatef(0, 0, .001);
	int oldr = r;
	addUnitPanel(n, white);

	// light
	for (int i = oldr; i < r; i++)
		for (int j = 0; j < 3; j++) {
			rectangles[i]->e[j] = 1;
		}

	glPopMatrix();

	// red left wall
	glPushMatrix();
	glRotatef(-90, 1, 0, 0);
	glTranslatef(0, 0, -larger);
	glScalef(larger, larger, larger);
	addUnitPanel(n, red);
	glPopMatrix();

	// green right wall
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 0, -larger);
	glScalef(larger, larger, larger);
	addUnitPanel(n, green);
	glPopMatrix();

	// small cube
	float smallr = larger / 3;
	glPushMatrix();
	glTranslatef(smallr * 1.1, smallr, -larger + smallr);
	glRotatef(-20, 0, 0, 1);
	glScalef(smallr, smallr, smallr);
	for (int i = 0; i < 4; i++) {
		glPushMatrix();
		glRotatef(i * 90, 0, 0, 1);
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, 1);
		addUnitPanel(n, white);
		glPopMatrix();
	}
	glTranslatef(0, 0, 1);
	addUnitPanel(n, white);
	glPopMatrix();

	// large cube
	float medh = smallr * 2;
	glPushMatrix();
	glTranslatef(-smallr * 1.1, -smallr, -larger + medh);
	glRotatef(20, 0, 0, 1);
	glScalef(smallr, smallr, medh);
	for (int i = 0; i < 4; i++) {
		glPushMatrix();
		glRotatef(i * 90, 0, 0, 1);
		glRotatef(90, 0, 1, 0);
		glTranslatef(0, 0, 1);
		addUnitPanel(n, white);
		glPopMatrix();
	}
	glTranslatef(0, 0, 1);
	addUnitPanel(n, white);
	glPopMatrix();

	glPopMatrix();

	f = new float[r * r];
	float maxfactor = 0;
	for (int i = 0; i < r; i++) {
		float ff = 0;
		for (int j = 0; j < r; j++) {
			float factor = calcFactor(i, j);
			f[i * r + j] = factor;
			ff += factor;
		}
		maxfactor = std::max(maxfactor, ff);
	}

	std::cout<<maxfactor<<"\n";

//	for(int i = 0; i < r * r; i++)
//		f[i] /= maxfactor;

	for(int i = 0; i < r; i++)
		for(int j = 0; j < 3; j++)
			rectangles[i]->b[j] = rectangles[i]->e[j];
}

/**
 * Executes one iteration of the algorithm: the "next" radiosity
 * values for each patch are computed and then applied.
 */
void iterate() {

	for (int i = 0; i < r; i++)
		rectangles[i]->calcNextColor(rectangles, r, f, i, 1);
	for (int i = 0; i < r; i++)
		rectangles[i]->takeNextColor();

//	glPushMatrix();
//	glLoadIdentity();
//	glRotatef(45, 0, 0, 1);
//	for (int i = 0; i < a; i++)
//		for (int j = 0; j < 4; j++)
//			Util::transform(all[i]->pos[j]);
//	glPopMatrix();
}

void draw() {
	for (int i = 0; i < r; i++)
		rectangles[i]->draw();
}

void display(void) {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//	glOrtho(-100, 100, -100, 100, 0, 1000);
	gluPerspective(60, aspect, 1, 1000);
	gluLookAt(140, 0, 0, 0, 0, 0, 0, 0, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	draw();

	glutSwapBuffers();

	iterate();
	//	glutPostRedisplay();
}

void init(void) {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// remove rear-facing polygons
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(0.0, 0.0, 0.0, 0.0);

	constructScene();
}

void reshape(int width, int height) {
	w = width;
	h = height;

	glViewport(0, 0, w, h);

	aspect = (float) w / (float) h;
}

void keyboard(unsigned char key, int x, int y) {
	if (key == 'n') {
		glutPostRedisplay();
	}
}

int main(int argc, char ** argv) {

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(512, 512);
	glutCreateWindow(*argv);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);

	init();

	glutMainLoop();

	return 0;
}
