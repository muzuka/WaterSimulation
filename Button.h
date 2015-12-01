
#ifndef _BUTTON_H_
#define _BUTTON_H_

/*
 *	Button class definition
 *
 */

#define _GLFW_X11
#define _GLFW_GLX
#define _GLFW_USE_OPENGL
#define _GLFW_HAS_GLXGETPROCADDRESS
#define GLFW_INCLUDE_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string.h>
#include "Simulation.h"
#include "Vector.h"

class Button {

private:
  unsigned int vbo;
  Simulation buttonChange;
  Vector bottomLeft;
  Vector topRight;
  Vector color;
  double width;
  double height;
  double vertices[12];

public:
	Button(Simulation, Vector, Vector);

	void render();
	void renderBuffer();

};

#endif
