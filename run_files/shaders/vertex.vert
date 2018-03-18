#version 140
 
in vec4 vPosition; // pozycja wierzcholka w lokalnym ukladzie wspolrzednych
in vec3 vColor; // kolor wierzcholka
 
out vec3 color; // kolor przekazywany do shadera fragmentow
 
void main()
{
    color = vColor;

    gl_Position = vPosition;
}