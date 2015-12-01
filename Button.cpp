
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

	double tempvertices[] = {
		bl.getX(), bl.getY(), bl.getZ(),
		bl.getX(), bl.getY() + height, bl.getZ(),
		bl.getX() + width, bl.getY(), bl.getZ(),
		tr.getX(), tr.getY(), tr.getZ()
	};

	memcpy(&vertices, &tempvertices, sizeof(tempvertices));

	glGenBuffers(1, &vbo);
}

void Button::render() {
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(bottomLeft.getX(), bottomLeft.getY(), bottomLeft.getZ());
	glVertex3f(bottomLeft.getX(), bottomLeft.getY() + height, bottomLeft.getZ());
	glVertex3f(bottomLeft.getX() + width, bottomLeft.getY(), bottomLeft.getZ());
	glVertex3f(topRight.getX(), topRight.getY(), topRight.getZ());
  glVertex3f(bottomLeft.getX(), bottomLeft.getY(), bottomLeft.getZ());
	glEnd();
}

void Button::renderBuffer() {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 12);
}
