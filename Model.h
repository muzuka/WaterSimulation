#ifndef _MODEL_H_
#define _MODEL_H_

/*
 *	Model class
 *
 *  holds collection of triangles
 *
 *  Written by: Sean Brown
 */

#include <vector>
#include "Triangle.h"

class Model {
	private:
		std::vector<Triangle> mesh;

	public:
	
	Model();
	Model(std::vector<Triangle>);
	Model(const char*);

	std::vector<Triangle> getMesh();

	std::vector<Triangle> extractOBJ(const char*);
};

#endif