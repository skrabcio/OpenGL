#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "shaders.h"
#include <vector>
using namespace std;

const int WIDTH = 768;
const int HEIGHT = 576;
int type = 0;

void onShutdown();
void initGL();
void renderScene();
void keyboard(unsigned char key, int x, int y);
void setupShaders();
void setupBuffers();

//******************************************************************************************
GLuint vao[2]; // identyfikator VAO
GLuint buffers[2]; // identyfikatory VBO

GLuint shaderProgram; // identyfikator programu cieniowania

GLuint vertexLoc; // lokalizacja atrybutu wierzcholka - wspolrzedne wierzcholka
GLuint colorLoc;


/*------------------------------------------------------------------------------------------
** funkcja glowna
**------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	atexit(onShutdown);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	glutInitContextVersion(3, 1);
	glutInitContextFlags(GLUT_DEBUG);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("OpenGL (Core Profile) - Transformations");

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cerr << "Blad: " << glewGetErrorString(err) << std::endl;
		exit(1);
	}

	if (!GLEW_VERSION_3_1)
	{
		std::cerr << "Brak obslugi OpenGL 3.1\n";
		exit(2);
	}

	glutReshapeFunc(changeSize);
	glutDisplayFunc(renderScene);

	glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKeys);

	initGL();

	glutMainLoop();

	return 0;
}

/*------------------------------------------------------------------------------------------
** funkcja wykonywana przed zamknieciem programu
**------------------------------------------------------------------------------------------*/
void onShutdown()
{
	glDeleteBuffers(2, buffers);
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(shaderProgram);
}

/*------------------------------------------------------------------------------------------
** funkcja wypisujaca podstawowe informacje o ustawieniach programu
**------------------------------------------------------------------------------------------*/
void printStatus()
{
	std::cout << "GLEW = " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "GL_VENDOR = " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "GL_RENDERER = " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "GL_VERSION = " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL = " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl << std::endl;
}

/*------------------------------------------------------------------------------------------
** funkcja inicjujaca ustawienia OpenGL
**------------------------------------------------------------------------------------------*/
void initGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	setupShaders();

	setupBuffers();

	printStatus();
}


/*------------------------------------------------------------------------------------------
** funkcja rysujaca scene
**------------------------------------------------------------------------------------------*/
void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgram);
	GLint lokalizacja = glGetUniformLocation(shaderProgram, "pozycja");

	
	glBindVertexArray(0);
	glutSwapBuffers();
}

/*------------------------------------------------------------------------------------------
** funkcja obslugujaca klawiature (klawiesze ASCII)
** key - nacisniety klawisz
** x, y - wspolrzedne kursora myszki w momencie nacisniecia klawisza key na klawiaturze
**------------------------------------------------------------------------------------------*/
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // ESC
		exit(0);
		break;

	case '1': // stozek
		type = 0;
		break;

	case '2': // kula
		type = 1;
		break;
	}
	glutPostRedisplay();
	renderScene();
}


/*------------------------------------------------------------------------------------------
** funkcja tworzaca program cieniowania skladajacy sie z shadera wierzcholkow i fragmentow
**------------------------------------------------------------------------------------------*/
void setupShaders()
{
	if (!setupShaders("shaders/vertex.vert", "shaders/fragment.frag", shaderProgram))
		exit(3);

	vertexLoc = glGetUniformLocation(shaderProgram, "vPosition");
	colorLoc = glGetUniformLocation(shaderProgram, "vColor");
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca VAO i VBO z czajniczkiem
**------------------------------------------------------------------------------------------*/
void setupBuffers()
{
	
}

