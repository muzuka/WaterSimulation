

/*
 *  Water Simulation
 *  
 *  Contains Simulations:
 *    CUP       - water falling into a cup
 *    SHOWER    - water falls onto surface
 *    WATERFALL - water flows over edge
 *    FUNNEL    - water falls through a funnel
 *    STIRRING  - water is in cup and can be stirred
 * 
 *  Written by: Sean Brown
 *
 */

#define _GLFW_X11
#define _GLFW_GLX
#define _GLFW_USE_OPENGL
#define _GLFW_HAS_GLXGETPROCADDRESS
#define GLFW_INCLUDE_GLU
#define GLFW_INCLUDE_GL_3
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h> 
#include <vector>
#include <map>
#include "Vector.h"
#include "Particle.h"
#include "Simulation.h"
#include "Triangle.h"
#include "Model.h"

using namespace std;

GLFWwindow* window;

Model mesh;
vector<Particle> particles;

Simulation sim    = WATERFALL;
int pointXStart   = 0;
int pointYStart   = 0;
int pointZStart   = 0;

// Waterfall
int waterfallHeight = 5;
int waterfallWidth  = 5;
int waterfallDepth  = 5;

// Funnel
int funnelHeight  = 5;
int funnelWidth   = 5;
int funnelDepth   = 5;

// Stirring
int stirringHeight  = 5;
int stirringWidth   = 5;
int stirringDepth   = 5;

int pointHeight   = 5;
int pointWidth    = 5;
int pointDepth    = 5;
int numOfPoints   = 10;
bool simulate     = false;
bool debug        = false;
double sigma      = 1.5f;
double viscosity  = 0.1f;
Vector gravity    = Vector(0.0f, -1.0f, 0.0f);

double timeStep  = 0.1f;
double pointSize = 10.0f;
double nearPlane = 1.0f;
double farPlane  = 100.0f;
double fov       = 60.0f;
double zoom      = -1.0f;
double rotationX = 0.0f;
double rotationY = 0.0f;
int width        = 1024;
int height       = 760;
int menuWidth    = 100;
int menuHeight   = 250;

//rendering
GLenum       glError;
unsigned int shaderProgram;
unsigned int triangleShaderProgram;
unsigned int vertexShader;
unsigned int fragShader;
unsigned int particleVBO;
unsigned int particleVAO;
int positionInfo;

const char* vertexShaderText =
  "#version 120\n"
  "void main() {"
    "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
  "}";
const char* fragmentShaderText =
  "#version 120\n"
  "uniform float density;"
  "void main() {"
  "gl_FragColor = vec4(0.372, 0.659/density, 1.0, 1.0);"
  "}";
  
const char* triangleVertShaderText = 
  "#version 120\n"
  "void main() {"
  "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
  "}";
  
const char* triangleFragShaderText =
  "#version 120\n"
  "void main() {"
  "gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);"
  "}";

void checkForError(const char* func) {
  glError = glGetError();
  if(glError != GL_NO_ERROR) {
    switch(glError) {
      case GL_INVALID_ENUM:
        cout << "invalid enum: " << func << endl;
        break;
      case GL_INVALID_VALUE:
        cout << "invalid value: " << func << endl;
        break;
      case GL_INVALID_OPERATION:
        cout << "invalid operation: " << func << endl;
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        cout << "invalid framebuffer operation: " << func << endl;
        break;
      case GL_OUT_OF_MEMORY:
        cout << "Out of memory: " << func << endl;
        break;
      default:
        cout << "something else: " << func << endl;
        break;
    }
  }
}

void cleanUp() {
  glDeleteProgram(shaderProgram);
  glDeleteProgram(triangleShaderProgram);

  glfwDestroyWindow(window);
  glfwTerminate();
}

double kernel(Vector p) {
  double normalDistance = Vector::dotProduct(p, p) / (2 * pow(sigma, 2));
  if(normalDistance > 50)
    return 0;
  else
    return (1.0f / pow((sqrt(3.14f * 2) * sigma), 3)) * exp(-normalDistance);
}

Vector kernelGradient(Vector p, int i) {
  Vector temp = p;
  return ((temp / pow(sigma, 3)) * -1) * kernel(p);
}

double density(int i) {
  double result = 0.0f;
  for(int j = 0; j < numOfPoints; j++) {
    result = result + kernel(particles[i].getPosition() - particles[j].getPosition());
  }
  return result;
}

double pressure(int i) {
  return 0.01f * pow(particles[i].getDensity() - 0.05f, 9.0f);
}

Vector accelDueToPressure(int i) {
  Vector result = Vector();
  for(int j = 0; j < numOfPoints; j++) {
    result = result + (kernelGradient(particles[i].getPosition() - particles[j].getPosition(), j) * (particles[j].getPressure() + particles[i].getPressure())) / (particles[j].getDensity() * 2.0f);  
    if(debug && (j % 25 == 0)) {
      cout << j << ": accelPressure" << endl;
      cout << "kernelGradient: ";
      kernelGradient(particles[i].getPosition() - particles[j].getPosition(), j).print();
      cout << "pressure: " << (particles[j].getPressure() + particles[i].getPressure()) << endl;
      cout << "density: " << particles[j].getDensity() << endl;
      cout << "result: ";
      result.print();
      cout << "accelPressure: ";
    }
  }
  result.print();
  return result / particles[i].getDensity();
}

Vector accelDueToViscosity(int i) {
  Vector result = Vector();
  for(int j = 0; j < numOfPoints; j++)
    result = result + ((particles[j].getVelocity() - particles[i].getVelocity()) / particles[j].getDensity()) * viscosity * kernel(particles[j].getPosition() - particles[i].getPosition());
  return result / particles[i].getDensity();
}

void initCup() {
  zoom = -5.0f;
  rotationX = 50.0f;
  
  for(int i = 0; i < pointWidth; i++) {
    for(int j = 0; j < pointHeight; j++) {
      particles.push_back(Particle(Vector(i/10.0f, 0.9f - (j/10.0f), 0.0f)));
    }
  }
  mesh = Model("models/CUP.obj");
}

void initShower() {
  zoom = -5.0f;
  rotationX = 30.0f;
  
  for(int i = 0; i < pointWidth; i++)
      particles.push_back(Particle(Vector(i/10.0f, 0.9f, 0.0f)));
  mesh = Model("models/SHOWER.obj");
}

void initWaterfall() {
  zoom = -4.0f;
  rotationX = 30.0f;
  rotationY = -40.0f;
  
  for(int i = 0; i < waterfallWidth; i++) {
    for(int j = 0; j < waterfallHeight; j++) {
      for(int k = 0; k < waterfallDepth; k++) {
        particles.push_back(Particle(Vector(i/10.0f, 0.9f + (j/10.0f), (k/10.0f))));
      }
    }
  }
  mesh = Model("models/WATERFALL.obj");
}

void initFunnel() {
  zoom = -3.0f;
  rotationX = 60.0f;
  
  for(int i = 0; i < funnelWidth; i++) {
    for(int j = 0; j < funnelHeight; j++) {
      for(int k = 0; k < funnelDepth; k++) {
        particles.push_back(Particle(Vector(i/10.0f, 0.9f - (j/10.0f), (k/10.0f))));
      } 
    }
  }
  mesh = Model("models/FUNNEL.obj");
}

void initStirring() {
  zoom = -3.0f;
  rotationX = 50.0f;
  
  for(int i = 0; i < stirringWidth; i++) {
    for(int j = 0; j < stirringHeight; j++) {
      for(int k = 0; k < stirringDepth; k++) {
        particles.push_back(Particle(Vector(i/10.0f, 0.9f - (j/10.0f), (k/10.0f))));
      } 
    }
  }
  mesh = Model("models/CUP.obj");
}

void attachShaders(unsigned int vs, unsigned int fs, unsigned int *shaderProg) {
  *shaderProg = glCreateProgram();
  glAttachShader(*shaderProg, vs);
  glAttachShader(*shaderProg, fs);
}

// compiles and returns program
unsigned int loadShaders(const char* vertex, const char* fragment) {
  unsigned int tempProgram;
  int status;
  char buffer[512];

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertex, NULL);
  glCompileShader(vertexShader);

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
  if(!status) {
    glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
    cout << "Vertex Shader:" << endl;
    cout << buffer << endl;
    cleanUp();
    exit(EXIT_FAILURE);
  }

  fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragShader, 1, &fragment, NULL);
  glCompileShader(fragShader);

  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
  if(!status) {
    glGetShaderInfoLog(fragShader, 512, NULL, buffer);
    cout << "Fragment Shader:" << endl;
    cout << buffer << endl;
    cleanUp();
    exit(EXIT_FAILURE);
  }

  attachShaders(vertexShader, fragShader, &tempProgram);

  glLinkProgram(tempProgram);
  checkForError("glLinkProgram");

  return tempProgram;
}

void init() {

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if(GLEW_OK != err) {
      cout << "glew failed" << endl;
      cleanUp();
      exit(EXIT_FAILURE); 
    }
    
    glEnable(GL_DEPTH_TEST);
    glPointSize(pointSize);

    particles = vector<Particle>();

    switch(sim) {
        case CUP:
          numOfPoints = pointWidth * pointHeight;
          initCup();
          break;
        case SHOWER:
          numOfPoints = pointWidth;
          initShower();
          break;
        case WATERFALL:
          numOfPoints = waterfallWidth * waterfallHeight * waterfallDepth;
          initWaterfall();
          break;
        case FUNNEL:
          numOfPoints = funnelWidth * funnelHeight * funnelDepth;
          initFunnel();
          break;
        case STIRRING:
          numOfPoints = stirringWidth * stirringHeight * stirringDepth;
          initStirring();
          break;
        default:
          break;
    }

    shaderProgram = loadShaders(vertexShaderText, fragmentShaderText);
    triangleShaderProgram = loadShaders(triangleVertShaderText, triangleFragShaderText);
    
    simulate = false;
}

Vector reflect(Vector v, Vector n, double b, double s) {
  n.normalize();
  n = n * Vector::dotProduct(v, n);
  return (n * -b) + ((v - n) * s);
}

// checks ith particle
void checkCollision(int i) {
    Vector oldPos, newPos;
    double bounce = 0.1f;
    double slide  = 0.8f;

    // detect collision and change velocity
    oldPos = particles[i].getPosition();
    newPos = oldPos + (particles[i].getVelocity() * timeStep);
    for(Triangle t : mesh.getMesh()) {
      if(t.intersect(oldPos, newPos)) {
        particles[i].setPosition(t.getCollision());
        particles[i].setVelocity(reflect(particles[i].getVelocity(), t.getNormal(), bounce, slide));
      }
    }
}

void update() {
  // Set density and pressure
  for(int i = 0; i < numOfPoints; i++) {
    particles[i].setDensity(density(i));
    particles[i].setPressure(pressure(i));
    if(debug) {
      cout << "density: " << particles[i].getDensity() << endl;
      cout << "pressure: " << particles[i].getPressure() << endl;
    }
  }
  // set acceleration
  for(int i = 0; i < numOfPoints; i++) {
    Vector accPressure = accelDueToPressure(i);
    Vector accViscosity = accelDueToViscosity(i);
    particles[i].setAcceleration(accPressure + accViscosity + gravity);
    if(debug && (i % 10 == 0)) {
      cout << i << ": acceleration ";
      particles[i].getAcceleration().print();
      cout << i << ": pressure ";
      accPressure.print();
      cout << i << ": viscosity ";
      accViscosity.print();
    }
  }
  // move points
  for(int i = 0; i < numOfPoints; i++) {
    particles[i].setVelocity(particles[i].getVelocity() + (particles[i].getAcceleration() * timeStep));
    
    checkCollision(i);

    particles[i].setPosition(particles[i].getPosition() + (particles[i].getVelocity() * timeStep));
    //if(debug) {
      //cout << i << ": velocity ";
      //particles[i].getVelocity().print();
    //}
  }
}

void render() {

    // MAIN Rendering Start ############################################
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, width/height, nearPlane, farPlane);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, zoom);
    glRotatef(rotationX, 1.0f, 0.0f, 0.0f);
    glRotatef(rotationY, 0.0f, 1.0f, 0.0f);
      
    glUseProgram(triangleShaderProgram);
      
    for(Triangle t : mesh.getMesh())
      t.render();
      
    glUseProgram(shaderProgram);

    int densityUniformLoc;

    for(Particle p : particles) {
      densityUniformLoc = glGetUniformLocation(shaderProgram, "density");
      checkForError("glGetUniformLocation");
      glUniform1f(densityUniformLoc, p.getDensity());
      checkForError("glUniform1f");
      p.render();
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
}

// Use mouse wheel to zoom
void scrollFunc(GLFWwindow* win, double x, double y) {
    zoom += y;
}

void keyboardFunc(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS) {
    switch(key) {
      case GLFW_KEY_ENTER:
        update();
        break;
      case GLFW_KEY_SPACE:
        simulate = !simulate;
        break;
      case GLFW_KEY_D:
        debug = !debug;
        break;
      case GLFW_KEY_UP:
        rotationX += 10.0f;
        break;
      case GLFW_KEY_DOWN:
        rotationX -= 10.0f;
        break;
      case GLFW_KEY_LEFT:
        rotationY += 10.0f;
        break;
      case GLFW_KEY_RIGHT:
        rotationY -= 10.0f;
        break;
      case GLFW_KEY_W:
        sim = WATERFALL;
        init();
        break;
      case GLFW_KEY_E:
        sim = FUNNEL;
        init();
        break;
      case GLFW_KEY_R:
        sim = STIRRING;
        init();
        break;
      default:
        break;
    }
  }
}

void errorFunc(int error, const char* description) {
  cout << description << endl;
}

int main(int argc, char **argv)
{

  glfwSetErrorCallback(errorFunc);
	if(!glfwInit()) {
    cout << "glfw failed to initialize" << endl;
		exit(EXIT_FAILURE);
	}
	
	window = glfwCreateWindow(width, height, "SPH Simulation", NULL, NULL);
	if(!window) {
    cout << "Window failed to be created" << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
  
	glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, keyboardFunc);
  glfwSetScrollCallback(window, scrollFunc);

	init();

  cout << "about to start loop" << endl;
	while(!glfwWindowShouldClose(window)) {
    if(simulate) {
      update();
    }

    render();
  }
  
  cleanUp();
	
	return 0;
}

