#version 430

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

uniform float ConeAngle;

layout( location = 0 ) out vec4 FragColour;

void main() {

   //Calculate the light vector

   vec3 L = normalize(lightPos - vertPos);  
   vec3 Fatt = vertPos - lightPos;

   //calculate Ambient Light intensity

   vec3 Ia = 0.2 * Ka;
   Ia = clamp(Ia, 0.0, 1.0); 
    
   //calculate Diffuse Light Intensity making sure it is not negative 
  
   vec3 Id = Ld * max(dot(N,L), 0.0);
   Id = Id * Kd;
   Id = clamp(Id, 0.0, 1.0);     

    //specular reflection
	vec3 R = -reflect(L,N);
	vec3 V = normalize(-vertPos);

	float RV = clamp(dot(R,V),0.0,1.0);

	vec3 Is = vec3(0,0,0);

	if (dot(V,N) > 0.0)
	{
	 Is = Ld * (pow(RV,15));
	}
	
	
	float attenuation;
	float lightToSurfaceAngle = dot(normalize(-lightDirection), normalize(targetDirection));

	attenuation = 1.0 / (1.0 + attenuation * pow(length(lightDirection), 2));

		if(degrees(acos(lightToSurfaceAngle)) > ConeAngle)
		{
			attenuation = 0.0;
			FragColour = vec4(Ia,0.0);
		}else{ 
			attenuation = pow(lightToSurfaceAngle, 150);
			FragColour = vec4(Ia,1.0) + (attenuation*(vec4(Id, 1.0) + vec4(Is,1.0))); 
		}
	
}
