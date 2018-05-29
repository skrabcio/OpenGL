#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <IL/il.h>

#include <iostream>

#include "shaders.h"

#include <vector>
using namespace std;

const int WIDTH = 768;
const int HEIGHT = 576;
const float ROT_STEP = 10.0f;

// struktura danych obrazka
struct ImageStat
{
	ILubyte* bytes;
	int width;
	int height;
};

ImageStat* Image(const wchar_t* filename);

void onShutdown();
void printStatus();
void initGL();
void changeSize(GLsizei w, GLsizei h);
void updateProjectionMatrix();
void renderScene();
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void setupShaders();
void terrain();


GLfloat heightMulti = 40;
GLuint vao;
GLuint vbo[4];
GLuint elements;

//******************************************************************************************
GLuint shaderProgram; // identyfikator programu cieniowania

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
glm::vec3 scaleModel = glm::vec3(0.03, 0.03, 0.03); //zmienna zawieraj¹ca stopieñ skalowania 

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
	terrain();
	

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

	glUseProgram(shaderProgram);
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
	
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLE_STRIP, elements, GL_UNSIGNED_INT, 0);
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

	case 'w':
		rotationAngles.x -= ROT_STEP;
		if (rotationAngles.x < 0.0f)
			rotationAngles.x += 360.0f;
		break;

	case 's':
		rotationAngles.x += ROT_STEP;
		if (rotationAngles.x > 360.0f)
			rotationAngles.x -= 360.0f;
		break;

	case 'a':
		rotationAngles.y -= ROT_STEP;
		if (rotationAngles.y < 0.0f)
			rotationAngles.y += 360.0f;
		break;

	case 'd':
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
		scaleModel += 0.02;
		updateProjectionMatrix();
		break;

	case '-': // skalowanie modelu w dó³
		if (scaleModel.x > 0.03f)
			scaleModel -= 0.02;
		updateProjectionMatrix();
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
	normalMatrixLoc = glGetUniformLocation(shaderProgram, "normalMatrix");

	lightPositionLoc = glGetUniformLocation(shaderProgram, "lightPosition");
	lightDiffuseLoc = glGetUniformLocation(shaderProgram, "lightDiffuse");
	
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca VAO i VBO z czajniczkiem
**------------------------------------------------------------------------------------------*/
void terrain()
{
	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	ImageStat* tData = Image((const wchar_t*)(L"terrain.png"));
	ImageStat* cData = Image((const wchar_t*)(L"colormap.png"));

	vector<GLfloat> vertices; //kontener na wierzcho³ki
	vector<GLfloat> normals; //kontener na normale
	vector<GLuint> index;	//kontener na indeksy
	vector<GLfloat> colors; // kontener na kolory

	int primitive= tData->width * tData->height;
	int vertex;

	for (int y = 0; y < tData->height; y++)
	{
		for (int x = 0; x < tData->width; x++)
		{
			//generowanie wierzcho³ków
			vertex = x + y * tData->width;

			vertices.push_back(x - tData->width / 2);
			vertices.push_back(y - tData->height / 2);
			vertices.push_back((GLfloat)tData->bytes[vertex * 3] / 255 * heightMulti);

			//generowanie kolorów
			int colorIndex = ((GLfloat)tData->bytes[vertex * 3] / 255) * cData->width;

			colors.push_back(cData->bytes[colorIndex * 3] / 255.0f);
			colors.push_back(cData->bytes[colorIndex * 3 + 1] / 255.0f);
			colors.push_back(cData->bytes[colorIndex * 3 + 2] / 255.0f);

			//generowanie krawêdzi
			if (y != tData->height - 1)
			{
				
				index.push_back(x + y * tData->width);
				index.push_back(x + (y + 1) * tData->height);
			}
		}
		index.push_back(primitive);
	}
	
	for (int y = 0; y < tData->height; y++)
	{
		for (int x = 0; x < tData->width; x++)
		{
			// obliczenia normali oraz przes³anie ich do odpowiednich kontenerów
			int verts = 5;
			if (x == 0) verts--;
			if (x == tData->height - 1) verts--;
			if (y == 0) verts--;
			if (y == tData->width - 1) verts--;

			GLfloat vertX = vertices[(x + y * tData->width) * 3] +
				(x == 0 ? 0 : vertices[((x - 1) + y * tData->width) * 3]) +
				(x == tData->height - 1 ? 0 : vertices[((x + 1) + y * tData->width) * 3]) +
				(y == 0 ? 0 : vertices[(x + (y - 1) * tData->width) * 3]) +
				(y == tData->width - 1 ? 0 : vertices[(x + (y + 1) * tData->width) * 3]);

			GLfloat vertY = vertices[(x + y * tData->width) * 3 + 1] +
				(x == 0 ? 0 : vertices[((x - 1) + y * tData->width) * 3 + 1]) +
				(x == tData->height - 1 ? 0 : vertices[((x + 1) + y * tData->width) * 3 + 1]) +
				(y == 0 ? 0 : vertices[(x + (y - 1) * tData->width) * 3 + 1]) +
				(y == tData->width - 1 ? 0 : vertices[(x + (y + 1) * tData->width) * 3 + 1]);

			GLfloat vertZ = vertices[(x + y * tData->width) * 3 + 2] +
				(x == 0 ? 0 : vertices[((x - 1) + y * tData->width) * 3 + 2]) +
				(x == tData->height - 1 ? 0 : vertices[((x + 1) + y * tData->width) * 3 + 2]) +
				(y == 0 ? 0 : vertices[(x + (y - 1) * tData->width) * 3 + 2]) +
				(y == tData->width - 1 ? 0 : vertices[(x + (y + 1) * tData->width) * 3 + 2]);

			normals.push_back(vertX / verts);
			normals.push_back(vertY / verts);
			normals.push_back(vertZ / verts);
		}
	}
	// wyliczenie iloœci elementów do renderowania
	elements = index.size() * 3;
	
	glGenVertexArrays(1, &vao);
	glGenBuffers(4, vbo);
	glBindVertexArray(vao);

	// przekazywanie wierzcho³ków
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// przekazywanie normali
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(GLfloat), &normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// przekazywanie kolorów
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(GLfloat), &colors[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// przekazanie indeksów
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(GLuint), &index[0], GL_STATIC_DRAW);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(primitive);
	
	glBindVertexArray(0);
}

// wczytywanie danych z obrazka i jego generowanie
ImageStat* Image(const wchar_t* filename)
{
	ImageStat* data = new ImageStat();
	ILuint imageName;
	ilGenImages(1, &imageName);
	ilBindImage(imageName);
	
	if (!ilLoadImage(filename))
	{
		ilBindImage(0);
		ilDeleteImages(1, &imageName);
		exit(1);
	}
	
	data->width = ilGetInteger(IL_IMAGE_WIDTH);
	data->height = ilGetInteger(IL_IMAGE_HEIGHT);
	data->bytes = new ILubyte[data->width * data->height * 3];

	ilCopyPixels(0, 0, 0, data->width, data->height, 1, IL_RGB, IL_UNSIGNED_BYTE, data->bytes);

	ilBindImage(0);
	ilDeleteImages(1, &imageName);

	return data;
}
