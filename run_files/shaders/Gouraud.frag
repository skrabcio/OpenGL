#version 140

in vec3 color; // kolor pobierany z shadera wierzcho�k�w
out vec4 fColor; // wyj�ciowy kolor
 
void main()
{
    fColor = vec4( color, 1.0 );
}