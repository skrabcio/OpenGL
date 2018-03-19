#version 140
 
in vec3 color; // kolor interpolowany z shadera wierzcholkow

out vec3 fColor; // wyjsciowy kolor fragmentu
 
void main()
{
	int width = 512; 
	int height = 512;
	float cubeWidth = width/3;
	float cubeHeight = height/3;

  if( gl_FragCoord.y < cubeHeight) // gorny wiersz
  {
	if( gl_FragCoord.x < cubeWidth ) fColor = vec3( 1.0, 0.0, 0.0 ); //pierwsza kolumna
	else if( gl_FragCoord.x > cubeWidth && gl_FragCoord.x < cubeWidth*2 ) fColor = vec3( 1.0, 1.0, 0.0 ); // druga kolumna
	else fColor = vec3( 1.0, 1.0, 1.0 ); // trzecia kolumna
  }
  else if ( gl_FragCoord.y > cubeHeight && gl_FragCoord.y < cubeHeight*2 )  // srodkowy wiersz
  {
	if( gl_FragCoord.x < cubeWidth ) fColor = vec3( 0.0, 1.0, 0.0 ); //pierwsza kolumna 
	else if ( gl_FragCoord.x > cubeWidth && gl_FragCoord.x < cubeWidth*2) fColor = vec3( 0.0, 1.0, 1.0 ); // druga kolumna
	else fColor = vec3( 0.5, 0.5, 0.5 ); // trzecia kolumna
  }
  else // dolny wiersz
  {
	if( gl_FragCoord.x < cubeWidth ) fColor = vec3( 0.0, 0.0, 1.0 ); //pierwsza kolumna
	else if ( gl_FragCoord.x > cubeWidth && gl_FragCoord.x < cubeWidth*2 ) fColor = vec3( 0.0, 0.0, 0.0 ); // druga kolumna
	else fColor = vec3( 1.0, 0.0, 1.0 ); // trzecia kolumna
  }

}