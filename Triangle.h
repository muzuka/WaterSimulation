#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

/*
 * Triangle class for collisions
 *
 *
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

		bool intersect(Vector, Vector);
};

#endif