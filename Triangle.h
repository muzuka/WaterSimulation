#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

/*
 * Triangle class for collisions
 *
 * Represents a wall
 * 
 * Written by: Sean Brown
 */
 
#define _GLFW_X11
#define _GLFW_GLX
#define _GLFW_USE_OPENGL
#define _GLFW_HAS_GLXGETPROCADDRESS
#define GLFW_INCLUDE_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Vector.h"

class Triangle {
	private:
		Vector i, j, k;
		Vector normal;
		Vector collision;

	public:

		Triangle();
		Triangle(Vector, Vector, Vector);

		Vector getCollision();
		Vector getNormal();
    
    void render();

    // tests for intersection by a line
		bool intersect(Vector, Vector);
};

#endif
