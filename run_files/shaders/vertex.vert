#version 140
 
 uniform vec3 colors; // zmienna zawieraj¹ca kolor

in vec4 vPosition; // pozycja wierzcholka w lokalnym ukladzie wspolrzednych
 
out vec3 color; // kolor przekazywany do shadera fragmentow
 
void main()
{
    color = colors;

    gl_Position = vPosition;
}