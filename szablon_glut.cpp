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
const float ROT_STEP = 10.0f;
int model = 0;

void onShutdown();
void printStatus();
void initGL();
void changeSize(GLsizei w, GLsizei h);
void updateProjectionMatrix();
void renderScene();
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void setupShaders();
void setupBuffers();

//******************************************************************************************
GLuint vao; // identyfikator VAO
GLuint buffers[2]; // identyfikatory VBO

GLuint shaderProgram; // identyfikator programu cieniowania

GLuint vertexLoc; // lokalizacja atrybutu wierzcholka - wspolrzedne wierzcholka

GLuint projMatrixLoc; // lokalizacja zmiennej jednorodnej - macierz projekcji
GLuint mvMatrixLoc; // lokalizacja zmiennej jednorodnej - macierz model-widok

glm::mat4 projMatrix; // macierz projekcji
glm::mat4 mvMatrix; // macierz model-widok

bool wireframe = true; // czy rysowac siatke (true) czy wypelnienie (false)
glm::vec3 rotationAngles = glm::vec3(0.0, 0.0, 0.0); // katy rotacji wokol poszczegolnych osi
float fovy = 45.0f; // kat patrzenia (uzywany do skalowania sceny)
float aspectRatio = (float)WIDTH / HEIGHT;

unsigned int renderElements = 0; // liczba elementow do wyrysowania
								 //******************************************************************************************

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
** funkcja wywolywana przy zmianie rozmiaru okna
** w - aktualna szerokosc okna
** h - aktualna wysokosc okna
**------------------------------------------------------------------------------------------*/
void changeSize(GLsizei w, GLsizei h)
{
	if (h == 0)
		h = 1;
	glViewport(0, 0, w, h);

	aspectRatio = (float)w / h;

	updateProjectionMatrix();
}

/*------------------------------------------------------------------------------------------
** funkcja aktualizuje macierz projekcji
**------------------------------------------------------------------------------------------*/
void updateProjectionMatrix()
{
	projMatrix = glm::perspective(glm::radians(fovy), aspectRatio, 0.1f, 100.0f);
}

/*------------------------------------------------------------------------------------------
** funkcja rysujaca scene
**------------------------------------------------------------------------------------------*/
void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mvMatrix = glm::lookAt(glm::vec3(0, 0, 8), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

	glUseProgram(shaderProgram);
	glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	mvMatrix *= glm::rotate(glm::radians(rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
	mvMatrix *= glm::rotate(glm::radians(rotationAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
	mvMatrix *= glm::rotate(glm::radians(rotationAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));

	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));

	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, renderElements, GL_UNSIGNED_INT, 0);
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

	case 'a':
		rotationAngles.x -= ROT_STEP;
		if (rotationAngles.x < 0.0f)
			rotationAngles.x += 360.0f;
		break;

	case 'd':
		rotationAngles.x += ROT_STEP;
		if (rotationAngles.x > 360.0f)
			rotationAngles.x -= 360.0f;
		break;

	case 's':
		rotationAngles.y -= ROT_STEP;
		if (rotationAngles.y < 0.0f)
			rotationAngles.y += 360.0f;
		break;

	case 'w':
		rotationAngles.y += ROT_STEP;
		if (rotationAngles.y > 360.0f)
			rotationAngles.y -= 360.0f;
		break;

	case 'z':
		rotationAngles.z -= ROT_STEP;
		if (rotationAngles.z < 0.0f)
			rotationAngles.z += 360.f;
		break;

	case 'q':
		rotationAngles.z += ROT_STEP;
		if (rotationAngles.z > 360.0f)
			rotationAngles.z -= 360.0f;
		break;

	case '+':
	case '=':
		fovy /= 1.1;
		updateProjectionMatrix();
		break;

	case '-':
		if (1.1f * fovy < 180.0f)
		{
			fovy *= 1.1;
			updateProjectionMatrix();
		}
		break;

	case '1':
		model = 0;
		setupBuffers();
		renderScene();
		break;

	case '2':
		model = 1;
		setupBuffers();
		renderScene();
		break;
	case '3':
		model = 2;
		setupBuffers();
		renderScene();
		break;
	}
	glutPostRedisplay();
}

/*------------------------------------------------------------------------------------------
** funkcja obslugujaca klawiature (klawisze specjalne)
** key - nacisniety klawisz
** x, y - wspolrzedne kursora myszki w momencie nacisniecia klawisza key na klawiaturze
**------------------------------------------------------------------------------------------*/
void specialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F1:
		wireframe = !wireframe;
		break;

	case GLUT_KEY_F4:
		if (glutGetModifiers() == GLUT_ACTIVE_ALT)
			exit(0);
		break;
	}

	glutPostRedisplay();
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca program cieniowania skladajacy sie z shadera wierzcholkow i fragmentow
**------------------------------------------------------------------------------------------*/
void setupShaders()
{
	if (!setupShaders("shaders/vertex.vert", "shaders/fragment.frag", shaderProgram))
		exit(3);

	projMatrixLoc = glGetUniformLocation(shaderProgram, "projectionMatrix");
	mvMatrixLoc = glGetUniformLocation(shaderProgram, "modelViewMatrix");
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca VAO i VBO z czajniczkiem
**------------------------------------------------------------------------------------------*/
void setupBuffers()
{
	vector<GLfloat> vertices;
	vector<unsigned int> indexes;
	float radius = 0.5f;
	float height = 2.0f;
	int prymitive = 0;
	float angle, angle2, u, v, x, y;
	int vertices_base = 24, vertices_side = 24;

	if(model == 0)
	{
		int prymitive = (vertices_base + 2) * vertices_side;

		for (int i = 0; i < vertices_base; i++) 
		{
			if ( i == 0 )
			{
				for (int j = 0; j < vertices_side; j++) 
				{
					indexes.push_back(j);
					indexes.push_back(0);
				}
			}
			for (int j = 0; j < vertices_side; j++) 
			{
				x = (float)j;
				y = (float)i;
				u = x / (vertices_side - 1);
				v = y / (vertices_base - 1);
				angle = glm::radians(360 * u);

				vertices.push_back(radius * (1 - v) * cos(angle));
				vertices.push_back(radius * (1 - v) * sin(angle));
				vertices.push_back(height * v);
				vertices.push_back(1);

				indexes.push_back(i * vertices_side + j);
				indexes.push_back(i * vertices_side + vertices_side + j);
			}
			indexes.push_back(prymitive);
		}		
	}
	else if (model == 1)
	{
		for ( int i = 0; i < vertices_side; i++)
		{
			for (int j = 0; j < vertices_base; j++) 
			{
				x = (float)j;
				y = (float)i;

				if (i != (vertices_side - 1) && j != (vertices_base - 1)) 
				{
					indexes.push_back(i * vertices_base + vertices_base + j);
					indexes.push_back(i * vertices_base + j + 1);
					indexes.push_back(i * vertices_base + j);
					indexes.push_back(i * vertices_base + j + 1);
					indexes.push_back(i * vertices_base + vertices_base + j);
					indexes.push_back(i * vertices_base + vertices_base + j + 1 );
				}

				u = x / (vertices_base - 1);
				v = y / (vertices_side - 1);
				angle = glm::radians(360 * u);
				angle2 = glm::radians(-90 + 180 * v);

				vertices.push_back(radius * cos(angle) * cos(angle2));
				vertices.push_back(radius * sin(angle) * cos(angle2));
				vertices.push_back(radius * sin(angle2));
				vertices.push_back(1);
			}
		}
	}

	else
	{
		for (int i = 0; i < vertices_side; i++) {
			if (i == 0 || i == (vertices_side - 1)) {
				for (int j = 1; j < (vertices_base - 1); j++) {
					indexes.push_back(i * vertices_base);
					indexes.push_back(j + i * vertices_base);
					indexes.push_back(j + i * vertices_base + 1);
				}
			}
			for (int j = 0; j < vertices_base; j++) {

	
				x = (float)i; 
				y = (float)j;
				u = y / (vertices_base - 1);
				v = x / (vertices_side - 1);
				angle = glm::radians(360 * u);

				vertices.push_back(radius * cos(angle));
				vertices.push_back(radius * sin(angle));
				vertices.push_back((height * v - height / 2));
				vertices.push_back(1);

				if (i != vertices_side - 1 && j != vertices_base - 1) {
					indexes.push_back(i * vertices_base + vertices_base + j);
					indexes.push_back(i * vertices_base + j + 1);
					indexes.push_back(i * vertices_base + j);
					indexes.push_back(i * vertices_base + j + 1);
					indexes.push_back(i * vertices_base + vertices_base + j);
					indexes.push_back(i * vertices_base + vertices_base + j + 1);
				}
			}
		}

	}
	renderElements = indexes.size();
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(2, buffers);
	// VBO dla wierzcholkow
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertexLoc);
	glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);

	// VBO dla indeksow
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLfloat), &indexes[0], GL_STATIC_DRAW);
	
	if (model == 0)
	{
		glEnable(GL_PRIMITIVE_RESTART);
		glPrimitiveRestartIndex(prymitive);
	}
	glBindVertexArray(0);
}

