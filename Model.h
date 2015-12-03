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
		vector<Triangle> mesh;

	public:
	
	Model();
	Model(vector<Triangle>);
	Model(const char*);

	void extractOBJ(const char*);
};

#endif