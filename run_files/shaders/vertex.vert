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
	vec3 position = vec3( modelViewMatrix * vec4( vPosition, 1.0 ) );
	vec3 normal = normalize( normalMatrix * vNormal );
	vec3 light = normalize( vec3( lightPos ) );
	float Normal_Light = max(dot(normal, light), 0.0);

	color = lightDiffuse * vColor * min(1,(Normal_Light + 0.4));
    gl_Position = projectionMatrix * modelViewMatrix * vec4( vPosition, 1.0 );
}