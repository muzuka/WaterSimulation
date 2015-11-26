
/*
 *	Button class implementation
 *
 */

#include "Button.h"

Button::Button(Simulation s, Vector bl, Vector tr) {
	buttonChange = s;
	bottomLeft = bl;
	topRight = tr;
	width = fabs(bl.getX() - tr.getX());
	height = fabs(bl.getY() - tr.getY());
}

void Button::render() {
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(bottomLeft.getX(), bottomLeft.getY(), bottomLeft.getZ());
	glVertex3f(bottomLeft.getX(), bottomLeft.getY() + height, bottomLeft.getZ());
	glVertex3f(bottomLeft.getX() + width, bottomLeft.getY(), bottomLeft.getZ());
	glVertex3f(topRight.getX(), topRight.getY(), topRight.getZ());
	glEnd();
}