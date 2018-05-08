#version 140
#extension GL_ARB_explicit_attrib_location : enable
 
uniform mat4 modelViewMatrix; // macierz model-widok
uniform mat3 normalMatrix; // macierz normala
uniform mat4 projectionMatrix; // macierz projekcji
 
layout (location = 0) in vec3 vPosition; // pozycja wierzcho³ka w lokalnym uk³adzie wspó³rzêdnych
layout (location = 1) in vec3 vNormal; // informacje o normalu

out vec3 position; // pozycja wierzcho³ka w modelu widoku
out vec3 normal; // normal 
 
void main()
{

	position = vec3( modelViewMatrix * vec4( vPosition, 1.0 ) );	//transformacja do macierzy widoku
	normal = normalize( normalMatrix * vNormal ); // normalizacja 
    gl_Position = projectionMatrix * modelViewMatrix * vec4( vPosition, 1.0 ); 
}