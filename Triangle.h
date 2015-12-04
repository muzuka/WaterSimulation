#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

/*
 * Triangle class for collisions
 *
 * Represents a wall
 * 
 * Written by: Sean Brown
 */

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

    // tests for intersection by a line
		bool intersect(Vector, Vector);
};

#endif
