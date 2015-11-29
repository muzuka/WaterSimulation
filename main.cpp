

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
#include "Button.h"
#include "Simulation.h"

using namespace std;

GLFWwindow* menu;
GLFWwindow* window;

vector<Particle> particles;
vector<Button> buttons;

Simulation sim    = SHOWER;
int pointHeight   = 5;
int pointWidth    = 5;
int pointDepth    = 5;
int numOfPoints   = 10;
bool   simulate   = false;
double sigma      = 0.42f;
Vector gravity    = Vector(0.0f, -0.001f, 0.0f);

double timeStep  = 1.0f;
double pointSize = 10.0f;
double nearPlane = 1.0f;
double farPlane  = 100.0f;
double fov       = 60.0f;
int width        = 1024;
int height       = 760;
int menuWidth    = 100;
int menuHeight   = 500;

//rendering
GLenum       glError;
unsigned int shaderProgram;
unsigned int vertexShader;
unsigned int fragShader;
unsigned int particleVBO;
unsigned int particleVAO;
int positionInfo;
const char* vertexShaderText =
  "#version 120\n"
  "void main() {"
    "gl_Position = gl_Vertex;"
  "}";
const char* fragmentShaderText =
  "#version 120\n"
  "void main() {"
    "gl_FragColor = vec4(0.372, 0.659, 1.0, 1.0);"
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
  for(int j = 0; j < numOfPoints; j++) {
    cout << "accessing " << i << " and " << j << endl;
    result += kernel(particles[i].getPosition() - particles[j].getPosition());
  }
  cout << "set Density" << endl;
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

vector<Vector> getParticlePositions() {
  vector<Vector> positions = vector<Vector>();
  for(int i = 0; i < numOfPoints; i++) {
    positions.push_back(particles[i].getPosition());
  }
  return positions;
}

void cleanUp() {
  glDeleteProgram(shaderProgram);
  glDeleteShader(fragShader);
  glDeleteShader(vertexShader);
  glDeleteBuffers(1, &particleVBO);
  glDeleteVertexArrays(1, &particleVAO);

  glfwDestroyWindow(menu);
  glfwDestroyWindow(window);
  glfwTerminate();
}

void initCup() {
  for(int i = 0; i < pointWidth; i++) {
    for(int j = 0; j < pointHeight; j++) {
      particles.push_back(Particle(Vector(i/10.0f, 0.9f - (j/10.0f), -2.0f)));
    }
  }
}

void initShower() {
  for(int i = 0; i < pointWidth; i++)
      particles.push_back(Particle(Vector(i/10.0f, 0.9f, -2.0f)));
}

void initWaterfall() {
  for(int i = 0; i < pointWidth; i++) {
    for(int j = 0; j < pointHeight; j++) {
      for(int k = 0; k < pointDepth; k++) {
        particles.push_back(Particle(Vector(i/10.0f, 0.9f + (j/10.0f), -2.0f - (k/10.0f))));
      }
    }
  }
}

void initFunnel() {
  for(int i = 0; i < pointWidth; i++) {
    for(int j = 0; j < pointHeight; j++) {
      particles.push_back(Particle(Vector(i/10.0f, 0.9f - (j/10.0f), -2.0f)));
    }
  }
}

void initStirring() {
  for(int i = 0; i < pointWidth; i++) {
    for(int j = 0; j < pointHeight; j++) {
      for(int k = 0; k < pointDepth; k++) {
        particles.push_back(Particle(Vector(i/10.0f, 0.9f - (j/10.0f), -2.0f - (k/10.0f))));
      } 
    }
  }
}

void initButtons() {
  buttons = vector<Button>();

  double z = 0.0f;

  buttons.push_back(Button(CUP, Vector(-1.0f, 0.9f, z), Vector(1.0f, 1.0f, z)));
  buttons.push_back(Button(SHOWER, Vector(-1.0f, 0.8f, z), Vector(1.0f, 0.9f, z)));
  buttons.push_back(Button(WATERFALL, Vector(-1.0f, 0.7f, z), Vector(1.0f, 0.8f, z)));
  buttons.push_back(Button(FUNNEL, Vector(-1.0f, 0.6f, z), Vector(1.0f, 0.7f, z)));
  buttons.push_back(Button(STIRRING, Vector(-1.0f, 0.5f, z), Vector(1.0f, 0.6f, z)));
}

void attachShaders(unsigned int vs, unsigned int fs) {
  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vs);
  glAttachShader(shaderProgram, fs);
}

void loadShaders() {
  int status;
  char buffer[512];

  vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderText, NULL);
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
  glShaderSource(fragShader, 1, &fragmentShaderText, NULL);
  glCompileShader(fragShader);

  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);
  if(!status) {
    glGetShaderInfoLog(fragShader, 512, NULL, buffer);
    cout << "Fragment Shader:" << endl;
    cout << buffer << endl;
    cleanUp();
    exit(EXIT_FAILURE);
  }

  attachShaders(vertexShader, fragShader);

  //glBindFragDataLocation(shaderProgram, 0, "outColor");

  glLinkProgram(shaderProgram);
  checkForError("glLinkProgram");

  glUseProgram(shaderProgram);
  checkForError("glUseProgram");
}

void init() {

    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if(GLEW_OK != err) {
      cout << "glew failed" << endl;
      cleanUp();
      exit(EXIT_FAILURE); 
    }

    particles = vector<Particle>();

    glPointSize(pointSize);

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
          numOfPoints = pointWidth * pointHeight * pointDepth;
          initWaterfall();
          break;
        case FUNNEL:
          numOfPoints = pointWidth * pointHeight;
          initFunnel();
          break;
        case STIRRING:
          numOfPoints = pointWidth * pointHeight * pointDepth;
          initStirring();
          break;
        default:
          break;
    }

    //loadShaders();

    /*glBindAttribLocation(shaderProgram, 2, "position");
    //positionInfo = glGetAttribLocation(shaderProgram, "position");
    checkForError("glBindAttribLocation");
    cout << "position info = " << positionInfo << endl;
    glVertexAttribPointer(2, 3, GL_DOUBLE, GL_FALSE, 0, 0);
    checkForError("glVertexAttribPointer");
    glEnableVertexAttribArray(positionInfo);
    checkForError("glEnableVertexAttribArray");

    glGenVertexArrays(1, &particleVAO);
    checkForError("glGenVertexArrays");

    glGenBuffers(1, &particleVBO);
    checkForError("glGenBuffers");*/
    
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

    for(Particle p : particles)
      p.render();

    /*vector<Vector> positions = getParticlePositions();

    glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(Vector), &positions.front(), GL_STREAM_DRAW);
    //checkForError("glBufferData");

    glBindVertexArray(particleVAO);
    //checkForError("glBindVertexArray");
    glBindBuffer(GL_ARRAY_BUFFER, particleVBO);
    //checkForError("glBindBuffer");

    glDrawArrays(GL_POINTS, 0, particles.size());
    //checkForError("glDrawArrays");*/

    glfwSwapBuffers(window);
    glfwPollEvents();

    glfwMakeContextCurrent(menu);

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
        
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(fov, width/height, nearPlane, farPlane);
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);

    for(Button b : buttons)
      b.render();
    
    glfwSwapBuffers(menu);
    glfwPollEvents();

    glfwMakeContextCurrent(window);
}

void keyboardFunc(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if(action == GLFW_PRESS) {
    if(key == GLFW_KEY_ENTER)
      update();
    if(key == GLFW_KEY_SPACE)
      simulate = !simulate;
  }
}

void mouseFunc(GLFWwindow* window, int button, int action, int mods) {
  if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    double x, y;
    glfwGetCursorPos(menu, &x, &y);
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

  menu = glfwCreateWindow(menuWidth, menuHeight, "Menu", NULL, NULL);
  if(!menu) {
    cout << "menu failed to be created" << endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  
	glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, keyboardFunc);
  glfwSetMouseButtonCallback(menu, mouseFunc);

	init();
  initButtons();

  cout << "about to start loop" << endl;
	while(!glfwWindowShouldClose(window)) {
    while(simulate) {
      update();
    }

    render();
  }
  
  cleanUp();
	
	return 0;
}

