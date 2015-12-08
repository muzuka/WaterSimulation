

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
#include "Button.h"
#include "Simulation.h"
#include "Triangle.h"
#include "Model.h"

using namespace std;

GLFWwindow* menu;
GLFWwindow* window;

Model mesh;
vector<Particle> particles;
vector<Button> buttons;

Simulation sim    = SHOWER;
int pointXStart   = 0;
int pointYStart   = 0;
int pointZStart   = 0;
int pointHeight   = 5;
int pointWidth    = 5;
int pointDepth    = 5;
int numOfPoints   = 10;
bool simulate     = false;
bool debug        = false;
double sigma      = 0.42f;
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
unsigned int buttonShaderProgram;
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
  
const char* buttonVertShaderText = 
  "#version 120\n"
  "void main() {"
  "gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;"
  "}";
  
const char* buttonFragShaderText =
  "#version 120\n"
  "uniform vec3 color;"
  "void main() {"
  "gl_FragColor = vec4(color, 1.0f);"
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
    result += kernel(particles[i].getPosition() - particles[j].getPosition());
  }
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
  zoom = -5.0f;
  rotationX = 50.0f;
  
  for(int i = 0; i < pointWidth; i++) {
    for(int j = 0; j < pointHeight; j++) {
      particles.push_back(Particle(Vector(i/10.0f, 0.9f - (j/10.0f), 0.0f)));
    }
  }
  mesh = Model("CUP.obj");
}

void initShower() {
  zoom = -5.0f;
  rotationX = 30.0f;
  
  for(int i = 0; i < pointWidth; i++)
      particles.push_back(Particle(Vector(i/10.0f, 0.9f, 0.0f)));
  mesh = Model("SHOWER.obj");
}

void initWaterfall() {
  zoom = -4.0f;
  rotationX = 30.0f;
  rotationY = -40.0f;
  
  for(int i = 0; i < pointWidth; i++) {
    for(int j = 0; j < pointHeight; j++) {
      for(int k = 0; k < pointDepth; k++) {
        particles.push_back(Particle(Vector(i/10.0f, 0.9f + (j/10.0f), (k/10.0f))));
      }
    }
  }
  mesh = Model("WATERFALL.obj");
}

void initFunnel() {
  zoom = -3.0f;
  rotationX = 60.0f;
  
  for(int i = 0; i < pointWidth; i++) {
    for(int j = 0; j < pointHeight; j++) {
      particles.push_back(Particle(Vector(i/10.0f, 0.9f - (j/10.0f), 0.0f)));
    }
  }
  mesh = Model("FUNNEL.obj");
}

void initStirring() {
  zoom = -3.0f;
  rotationX = 50.0f;
  
  for(int i = 0; i < pointWidth; i++) {
    for(int j = 0; j < pointHeight; j++) {
      for(int k = 0; k < pointDepth; k++) {
        particles.push_back(Particle(Vector(i/10.0f, 0.9f - (j/10.0f), (k/10.0f))));
      } 
    }
  }
  mesh = Model("CUP.obj");
}

// sets button positions.
void initButtons() {
  double z = 0.0f;
  double top = -0.5f;
  double rightSide = -0.5f;
  double leftSide = -1.0f;
  buttons = vector<Button>();

  buttons.push_back(Button(CUP, Vector(leftSide, top - 0.1f, z), Vector(rightSide, top, z)));
  buttons[0].setColor(Vector(1.0f, 0.0f, 0.0f));
  buttons.push_back(Button(SHOWER, Vector(leftSide, top - 0.2f, z), Vector(rightSide, top - 0.1f, z)));
  buttons[1].setColor(Vector(0.0f, 1.0f, 0.0f));
  buttons.push_back(Button(WATERFALL, Vector(leftSide, top - 0.3f, z), Vector(rightSide, top - 0.2f, z)));
  buttons[2].setColor(Vector(0.0f, 0.0f, 1.0f));
  buttons.push_back(Button(FUNNEL, Vector(leftSide, top - 0.4f, z), Vector(rightSide, top - 0.3f, z)));
  buttons[3].setColor(Vector(1.0f, 1.0f, 0.0f));
  buttons.push_back(Button(STIRRING, Vector(leftSide, top - 0.5f, z), Vector(rightSide, top - 0.4f, z)));
  buttons[4].setColor(Vector(1.0f, 0.0f, 1.0f));
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

    shaderProgram = loadShaders(vertexShaderText, fragmentShaderText);
    buttonShaderProgram = loadShaders(buttonVertShaderText, buttonFragShaderText);
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
    double bounce = 0.0f;
    double slide  = 1.0f;

    // detect collision and change velocity
    oldPos = particles[i].getPosition();
    newPos = oldPos + particles[i].getVelocity();
    for(Triangle t : mesh.getMesh()) {
      if(t.intersect(oldPos, newPos)) {
        // process collision
        //cout << "Collision!" << endl;
        //t.getI().print();
        //t.getJ().print();
        //t.getK().print();
        //t.getNormal().print();
        //oldPos.print();
        //newPos.print();
        //t.getCollision().print();
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
    if(debug) {
      cout << i << ": acceleration ";
      particles[i].getAcceleration().print();
    }
  }
  // move points
  for(int i = 0; i < numOfPoints; i++) {
    particles[i].setVelocity(particles[i].getVelocity() + (particles[i].getAcceleration() * timeStep));
    
    checkCollision(i);

    particles[i].setPosition(particles[i].getPosition() + (particles[i].getVelocity() * timeStep));
    if(debug) {
      cout << i << ": velocity ";
      particles[i].getVelocity().print();
    }
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

    // MENU Rendering Start ############################################
    glfwMakeContextCurrent(menu);

    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
        
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f);
    
    int colorUniformLoc;

    glUseProgram(buttonShaderProgram);

    for(Button b : buttons) {
      colorUniformLoc = glGetUniformLocation(buttonShaderProgram, "color");
      checkForError("glGetUniformLocation");
      glUniform3f(colorUniformLoc, b.getColor().getX(), b.getColor().getY(), b.getColor().getZ());
      checkForError("glUniform3f");
      b.render();
    }
    
    glfwSwapBuffers(menu);
    glfwPollEvents();

    glfwMakeContextCurrent(window);
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

void mouseFunc(GLFWwindow* window, int button, int action, int mods) {
  double x, y;

  if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    glfwGetCursorPos(menu, &x, &y);
    cout << "mouseFunc called at " << x << " " << y << endl;
    
    for(Button b : buttons) {
      if(b.inside(1.0f/x, 1.0f/y)) {
        cout << "button pressed" << endl;
        sim = b.getSim();
        init();
      }
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

  menu = glfwCreateWindow(menuWidth, menuHeight, "Menu", NULL, NULL);
  if(!menu) {
    cout << "menu failed to be created" << endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  
	glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, keyboardFunc);
  glfwSetScrollCallback(window, scrollFunc);
  glfwSetMouseButtonCallback(menu, mouseFunc);

	init();
  initButtons();

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

