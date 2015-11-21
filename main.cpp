
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include <vector>
#include "BSpline.h"

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

GLFWwindow* window;

int   width  = 1024;
int   height = 760;

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

    glPointSize(pointSize);

    while(!glfwWindowShouldClose(window)) {

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 1;

}