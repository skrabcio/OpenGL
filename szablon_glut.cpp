#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "shaders.h"

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

	
	glutDisplayFunc(renderScene);
	glutKeyboardFunc(keyboard);

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
	glDeleteVertexArrays(2, vao);
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
	GLint location = glGetUniformLocation(shaderProgram, "position");

	float angle;
	
	if (type != 0) angle = 0.0f;
	else angle = 45.0f;

	for (int i = 0; i < 5; i++) 
	{
		for (int j = 0; j < 5; j++)
		{
			for (int k = 0; k < 4; k++)
			{
				glm::mat4 transform;
				transform = glm::translate(transform, glm::vec3(-0.80f + (j * 0.4f), 0.80f - (i * 0.4f), 0.0f));
				transform = glm::rotate(transform, glm::radians(angle + (k*90.0f)), glm::vec3(0.0f, 0.0f, 1.0f));
				transform = glm::scale(transform, glm::vec3(0.2f, 0.2f, 0.2f));
				transform = glm::translate(transform, glm::vec3(0.0f, -0.75f, 0.0f));

				glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
				glBindVertexArray(vao[0]);
				glDrawArrays(GL_TRIANGLES, 0, 3);
			}

			glm::mat4 transform;
			transform = glm::translate(transform, glm::vec3(-0.80f + (j * 0.4f), 0.80f - (i * 0.4f), 0.0f));

			if (type == 0) transform = glm::rotate(transform, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

			transform = glm::scale(transform, glm::vec3(0.2f, 0.2f, 0.2f));
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(transform));
			glBindVertexArray(vao[1]);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
	}
	
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

	vertexLoc = glGetAttribLocation(shaderProgram, "vPosition");
	colorLoc = glGetAttribLocation(shaderProgram, "vColor");
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca VAO i VBO z czajniczkiem
**------------------------------------------------------------------------------------------*/
void setupBuffers()
{
	glGenVertexArrays(2, vao);
	glGenBuffers(2, buffers);
	int space = 8 * sizeof(float);

	float triangle[] = 
	{
		0.0f, 0.25f, 0.0f, 1.0f, 0.0f, 0.2f, 0.5f, 1.0f,
		-0.25f, -0.25f, 0.0f, 1.0f, 0.0f, 0.2f, 0.5f, 1.0f,
		0.25f, -0.25f, 0.0f, 1.0f, 0.0f, 0.2f, 0.5f, 1.0f,
	};

	glBindVertexArray(vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

	glEnableVertexAttribArray(vertexLoc);
	glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, GL_FALSE, space, 0);
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, space, (void*)(4 * sizeof(float)));

	float square[] = 
	{
		-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f
	};

	glBindVertexArray(vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);

	glEnableVertexAttribArray(vertexLoc);
	glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, GL_FALSE, space, 0); 
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, space, (void*)(4 * sizeof(float)));

	glBindVertexArray(0);
}

