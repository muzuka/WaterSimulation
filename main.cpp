

/*
 *  Water Simulation
 *  
 *  Author: Sean Brown
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include <vector>
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

typedef enum Simulation {
    CUP,
    WATERFALL,
    FUNNEL,
    STIRRING
};

GLFWwindow* window;

vector<Particle> particles;

Simulation        = CUP;
int   numOfPoints = 10;

int   width  = 1024;
int   height = 760;

void init() {

    particles = vector<Particle>(numOfPoints);

    
}

void update() {

}

void render() {
    for(unsigned int i = 0; i < particles.size(); i++) {
        particles[i].render();
    }
}

int main(int argc, char* argv[]) {

    if(!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    window = glfwCreateWindow(width, height, "The B Spline - CPSC 589 Assignment 2", NULL, NULL);
    if(!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    init();

    while(!glfwWindowShouldClose(window)) {

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        
        update();

        render();

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 1;

}