#ifndef _MODEL_H_
#define _MODEL_H_

/*
 *	Model class
 *
 *  holds collection of triangles
 *
 *  Written by: Sean Brown
 */

#define _GLFW_X11
#define _GLFW_GLX
#define _GLFW_USE_OPENGL
#define _GLFW_HAS_GLXGETPROCADDRESS
#define GLFW_INCLUDE_GLU
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include "Triangle.h"

class Model {
	private:
		std::vector<Triangle> mesh;

	public:
	
	Model();
	Model(std::vector<Triangle>);
	Model(const char*);

	void render();

	std::vector<Triangle> getMesh();

	std::vector<Triangle> extractOBJ(const char*);
};

#endif