
/*
 * Triangle class for collisions
 *
 * 
 * Written by:Sean Brown
 */

#include "Triangle.h"

 using namespace std;

Triangle::Triangle() {

}

Triangle::Triangle(Vector i, Vector j, Vector k) {
	this->i = i;
	this->j = j;
	this->k = k;
	Vector ij = i - j;
	Vector kj = k - j;
	this->normal = Vector::crossProduct(ij, kj);
	this->normal.normalize();
}

Vector Triangle::getCollision() {
	return collision;
}

Vector Triangle::getNormal() {
	return normal;
}

void Triangle::render() {
	glBegin(GL_LINE_STRIP);
		glVertex3f(i.getX(), i.getY(), i.getZ());
		glVertex3f(j.getX(), j.getY(), j.getZ());
		glVertex3f(k.getX(), k.getY(), k.getZ());
	glEnd();

  // glBegin(GL_TRIANGLES);
  //   glVertex3f(i.getX(), i.getY(), i.getZ());
  //   glVertex3f(j.getX(), j.getY(), j.getZ());
  //   glVertex3f(k.getX(), k.getY(), k.getZ());
  // glEnd();
}

// Code from http://geomalgorithms.com/a06-_intersect-2.html
bool Triangle::intersect(Vector p1, Vector p2) {
	Vector dir, wo, w;
	float r, a, b;

	dir = p2 - p1;
	wo = p1 - i;
	a = -Vector::dotProduct(normal, wo);
	b = Vector::dotProduct(normal, dir);
	if(fabs(b) < 0.000001) {
		if(a == 0)
			return false;
		else
			return false;
	}

	// get intersect point on plane
	r = a / b;
	if(r < 0.0f || r > 1.0f)
		return false;

	collision = p1 + dir * r;

	float uu, uv, vv, wu, wv, denom;
  	Vector u, v;
  	u = j - i;
  	v = k - i;

	// is point inside Triangle?
	uu = Vector::dotProduct(u, u);
	uv = Vector::dotProduct(u, v);
	vv = Vector::dotProduct(v, v);
	w = collision - i;
	wu = Vector::dotProduct(w, u);
	wv = Vector::dotProduct(w, v);
	denom = uv * uv - uu * vv;

	// get and test parametric coordinates
	float s, t;
	s = (uv * wv - vv * wu) / denom;
	if(s < 0.0f || s > 1.0f)
		return false;
	t = (uv * wu - uu * wv) / denom;
	if(t < 0.0f || (s+t) > 1.0f)
		return false;

	return true;
}

bool Triangle::intersectMT(Vector p1, Vector p2) {
  Vector e1, e2;
  Vector p, q, T, d;
  double det, inv_det, u, v;
  double t;
  
  e1 = j - i;
  e2 = k - i;
  d = p2 - p1;
  
  p = Vector::crossProduct(d, e2);
  det = Vector::dotProduct(e1, p);
  
  if(det > -0.000001f && det < 0.000001f)
    return false;
  inv_det = 1.0f / det;
  
  T = p1 - i;
  
  u = Vector::dotProduct(T, p) * inv_det;
  if(u < 0.0f || u > 1.0f)
    return false;
  
  q = Vector::crossProduct(T, e1);
  
  v = Vector::dotProduct(d, q) * inv_det;
  if(v < 0.0f || u + v > 1.0f)
    return 0;
    
  t = Vector::dotProduct(e2, q) * inv_det;
  
  if(t > 0.000001f) {
    return true;
  }
  
  return false;
}
