#version 140
#extension GL_ARB_explicit_attrib_location : enable
 
uniform mat4 modelViewMatrix; // macierze modelu
uniform mat3 normalMatrix; // macierz normali
uniform mat4 projectionMatrix; // macierz projekcji

//parametry œwiat³a
uniform vec4 lightPosition; 
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;

// parametry materia³u
uniform vec3 matAmbient; 
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float matShine;
 
layout (location = 0) in vec3 vPosition; //  pozycja
layout (location = 1) in vec3 vNormal; //normale

out vec3 color;  // kolor
 
void main()
{
	
	vec3 position = vec3( modelViewMatrix * vec4( vPosition, 1.0 ) ); //obliczanie wierzcho³ków do modelu widoku

	//obliczenie œwiat³a 
	vec3 normal = normalize( normalMatrix * vNormal );
	vec3 light = normalize( vec3( lightPosition ) - position );
	vec3 view = normalize( vec3( 0.0, 0.0, 0.0 ) - position );
	vec3 ambient = lightAmbient * matAmbient;
	vec3 diffuse = lightDiffuse * matDiffuse * max( dot( light, normal ), 0.0 );
	vec3 specular = vec3( 0.0, 0.0, 0.0 );
	vec3 Vec = normalize(light + view);

	//wyliczenie refleksów œwiat³a
	if( dot( light, view ) > 0.0 )
	{
		vec3 refl = reflect( vec3( 0.0, 0.0, 0.0 ) - light, normal );

		specular = lightSpecular * matSpecular * exp(-matShine*(1-pow(dot(normal, Vec), 2))/pow(dot(normal, Vec), 2));
	}

	// konwersje do zmiennych wyjœciowych
	color = clamp( ambient + diffuse + specular, 0.0, 1.0 );
    gl_Position = projectionMatrix * modelViewMatrix * vec4( vPosition, 1.0 );
}