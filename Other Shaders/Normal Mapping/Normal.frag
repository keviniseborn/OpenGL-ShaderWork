#version 430

uniform sampler2D texDif;
uniform sampler2D texNorm;
in vec2 uvCoord;

in vec3 vertPos;
in vec3 N;
in vec3 lightPos;

in vec3 lightDirection;
in vec3 targetDirection;
in vec3 pos_eye;

uniform vec3 Ka;			// Ambient reflectivity
uniform vec3 Kd;            // Diffuse reflectivity
uniform vec3 Ld;            // Diffuse light intensity
uniform vec3 Ls;			// Specular light intensity
uniform vec3 Ks;			// Specular reflection coefficient


in vec3 TangentSurface2Light;
in vec3 TangentSurface2View;

uniform float ConeAngle;

layout( location = 0 ) out vec4 FragColour;


vec3 calcShading(vec3 normal, vec3 textureColour)
{
	
	
   vec3 L = normalize(TangentSurface2Light); 

	//calculate Ambient Light intensity

   vec3 Ia = 0.2 * textureColour;
   Ia = clamp(Ia, 0.0, 1.0); 

   //calculate Diffuse Light Intensity making sure it is not negative 
  
   vec3 Id = Ld * max(dot(normal,L), 0.0);
   Id = Id * textureColour;
   Id = clamp(Id, 0.0, 1.0);     

    //specular reflection
	vec3 R = -reflect(L,normal);
	vec3 V = normalize(TangentSurface2Light);

	float RV = clamp(dot(R,V),0.0,1.0);

	vec3 Is = vec3(0,0,0);

	if (dot(V,N) > 0.0)
	{
	 Is = Ls * (pow(RV,30));
	}
   
   return Ia + Id + Is;
}

void main() {


	vec3 texColour = vec3(texture(texDif, uvCoord).rgb); //vec3( 0.3, 0.3, 0.3);
	vec3 norm = 2.0 * texture(texNorm, uvCoord).xyz - 1.0;
	norm = normalize(norm);  
	
   FragColour = vec4(calcShading(norm, texColour), 1.0);
 
}
