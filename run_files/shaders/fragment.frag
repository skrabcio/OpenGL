#version 140

in vec3 color; // kolor wej.
out vec4 fColor; // kolor wyj.
 
void main()
{
    fColor = vec4( color, 1.0 );
}