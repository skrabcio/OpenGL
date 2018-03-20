#version 140

uniform float angle; // zmianna globalna przechowywujaca kat o jaki ma nastapic obrot

in vec4 vPosition; // pozycja wierzcholka w lokalnym ukladzie wspolrzednych
in vec3 vColor; // kolor wierzcholka
 
out vec3 color; // kolor przekazywany do shadera fragmentow
 
void main()
{
    float newPositionX = vPosition.x * cos( radians( angle )) - vPosition.y * sin( radians( angle )); // wyliczenie nowej pozycji X wierzcholka
	float newPositionY = vPosition.x * sin( radians( angle )) + vPosition.y * cos( radians( angle )); // wyliczenie nowej pozycji Y wierzcholka

	vec4 newPosition = vec4( newPositionX, newPositionY, vPosition.z, vPosition.w); // wpisanie nowych wspolrzednych wierzcholka do wektora

	color = vColor;
    gl_Position = newPosition; // przekazanie nowych wspolrzednych
}