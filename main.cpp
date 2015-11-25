

/*
 *  Water Simulation
 *  
 *  Author: Sean Brown
 *
 */

#define _GLFW_X11
#define _GLFW_GLX
#define _GLFW_USE_OPENGL
#define _GLFW_HAS_GLXGETPROCADDRESS
#define GLFW_INCLUDE_GLU

#include <GLFW/glfw3.h>

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h> 
#include <vector>
#include <map>
#include "Vector.h"
#include "Particle.h"

using namespace std;

typedef enum {
    CUP,
    SHOWER,
    WATERFALL,
    FUNNEL,
    STIRRING
} Simulation;

GLFWwindow* window;

vector<Particle> particles;

Simulation sim    = SHOWER;
int   numOfPoints = 10;
bool   simulate   = false;
double sigma      = 0.42f;
Vector gravity    = Vector(0.0f, -0.001f, 0.0f);

double timeStep  = 1.0f;
double pointSize = 10.0f;
double nearPlane = 1.0f;
double farPlane  = 100.0f;
double fov       = 60.0f;
int   width      = 1024;
int   height     = 760;

double kernel(Vector p) {
  double normalDistance = Vector::dotProduct(p, p) / (2 * sigma);
  if(normalDistance > 50)
    return 0;
  else
    return (1.0f / (3.14f * 2 * sigma)) * exp(-normalDistance);
}

Vector kernelGradient(Vector p) {
  Vector temp = p;
  return ((temp / sigma) * -1) * kernel(p);
}

double density(int i) {
  double result = 0.0f;
  for(int j = 0; j < numOfPoints; j++)
    result += kernel(particles[i].getPosition() - particles[j].getPosition());
  return result;
}

double pressure(int i) {
  return 0.01f * pow(particles[i].getDensity() - 0.05f, 9.0f);
}

Vector accelDueToPressure(int i) {
  Vector result = Vector();
  for(int j = 0; j < numOfPoints; j++)
    result += kernelGradient(particles[j].getPosition() - particles[i].getPosition()) * (particles[j].getPressure() + particles[i].getPressure()) / particles[j].getDensity();
  return result / particles[i].getDensity();
}

Vector accelDueToViscosity(int i) {
  Vector result = Vector();
  for(int j = 0; j < numOfPoints; j++)
    result += ((particles[j].getVelocity() - particles[i].getVelocity()) / particles[j].getDensity()) * 0.1f * kernel(particles[j].getPosition() - particles[i].getPosition());
  return result / particles[i].getDensity();
}

void initCup() {

}

void initShower() {
    for(unsigned int i = 0; i < numOfPoints; i++) {
        particles.push_back(Particle(Vector(i/10.0f, 0.9f, -2.0f), Vector(1.0f, 1.0f, 1.0f)));
    }
}

void initWaterfall() {

}

void initFunnel() {

}

void initStirring() {

}

void init() {

    glPointSize(pointSize);

    switch(sim) {
        case CUP:
            initCup();
            break;
        case SHOWER:
            initShower();
            break;
        case WATERFALL:
            initWaterfall();
            break;
        case FUNNEL:
            initFunnel();
            break;
        case STIRRING:
            initStirring();
            break;
        default:
            break;
    }
}

void update() {
  for(int i = 0; i < numOfPoints; i++) {
    particles[i].setDensity(i);
    particles[i].setPressure(i);
  }
  for(int i = 0; i < numOfPoints; i++)
    particles[i].setAcceleration(accelDueToPressure(i) + accelDueToViscosity(i) + gravity);
  for(int i = 0; i < numOfPoints; i++) {
    particles[i].setVelocity(particles[i].getVelocity() + (particles[i].getAcceleration() * timeStep));
    particles[i].setPosition(particles[i].getPosition() + (particles[i].getVelocity() * timeStep));
  }
}

void render() {
  
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
        
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, width/height, nearPlane, farPlane);
  
    for(unsigned int i = 0; i < particles.size(); i++) {
        particles[i].render();
    }
    
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void keyboardFunc(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS) {
    if(key == GLFW_KEY_ENTER)
      update();
    if(key == GLFW_KEY_SPACE)
      simulate = !simulate;
  }
}

int main(int argc, char **argv)
{

	if(!glfwInit()) {
    cout << "glfw failed to initialize" << endl;
		exit(EXIT_FAILURE);
	}
	
	window = glfwCreateWindow(width, height, "Test", NULL, NULL);
	if(!window) {
    cout << "Window failed to be created" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
  
	glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, keyboardFunc);

	init();
  
	while(!glfwWindowShouldClose(window)) {
    while(simulate) {
      update();
    }

    render();
  }
  
	glfwDestroyWindow(window);
	glfwTerminate();
	
	return 0;
}

