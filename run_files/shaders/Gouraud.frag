#version 140

in vec3 color; // kolor pobierany z shadera wierzcho³ków
out vec4 fColor; // wyjœciowy kolor
 
void main()
{
    fColor = vec4( color, 1.0 );
}