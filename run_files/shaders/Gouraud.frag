#version 140

in vec3 color; // kolor pobierany z shadera wierzchołków
out vec4 fColor; // wyjściowy kolor
 
void main()
{
    fColor = vec4( color, 1.0 );
}