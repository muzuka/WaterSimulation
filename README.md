# WaterSimulation
Implementation of Smooth Particle Hydrodynamic water physics using obj files as input obstacles.

## Controls

* SPACE key -> toggle animation
* ENTER key -> step through animation when paused
* ARROW keys -> rotate object

* W key -> loads waterfall scene
* E key -> loads funnel scene
* R key -> loads stirring scene

## Getting Started

These instructions will help with running the program.

Use this as the base compile instruction

~~~
g++ -o main main.cpp Vector.cpp Particle.cpp Model.cpp Triangle.cpp -std=c++0x
~~~

OpenGL has platform specific flags so try these anywhere around "-o main" and after g++.

For Mac:
~~~
-framework Opengl
~~~

For Linux:
~~~
-lGL
~~~

For Windows:
~~~
-lopengl32
~~~

You will need GLFW installed to run this program.
Just add the following if you have GLFW installed.
~~~
-lglfw3
~~~

You will also need GLEW so be sure to include it.
~~~
-lGLEW
~~~
