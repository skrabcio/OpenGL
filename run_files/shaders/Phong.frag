#version 140

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

in vec3 position; // pozycja
in vec3 normal;  // normal

out vec4 fColor; // wyjsciowy kolor 
 
void main()
{

	vec3 norm = normalize(normal); // normal

	// obliczenie oœwietlenia 
	vec3 light = normalize(vec3(lightPosition) - position);
	vec3 view = normalize(vec3(0.0, 0.0, 0.0) - position);
	vec3 ambient = lightAmbient * matAmbient;
	vec3 specular = vec3(0.0, 0.0, 0.0);
	vec3 diffuse = lightDiffuse * matDiffuse * max(dot(light, norm), 0.0);

	// obliczenie refleksów œwiat³a
	if(dot(light, view) > 0.0)
	{
		vec3 refl = reflect(vec3(0.0, 0.0, 0.0) - light, norm);
		specular = pow(max(0.0, dot(view, refl)), matShine) * matSpecular * lightSpecular;
	}

	// konwersja na zmienn¹ wyjœciow¹ 
    fColor = vec4(clamp(ambient + diffuse + specular, 0.0, 1.0), 1.0);
}