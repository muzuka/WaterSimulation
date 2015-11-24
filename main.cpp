

/*
 *  Water Simulation
 *  
 *  Author: Sean Brown
 *
 */

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h> 
#include <vector>
#include <map>
#include "Vector.h"
#include "Particle.h"

#define _GLFW_X11
#define _GLFW_GLX
#define _GLFW_USE_OPENGL
#define _GLFW_HAS_GLXGETPROCADDRESS
#define GLFW_INCLUDE_GLU

#ifdef __APPLE__
#include "GLFW/glfw3.h"
#else
#define GLFW_INCLUDE
#include <GLFW/glfw3.h>
#endif

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

double pointSize = 10.0f;
double nearPlane = 1.0f;
double farPlane  = 100.0f;
double fov       = 60.0f;
int   width      = 1024;
int   height     = 760;

void initCup() {

}

void initShower() {
    for(unsigned int i = 0; i < particles.size(); i++) {
        particles.push_back(Particle(Vector(i/10.0f, 0.9f, 0.0f), Vector(0.0f, 0.0f, 1.0f)));
    }
}

void initWaterfall() {

}

void initFunnel() {

}

void initStirring() {

}

void init() {

    particles = vector<Particle>(numOfPoints);

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

}

void render() {
  
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
        
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, width/height, nearPlane, farPlane);
  
    //for(unsigned int i = 0; i < particles.size(); i++) {
        //particles[i].render();
    //}
    
    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main(int argc, char **argv)
{

	if(!glfwInit()) {
    cout << "glfw failed to initialize" << endl;
		exit(EXIT_FAILURE);
	}
	
	window = glfwCreateWindow(width, height, "Water Animation", NULL, NULL);
	if(!window) {
    cout << "Window failed to be created" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
  
	glfwMakeContextCurrent(window);

	init();
  
	while(!glfwWindowShouldClose(window)) {
    update();

    render();
  }
  
	glfwDestroyWindow(window);
	glfwTerminate();
	
	return 0;
}

