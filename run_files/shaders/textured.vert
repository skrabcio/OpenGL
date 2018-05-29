#version 140

#extension GL_ARB_explicit_attrib_location : enable

uniform mat4 modelViewMatrix; // macierz model-widok
uniform mat4 viewMatrix; // macierz widoku
uniform mat3 normalMatrix; // macierz do transformacji normala
uniform mat4 projectionMatrix; // macierz projekcji
 
layout (location = 0) in vec3 vPosition; // pozycja wierzcholka w ukladzie modelu
layout (location = 1) in vec3 vNormal; // normal
layout (location = 2) in vec2 vTexCoord; // wspolrzedne tekstury

out vec3 position; // pozycja wierzcholka w ukladzie swiata
out vec3 normal; // normal w ukladzie swiata
out vec2 textureCord; // wspolrzedne tesktury
 
void main()
{
	position = vec3( modelViewMatrix * vec4( vPosition, 1.0 ) );
	normal = normalize( normalMatrix * vNormal );
	textureCord = vTexCoord;
	
	gl_Position = projectionMatrix * modelViewMatrix * vec4( vPosition, 1.0 );
}