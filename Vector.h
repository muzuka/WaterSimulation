 #ifndef _VECTOR_H_
 #define _VECTOR_H_

/*
 * vector class
 *
 * From CPSC 589
 *
 * Sean Brown
 *
 */

 #include <math.h>
 #include <stdio.h>
 #include <string>
 #include <sstream>

class Vector {
	
protected:
	double x, y, z;

public:
	Vector();
	Vector(double, double, double);

	Vector operator*(double);
	Vector operator+(Vector);
	Vector operator-(Vector);
  	Vector operator/(double);
  	Vector operator+=(Vector);
  	Vector operator-=(Vector);
  	bool   operator==(Vector);

	double getX();
	double getY();
	double getZ();

	void setX(double);
	void setY(double);
	void setZ(double);

	static Vector crossProduct(Vector, Vector);
	static double dotProduct(Vector, Vector);

	double length();
	void normalize();

  	void printOneLine();
	void print();

};

#endif
