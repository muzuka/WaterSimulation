
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

#include "Particle.h"


Particle::Particle() {// defaults: color = white, mass = 1.0f, pos = (0, 0, 0)
	this->pos = Vector(0.0f, 0.0f, 0.0f);
	this->velocity = Vector(0.0f, 0.0f, 0.0f);
	this->color = Vector(1.0f, 1.0f, 1.0f);
	this->acceleration = Vector(0.0f, 0.0f, 0.0f);
	this->density = 0.0f;
	this->pressure = 0.0f;
}
  
Particle::Particle(Vector p) {
	this->pos = p;
	this->velocity = Vector(0.0f, 0.0f, 0.0f);
	this->color = Vector(1.0f, 1.0f, 1.0f);
	this->acceleration = Vector(0.0f, 0.0f, 0.0f);
	this->density = 0.0f;
	this->pressure = 0.0f;
}

Particle::Particle(Vector p, Vector c){
	this->pos = p;
	this->velocity = Vector(0.0f, 0.0f, 0.0f);
	this->acceleration = Vector(0.0f, 0.0f, 0.0f);
	this->density = 0.0f;
	this->pressure = 0.0f;
	this->color = c;
}

void Particle::render() {
	glBegin(GL_POINTS);
	glColor3f(color.getX(), color.getY(), color.getZ());
	glVertex3f(pos.getX(), pos.getY(), pos.getZ());
	glEnd();
}

Vector Particle::getPosition() {
	return this->pos;
}

Vector Particle::getVelocity() {
	return this->velocity;
}

Vector Particle::getColor() {
	return this->color;
}

Vector Particle::getAcceleration() {
	return this->acceleration;
}

double Particle::getDensity() {
	return this->density;
}

double Particle::getPressure() {
	return this->pressure;
}

void Particle::setPosition(Vector p) {
	this->pos = p;
}

void Particle::setPosition(double x, double y, double z) {
	this->pos = Vector(x, y, z);
}

void Particle::setVelocity(Vector v) {
	this->velocity = v;
}

void Particle::setVelocity(double x, double y, double z) {
	this->velocity = Vector(x, y, z);
}

void Particle::setColor(Vector c) {
	this->color = c;
}

void Particle::setColor(double r, double g, double b) {
	this->color = Vector(r, g, b);
}

void Particle::setAcceleration(Vector f) {
	this->acceleration = f;
}

void Particle::setAcceleration(double x, double y, double z) {
	this->acceleration = Vector(x, y, z);
}

void Particle::setDensity(double d) {
	this->density = d;
}

void Particle::setPressure(double p) {
	this->pressure = p;
}
