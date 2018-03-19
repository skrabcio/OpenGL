#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <vector>

#include "shaders.h"
#define M_PI 3.141592

const int WIDTH = 512; // szerokosc okna
const int HEIGHT = 512; // wysokosc okna
const int VAOS = 1; // liczba VAO
const int VBOS = 2; // liczba VBO

const int numOfVertices = 5; //liczba wierzcholkow
const float radius = 0.4f; // promien figury

void onShutdown();
void initGL();
void renderScene();
void setupShaders();
void drawPolygons( int numOfVertices, float radius);

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
	atexit( onShutdown );

    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA );

	glutInitContextVersion( 3, 1 ); // inicjacja wersji kontekstu
	glutInitContextFlags( GLUT_DEBUG );
	glutInitContextProfile( GLUT_CORE_PROFILE ); // incicjacja profilu rdzennego

	glutSetOption( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS );

    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( WIDTH, HEIGHT );
    glutCreateWindow( "OpenGL - Core Profile" );

	// inicjacja GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if( err != GLEW_OK )	
	{
		std::cerr << "Blad: " << glewGetErrorString( err ) << std::endl;
		exit( 1 );
	}

	if( !GLEW_VERSION_3_1 )
	{
		std::cerr << "Brak obslugi OpenGL 3.1\n";
		exit( 2 );
	}
    
    glutDisplayFunc( renderScene );

	initGL();

    glutMainLoop();
    
    return 0;
}

/*------------------------------------------------------------------------------------------
** funkcja wykonywana przed zamknieciem programu
**------------------------------------------------------------------------------------------*/
void onShutdown()
{
	glDeleteBuffers( VBOS, buffers );  // usuniecie VBO
	glDeleteVertexArrays( VAOS, vao ); // usuiecie VAO
	glDeleteProgram( shaderProgram ); // usuniecie programu cieniowania
}

/*------------------------------------------------------------------------------------------
** funkcja inicjujaca ustawienia OpenGL
**------------------------------------------------------------------------------------------*/
void initGL()
{
	std::cout << "GLEW = "		  << glewGetString( GLEW_VERSION ) << std::endl;
	std::cout << "GL_VENDOR = "	  << glGetString( GL_VENDOR ) << std::endl;
	std::cout << "GL_RENDERER = " << glGetString( GL_RENDERER ) << std::endl;
	std::cout << "GL_VERSION = "  << glGetString( GL_VERSION ) << std::endl;
	std::cout << "GLSL = "		  << glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl;

	glClearColor( 0.1f, 0.2f, 0.3f, 1.0f ); // kolor uzywany do czyszczenia bufora koloru

	setupShaders();

	drawPolygons( numOfVertices, radius );
}

/*------------------------------------------------------------------------------------------
** funkcja rysujaca scene
**------------------------------------------------------------------------------------------*/
void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT); // czyszczenie bufora koloru

	glUseProgram(shaderProgram); // wlaczenie programu cieniowania

	GLint position = glGetUniformLocation(shaderProgram, "colors"); // pobranie lokalizacji obiektu
	glUniform3f(position, 1.0, 1.0, 0.0); // ustalenie koloru w zmiennej "colors" dla podanej lokalizacji

	// wyrysowanie VAO
	glBindVertexArray(vao[0]);
    glDrawArrays( GL_TRIANGLE_FAN, 0, numOfVertices );

	glBindVertexArray(0);

    glutSwapBuffers();
}

/*------------------------------------------------------------------------------------------
** funkcja tworzaca program cieniowania skladajacy sie z shadera wierzcholkow i fragmentow
**------------------------------------------------------------------------------------------*/
void setupShaders()
{
	if( !setupShaders( "shaders/vertex.vert", "shaders/fragment.frag", shaderProgram ) )
		exit( 3 );
 
    vertexLoc = glGetAttribLocation( shaderProgram, "vPosition" );
    colorLoc = glGetAttribLocation( shaderProgram, "vColor" );
}

/*------------------------------------------------------------------------------------------
** funkcja inicjujaca VAO oraz zawarte w nim VBO z danymi o trojkacie 
**------------------------------------------------------------------------------------------*/
void drawPolygons(int numOfVertices, float radius)
{

	// wspolrzedne wierzcholkow wielokata
	float vertices[400];
	int arrayBuffor = 0;

	for (int i = 0; i < numOfVertices; i++) {

		arrayBuffor = i * 4;
		//obliczanie wspolrzednych wierzcholkow wielokata
		vertices[arrayBuffor + 0] = radius * cos((M_PI*i * 2) / numOfVertices);
		vertices[arrayBuffor + 1] = radius * sin((M_PI*i * 2) / numOfVertices);
		vertices[arrayBuffor + 2] = 0.0f;
		vertices[arrayBuffor + 3] = 1.0f;
	}
	
    
	glGenVertexArrays( 1, vao );
	glGenBuffers( 1, buffers );

	glBindVertexArray( VAOS );
 
    // VBO dla kolorow
    glBindBuffer( GL_ARRAY_BUFFER, VBOS );
	glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );

    glEnableVertexAttribArray( vertexLoc ); // wlaczenie tablicy atrybutu wierzcholkow
    glVertexAttribPointer( vertexLoc, 4, GL_FLOAT, GL_FALSE, 0, 0 ); // zdefiniowanie danych tablicy atrybutu wierzcholkow

	
	glBindVertexArray( 0 );
}