#version 140

#extension GL_ARB_explicit_attrib_location : enable
 
uniform mat4 modelViewMatrix; // macierz widoku
uniform mat3 normalMatrix; // macierz normali
uniform mat4 projectionMatrix; // macierz projekcji

uniform vec4 lightPos; //zmienne przechowuj¹ce informacje o œwietle 
uniform vec3 lightDiffuse;
 
layout (location = 0) in vec3 vPosition; // wierzcho³ek
layout (location = 1) in vec3 vNormal; // normal
layout (location = 2) in vec3 vColor; // kolor

out vec3 color; // kolor
 
void main()
{
	vec3 position = vec3( modelViewMatrix * vec4( vPosition, 1.0 ) ); //wyliczenie pozycji
	vec3 normal = normalize( normalMatrix * vNormal ); // wyliczenie normala
	vec3 light = normalize( vec3( lightPos ) ); //wyliczenie œwiat³a
	float Normal_Light = max(dot(normal, light), 0.0); // wyliczenie wyznacznika normal-œwiat³o

	color = lightDiffuse * vColor * min(1,(Normal_Light + 0.4)); // wyliczenie koloru
    gl_Position = projectionMatrix * modelViewMatrix * vec4( vPosition, 1.0 ); //przekazanie wspó³rzêdnych wierzcholków
}