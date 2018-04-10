#version 140
 
 in vec4 vPosition; // pozycja wierzcholka w lokalnym ukladzie wspolrzednych
 in vec3 vColor;

 out vec3 color;

 uniform mat4 position;

void main()
{
	color = vColor;

    gl_Position = position * vPosition;
}