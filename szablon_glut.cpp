#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

#include "shaders.h"
#include "Model.h"
#include <vector>
using namespace std;

const int WIDTH = 768;
const int HEIGHT = 576;
const float ROT_STEP = 10.0f;
const int shaders = 2;

enum Shader {Gouraud, Phong};

Model *model;

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
void importModel();

//******************************************************************************************
GLuint shaderProgram[shaders]; // identyfikator programu cieniowania

GLuint vertexLoc; // lokalizacja atrybutu wierzcholka - wspolrzedne wierzcholka
GLuint colorLoc; // lokalizacja zmiennej jednorodnej zawieraj¹cej kolor
GLuint normalLoc; // lokalizacja zmiennych normalnych


GLuint projMatrixLoc; // lokalizacja zmiennej jednorodnej - macierz projekcji
GLuint mvMatrixLoc; // lokalizacja zmiennej jednorodnej - macierz model-widok
GLuint normalMatrixLoc; // lokalizacja zmiennej jednorodnej zawierajacej macierz normali

GLuint lightAmbientLoc; // lokalizacja zmiennej jednorodnej ambientu œwiat³a
GLuint matAmbientLoc; // lokalizacja  zmiennej jednorodnej ambientu materia³u

GLuint lightPositionLoc; // lokalizacja zmiennej jednorodnej pozycji œwiat³a
GLuint lightDiffuseLoc; // lokalizacja zmiennej jednorodnej diffuse œwiat³a
GLuint matDiffuseLoc; // lokalizacja zmiennej jednorodnej diffuse materia³u

GLuint lightSpecularLoc; // lokalizacja zmiennej jednorodnej speculara œwiat³a
GLuint matSpecularLoc; // lokalizacja zmiennej jednorodnej speculara materia³u
GLuint matShineLoc; // lokalizacja zmiennej jednorodnej po³ysku materia³u

// parametry œwwiat³a
glm::vec4 lightPosition = glm::vec4(0.0f, 0.0f, 10.0f, 1.0f);
glm::vec3 lightAmbient = glm::vec3(0.2f, 0.2f, 0.2f);
glm::vec3 lightDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 lightSpecular = glm::vec3(1.0, 1.0, 1.0);

//parametry materia³u
glm::vec3 matAmbient = glm::vec3(0.0215f, 0.1745f, 0.0215f);
glm::vec3 matDiffuse = glm::vec3(0.07568f, 0.61424f, 0.07568f);
glm::vec3 matSpecular = glm::vec3(0.633f, 0.727811f, 0.633f);
float matShine = 83.2f;

glm::mat4 projMatrix; // macierz projekcji
glm::mat4 mvMatrix; // macierz model-widok

bool wireframe = false; // czy rysowac siatke (true) czy wypelnienie (false)
glm::vec3 rotationAngles = glm::vec3(0.0, 0.0, 0.0); // katy rotacji wokol poszczegolnych osi
float aspectRatio = (float)WIDTH / HEIGHT;
glm::vec3 scaleModel = glm::vec3(1.0, 1.0, 1.0); //zmienna zawieraj¹ca stopieñ skalowania 
Shader shader = Phong;

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
	glDeleteProgram(shaderProgram[1]);
	glDeleteProgram(shaderProgram[2]);
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

	importModel();

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
	projMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
}

/*------------------------------------------------------------------------------------------
** funkcja rysujaca scene
**------------------------------------------------------------------------------------------*/
void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mvMatrix = glm::lookAt(glm::vec3(0, 0, 8), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

	glUseProgram(shaderProgram[shader]);
	glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));

	glUniform3fv(lightAmbientLoc, 1, glm::value_ptr(lightAmbient));
	glUniform3fv(matAmbientLoc, 1, glm::value_ptr(matAmbient));

	glUniform3fv(lightDiffuseLoc, 1, glm::value_ptr(lightDiffuse));
	glUniform3fv(matDiffuseLoc, 1, glm::value_ptr(matDiffuse));

	glUniform3fv(lightSpecularLoc, 1, glm::value_ptr(lightSpecular));
	glUniform3fv(matSpecularLoc, 1, glm::value_ptr(matSpecular));
	glUniform1f(matShineLoc, matShine);


	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	mvMatrix *= glm::scale(scaleModel);
	mvMatrix *= glm::rotate(glm::radians(rotationAngles.z), glm::vec3(0.0f, 0.0f, 1.0f));
	mvMatrix *= glm::rotate(glm::radians(rotationAngles.y), glm::vec3(0.0f, 1.0f, 0.0f));
	mvMatrix *= glm::rotate(glm::radians(rotationAngles.x), glm::vec3(1.0f, 0.0f, 0.0f));

	glUniformMatrix4fv(mvMatrixLoc, 1, GL_FALSE, glm::value_ptr(mvMatrix));
	glm::mat3 normalMatrix = glm::inverseTranspose(glm::mat3(mvMatrix));

	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	model->render();
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
	case '=': //skalowanie modelu w górê
		scaleModel += 0.1;
		updateProjectionMatrix();
		break;

	case '-': // skalowanie modelu w dó³
		if (scaleModel.x > 0.1f)
			scaleModel -= 0.1;
		updateProjectionMatrix();
		break;
	case '1':
		shader = Gouraud;
		setupShaders();
		break;

	case '2':
		shader = Phong;
		setupShaders();
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
	if (!setupShaders("shaders/Gouraud.vert", "shaders/Gouraud.frag", shaderProgram[Gouraud]))
		exit(3);
	if (!setupShaders("shaders/Phong.vert", "shaders/Phong.frag", shaderProgram[Phong]))
		exit(3);

	projMatrixLoc = glGetUniformLocation(shaderProgram[shader], "projectionMatrix");
	mvMatrixLoc = glGetUniformLocation(shaderProgram[shader], "modelViewMatrix");
	normalMatrixLoc = glGetUniformLocation(shaderProgram[shader], "normalMatrix");

	lightAmbientLoc = glGetUniformLocation(shaderProgram[shader], "lightAmbient");
	matAmbientLoc = glGetUniformLocation(shaderProgram[shader], "matAmbient");

	lightPositionLoc = glGetUniformLocation(shaderProgram[shader], "lightPosition");
	lightDiffuseLoc = glGetUniformLocation(shaderProgram[shader], "lightDiffuse");
	matDiffuseLoc = glGetUniformLocation(shaderProgram[shader], "matDiffuse");

	lightSpecularLoc = glGetUniformLocation(shaderProgram[shader], "lightSpecular");
	matSpecularLoc = glGetUniformLocation(shaderProgram[shader], "matSpecular");
	matShineLoc = glGetUniformLocation(shaderProgram[shader], "matShine");
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca VAO i VBO z czajniczkiem
**------------------------------------------------------------------------------------------*/
void importModel()
{
	model = new Model(".\\models\\cow.obj");
}