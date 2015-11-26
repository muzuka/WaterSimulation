#ifndef _PARTICLE_H_
#define _PARTICLE_H_

/*
 * Particle with mass
 * 
 * Written by: Sean Brown
 * 
 */

#define _GLFW_X11
#define _GLFW_GLX
#define _GLFW_USE_OPENGL
#define _GLFW_HAS_GLXGETPROCADDRESS
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

 #include <vector>
 #include "Vector.h"
 
 class Particle {
 private:
   Vector pos;
   Vector velocity;
   Vector color;
   Vector acceleration;
   double density;
   double pressure;

 public:
  Particle();                         // defaults: color = white, mass = 1.0f, pos = (0, 0, 0)
  Particle(Vector);                   // (pos)
 	Particle(Vector, Vector);           // (pos, color)

  void render();

 	Vector getPosition();
  Vector getVelocity();
  Vector getColor();
  Vector getAcceleration();
 	double getDensity();
  double getPressure();

 	void setPosition(Vector);
 	void setPosition(double, double, double);
  void setVelocity(Vector);
  void setVelocity(double, double, double);
  void setColor(Vector);
  void setColor(double, double, double);
  void setAcceleration(Vector);
  void setAcceleration(double, double, double);
 	void setDensity(double);
  void setPressure(double);

 };

#endif
