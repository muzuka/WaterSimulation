
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
#include <GLFW/glfw3.h>

#include "Simulation.h"
#include "Vector.h"

class Button {

private:
  Simulation buttonChange;
  Vector bottomLeft;
  Vector topRight;
  double width;
  double height;

public:
	Button(Simulation, Vector, Vector);

	void render();

};

#endif