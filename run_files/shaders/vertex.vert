#version 140
 
in vec4 vPosition; // pozycja wierzcholka w lokalnym ukladzie wspolrzednych

out vec3 color; // kolor przekazywany do shadera fragmentow
 
void main()
{
  if( vPosition.y < -0.33 ) // gorny wiersz
  {
	if( vPosition.x < -0.33 ) color = vec3( 1.0, 0.0, 0.0 ); //pierwsza kolumna
	else if( vPosition.x > -0.33 && vPosition.x < 0.33 ) color = vec3( 1.0, 1.0, 0.0 ); // druga kolumna
	else color = vec3( 1.0, 1.0, 1.0 ); // trzecia kolumna
  }
  else if ( vPosition.y > -0.33 && vPosition.y < 0.33 )  // srodkowy wiersz
  {
	if( vPosition.x < -0.33 ) color = vec3( 0.0, 1.0, 0.0 ); //pierwsza kolumna 
	else if ( vPosition.x > -0.33 && vPosition.x < 0.33) color = vec3( 0.0, 1.0, 1.0 ); // druga kolumna
	else color = vec3( 0.5, 0.5, 0.5 ); // trzecia kolumna
  }
  else // dolny wiersz
  {
	if( vPosition.x < -0.33 ) color = vec3( 0.0, 0.0, 1.0 ); //pierwsza kolumna
	else if ( vPosition.x > -0.33 && vPosition.x < 0.33) color = vec3( 0.0, 0.0, 0.0 ); // druga kolumna
	else color = vec3( 1.0, 0.0, 1.0 ); // trzecia kolumna
  }


    gl_Position = vPosition;
}