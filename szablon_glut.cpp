#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>

#include "shaders.h"

const int WIDTH = 512; // szerokosc okna
const int HEIGHT = 512; // wysokosc okna
const int VAOS = 2; // liczba VAO
const int VBOS = 3; // liczba VBO

void onShutdown();
void initGL();
void renderScene();
void setupShaders();
void setupBuffers();

//******************************************************************************************
GLuint shaderProgram; // identyfikator programu cieniowania

GLuint vertexLoc; // lokalizacja atrybutu wierzcholka - wspolrzedne
GLuint colorLoc; // lokalizacja atrybutu wierzcholka - kolor

GLuint vao[VAOS]; // identyfikatory VAO
GLuint buffers[VBOS]; // identyfikatory VBO
					  //******************************************************************************************

					  /*------------------------------------------------------------------------------------------
					  ** funkcja glowna
					  **------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	// rejestracja funkcji wykonywanej przy wyjsciu
	atexit(onShutdown);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitContextVersion(3, 1); // inicjacja wersji kontekstu
	glutInitContextFlags(GLUT_DEBUG);
	glutInitContextProfile(GLUT_CORE_PROFILE); // incicjacja profilu rdzennego

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutInitWindowPosition(0, 0);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("OpenGL - Core Profile");

	// inicjacja GLEW
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

	initGL();

	glutMainLoop();

	return 0;
}

/*------------------------------------------------------------------------------------------
** funkcja wykonywana przed zamknieciem programu
**------------------------------------------------------------------------------------------*/
void onShutdown()
{
	glDeleteBuffers(VBOS, buffers);  // usuniecie VBO
	glDeleteVertexArrays(VAOS, vao); // usuiecie VAO
	glDeleteProgram(shaderProgram); // usuniecie programu cieniowania
}

/*------------------------------------------------------------------------------------------
** funkcja inicjujaca ustawienia OpenGL
**------------------------------------------------------------------------------------------*/
void initGL()
{
	std::cout << "GLEW = " << glewGetString(GLEW_VERSION) << std::endl;
	std::cout << "GL_VENDOR = " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "GL_RENDERER = " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "GL_VERSION = " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL = " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // kolor uzywany do czyszczenia bufora koloru

	setupShaders();

	setupBuffers();
}

/*------------------------------------------------------------------------------------------
** funkcja rysujaca scene
**------------------------------------------------------------------------------------------*/
void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT); // czyszczenie bufora koloru

	glUseProgram(shaderProgram); // wlaczenie programu cieniowania

								 // wyrysowanie pierwszego VAO (trojkat)
	glBindVertexArray(vao[0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// wyrysowanie dugiego VAO (kwadrat)
	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);

	glutSwapBuffers();
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
** funkcja inicjujaca VAO oraz zawarte w nim VBO z danymi o trojkacie
**------------------------------------------------------------------------------------------*/
void setupBuffers()
{
	glGenVertexArrays(2, vao); // generowanie identyfikatora VAO
	glGenBuffers(3, buffers); // generowanie identyfikatorow VBO

							  // wspolrzedne wierzcholkow trojkata
	float vertices[] =
	{
		0.1f, 0.0f, 0.0f, 1.0f,
		0.9f, -0.5f, 0.0f, 1.0f,
		0.9f, 0.5f, 0.0f, 1.0f
	};

	// kolory wierzchokow trojkata
	std::vector<float> colors;
	colors.push_back(1.0f);
	colors.push_back(0.0f);
	colors.push_back(0.0f);
	colors.push_back(1.0f);

	colors.push_back(0.0f);
	colors.push_back(1.0f);
	colors.push_back(0.0f);
	colors.push_back(1.0f);

	colors.push_back(0.0f);
	colors.push_back(0.0f);
	colors.push_back(1.0f);
	colors.push_back(1.0f);

	glBindVertexArray(vao[0]); // dowiazanie pierwszego VAO    	

							   // VBO dla wspolrzednych wierzcholkow
	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vertexLoc); // wlaczenie tablicy atrybutu wierzcholka - wspolrzedne
	glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, GL_FALSE, 0, 0); // zdefiniowanie danych tablicy atrybutu wierzchoka - wspolrzedne

																   // VBO dla kolorow
	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), reinterpret_cast<GLfloat *>(&colors[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(colorLoc); // wlaczenie tablicy atrybutu wierzcholka - kolory
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0); // zdefiniowanie danych tablicy atrybutu wierzcholka - kolory

																  // wspolrzedne i kolory kwadratu (x, y, z, w, r, g, b, a)
	float verticesAndColors[] =
	{
		-0.9f, 0.4f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
		-0.9f, -0.4f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.1f, 0.4f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		-0.1f, -0.4f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f
	};

	glBindVertexArray(vao[1]);
	// VBO dla wspolrzednych i kolorow
	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesAndColors), verticesAndColors, GL_STATIC_DRAW);

	int stride = 8 * sizeof(float);
	glEnableVertexAttribArray(vertexLoc);
	glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, GL_FALSE, stride, 0); // wspolrzedne wierzcholkow
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 4, GL_FLOAT, GL_FALSE, stride, (void*)(4 * sizeof(float))); // kolory wierzcholkow

	glBindVertexArray(0);
}